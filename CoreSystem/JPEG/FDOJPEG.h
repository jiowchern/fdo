//
// FDOJPEG.h
//
#ifndef __FDOJPEG_h__
#define __FDOJPEG_h__

namespace FDO {

bool CompressToJPEG(
    const char* infilename, const char* outfilename, int iQuality, std::string& out_strErr);

bool TGA_To_JPEG(
    const char* infilename, const char* outfilename, int iQuality, std::string& out_strErr);

} // namespace FDO 

#endif // __FDOJPEG_h__

