/*
 * FDOJPEG.cpp - a modified version of scjpeg.cpp which is modified from cjpeg.c
 *
 * cjpeg.c:
 * Copyright (C) 1991-1998, Thomas G. Lane.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying README file.
 *
 */
#include "stdafx.h"
#pragma hdrstop
//---------------------------------------------------------------------------
#include "cdjpeg.h"		/* Common decls for cjpeg/djpeg applications */
#include "jversion.h"		/* for version message */
#include <string>
#include "FDOJPEG.h"

#define TARGA_SUPPORTED

/* Create the add-on message string table. */

#define JMESSAGE(code,string)	string ,

static const char * const cdjpeg_message_table[] = {
#include "cderror.h"
  NULL
};


/*
 * This routine determines what format the input file is,
 * and selects the appropriate input-reading module.
 *
 * To determine which family of input formats the file belongs to,
 * we may look only at the first byte of the file, since C does not
 * guarantee that more than one character can be pushed back with ungetc.
 * Looking at additional bytes would require one of these approaches:
 *     1) assume we can fseek() the input file (fails for piped input);
 *     2) assume we can push back more than one character (works in
 *        some C implementations, but unportable);
 *     3) provide our own buffering (breaks input readers that want to use
 *        stdio directly, such as the RLE library);
 * or  4) don't put back the data, and modify the input_init methods to assume
 *        they start reading after the start of file (also breaks RLE library).
 * #1 is attractive for MS-DOS but is untenable on Unix.
 *
 * The most portable solution for file types that can't be identified by their
 * first byte is to make the user tell us what they are.  This is also the
 * only approach for "raw" file types that contain only arbitrary values.
 * We presently apply this method for Targa files.  Most of the time Targa
 * files start with 0x00, so we recognize that case.  Potentially, however,
 * a Targa file could start with any byte value (byte 0 is the length of the
 * seldom-used ID field), so we provide a switch to force Targa input mode.
 */

static boolean is_targa;	/* records user -targa switch */


LOCAL(cjpeg_source_ptr)
select_file_type (j_compress_ptr cinfo, FILE * infile)
{
  int c;

  if (is_targa) {
#ifdef TARGA_SUPPORTED
    return jinit_read_targa(cinfo);
#else
    ERREXIT(cinfo, JERR_TGA_NOTCOMP);
#endif
  }

  if ((c = getc(infile)) == EOF)
    ERREXIT(cinfo, JERR_INPUT_EMPTY);
  if (ungetc(c, infile) == EOF)
    ERREXIT(cinfo, JERR_UNGETC_FAILED);

  switch (c) {
#ifdef BMP_SUPPORTED
  case 'B':
    return jinit_read_bmp(cinfo);
#endif
#ifdef GIF_SUPPORTED
  case 'G':
    return jinit_read_gif(cinfo);
#endif
#ifdef PPM_SUPPORTED
  case 'P':
    return jinit_read_ppm(cinfo);
#endif
#ifdef RLE_SUPPORTED
  case 'R':
    return jinit_read_rle(cinfo);
#endif
#ifdef TARGA_SUPPORTED
  case 0x00:
    return jinit_read_targa(cinfo);
#endif
  default:
    ERREXIT(cinfo, JERR_UNKNOWN_FORMAT);
    break;
  }

  return NULL;			/* suppress compiler warnings */
}

//==========================================================================================
// internal_compress_to_jpeg()
//==========================================================================================
static 
void internal_compress_to_jpeg(const char* infilename, const char* outfilename, int iQuality)
{
  struct jpeg_compress_struct cinfo;
  struct jpeg_error_mgr jerr;
#ifdef PROGRESS_REPORT
  struct cdjpeg_progress_mgr progress;
#endif
  cjpeg_source_ptr src_mgr;
  FILE * input_file;
  FILE * output_file;
  JDIMENSION num_scanlines;

  /* Initialize the JPEG compression object with default error handling. */
  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_compress(&cinfo);
  /* Add some application-specific error messages (from cderror.h) */
  jerr.addon_message_table = cdjpeg_message_table;
  jerr.first_addon_message = JMSG_FIRSTADDONCODE;
  jerr.last_addon_message = JMSG_LASTADDONCODE;

  /* Initialize JPEG parameters.
   * Much of this may be overridden later.
   * In particular, we don't yet know the input file's color space,
   * but we need to provide some value for jpeg_set_defaults() to work.
   */
  cinfo.in_color_space = JCS_RGB; /* arbitrary guess */
  jpeg_set_defaults(&cinfo);

  /* Open the input file. */
  static char errmsg[128];
  if ((input_file = fopen(infilename, READ_BINARY)) == NULL) {
    sprintf(errmsg, "SaveToScreen: can't open %s\n", infilename);
    throw(errmsg);
  }

  /* Open the output file. */
  if ((output_file = fopen(outfilename, WRITE_BINARY)) == NULL) {
    sprintf(errmsg, "SaveToScreen: can't open %s\n", outfilename);
    throw(errmsg);
  }

#ifdef PROGRESS_REPORT
  start_progress_monitor((j_common_ptr) &cinfo, &progress);
#endif

  /* Figure out the input file format, and set up to read it. */
  src_mgr = select_file_type(&cinfo, input_file);
  src_mgr->input_file = input_file;

  /* Read the input file header to obtain file size & colorspace. */
  (*src_mgr->start_input) (&cinfo, src_mgr);

  /* Now that we know input colorspace, fix colorspace-dependent defaults */
  jpeg_default_colorspace(&cinfo);

  /* Adjust default compression parameters by re-parsing the options */
  jpeg_set_quality(&cinfo, iQuality, TRUE);

  /* Specify data destination for compression */
  jpeg_stdio_dest(&cinfo, output_file);

  /* Start compressor */
  jpeg_start_compress(&cinfo, TRUE);

  /* Process data */
  while (cinfo.next_scanline < cinfo.image_height) {
    num_scanlines = (*src_mgr->get_pixel_rows) (&cinfo, src_mgr);
    (void) jpeg_write_scanlines(&cinfo, src_mgr->buffer, num_scanlines);
  }

  /* Finish compression and release memory */
  (*src_mgr->finish_input) (&cinfo, src_mgr);
  jpeg_finish_compress(&cinfo);
  jpeg_destroy_compress(&cinfo);

  /* Close files, if we opened them */
  if (input_file != stdin)
    fclose(input_file);
  if (output_file != stdout)
    fclose(output_file);

#ifdef PROGRESS_REPORT
  end_progress_monitor((j_common_ptr) &cinfo);
#endif

  /* All done. */
  //exit(jerr.num_warnings ? EXIT_WARNING : EXIT_SUCCESS);
  //return 0;			/* suppress no-return-value warnings */
}

//==========================================================================================
// FDO::CompressToJPEG() - wrapper of internal_compress_to_jpeg()
//==========================================================================================

bool FDO::CompressToJPEG(
    const char* infilename, const char* outfilename, int iQuality, std::string& out_strErr)
{
    try {
        is_targa = FALSE;
        internal_compress_to_jpeg(infilename, outfilename, iQuality);
        return true;
    }
    catch(const char* errmsg) {
        out_strErr = errmsg;
        return false;
    }
}

bool FDO::TGA_To_JPEG(
    const char* infilename, const char* outfilename, int iQuality, std::string& out_strErr)
{
    try {
        is_targa = TRUE;
        internal_compress_to_jpeg(infilename, outfilename, iQuality);
        return true;
    }
    catch(const char* errmsg) {
        out_strErr = errmsg;
        return false;
    }
}

/*
//==========================================================================================
// main() - Demonstrate how to use FDO::CompressToJPEG().
//==========================================================================================

#include "stdlib.h"

int main(int argc, char **argv)
{
    const char* szInputFile = "fdo.bmp";
    const char* szOutputFile = "result.jpg";
    int iQuality = 50;
    std::string strErr;
    bool bResult = FDO::CompressToJPEG(szInputFile, szOutputFile, iQuality, strErr);
    if (false == bResult) {
        printf("%s\n", strErr.c_str());
        system("pause");
    }
    return 0;
}
*/

