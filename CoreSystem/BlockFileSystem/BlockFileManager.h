/*
  Filename    : BlockFileManager.H
  Author      : Curtis Lo
  Date        : 2003/09/17
  Descirption : Header file of block file manager
*/
// Avoid recurrence include this file, Start condition compile
#ifndef __BlockFileManager_H
#define __BlockFileManager_H

 // Include private header files
 #include "VirtualDirectoryManager.h"
 #include "BlockMemoryPool.h"
 #include "Callbacks.h"
 #include "Defines.h"
 #include <stdio.h>

 #ifdef __BORLANDC__
  // Set data member's align byte to 1 bytes
  #pragma option push
  #pragma option -a1
 #else
  // Adjust structure alignment to 1 byte ( default is 8 byte )
  #pragma pack( push )
  #pragma pack( 1 )
 #endif

 // Declare structure of block file manager's file header
 struct _CBfmFileHeader
  {
   // Data member of compress block size
   int m_CompressBlockSize;
   // Data member of compress level
   int m_CompressLevel;
  };

 #ifdef __BORLANDC__
  // Back to default alignment
  #pragma option pop
 #else
  // Back to default alignment
  #pragma pack( pop )
 #endif

 // Declare class of file importer
 class CFileImporter
  {
   private:
    // Data member of block size
    int m_BlockSize;
    // Data member of compress block size
    int m_CompressBlockSize;
    // Data member of compress level
	int m_CompressLevel;
	// Data member of file compress flag
    bool m_FileCompressFlag;
    // Data member of file handle
	//int m_FileHandle;
    // Data member of file size
    int m_FileSize;
	// Data member of compress size
    int m_CompressSize;
    // Data member of crc32
    unsigned long m_Crc32;
    // Data member of offset table
    long *m_pOffsetTable;
    int  m_OffsetNumber;
    // Data member of work buffer
    char *m_pFileBlock;
    char *m_pCompressBlock;
    // Data member of remain size
	int m_RemainSize;
    // Data member of block index
	int m_BlockIndex;

	FILE* m_pFile;

   public:
    // Constructor
    CFileImporter( void );
    // Destructor
    ~CFileImporter( void );
    // Function member of startup file importer ( CompressLevel = -1, not compress )
    bool StartupFileImporter( int BlockSize, int CompressBlockSize, int CompressLevel );
    // Function member of shutdown file importer
    void ShutdownFileImporter( void );
    // Function member of import file
    bool ImportFile( char *pFilename, bool CompressFlag );
    // Function member of import block ( if DataSize == 0, then read end. )
    bool ImportBlock( char **pDataBuffer, int *pDataSize );
    // Function member of get file compress flag
    bool GetFileCompressFlag( void )    { return m_FileCompressFlag; }
    // Function member of get file size
    int GetFileSize( void )             { return m_FileSize; }
    int GetCompressSize( void )         { return m_CompressSize; }
    // Function member of get crc32 value
    unsigned long GetCrc32( void )      { return m_Crc32; }
    // Function member of get offset table
    long *GetOffsetTable( void )        { return m_pOffsetTable; }
    int  GetOffsetNumber( void )        { return m_OffsetNumber; }
  };

 // Declare class of compress file reader         lseek
 class CCompressFileReader
  {                                                
   private:
    // Data member of system parameter
    int m_CompressBlockSize;
    // Data member of max. number of file compress block
    int m_MaxCompressBlockNumber;
    // Data member of offset table of compress block
    long *m_pOffsetTable;
    long m_OffsetTableSize;
    // Data member of work block buffer
    char *m_pUncompressBlock;
    char *m_pCompressBlock;
    // Data member of work file handle
    int m_WorkFileHandle;
    // Data member of handle of block memory pool
    int m_BmpHandle;
    // Data member of file size
    int m_FileOriginSize;
    int m_FilePhysicalSize;
    // Data member of file compress block number
    int m_FileCompressBlockNumber;
    // Data member of virtual directory manager
    CVirtualDirectoryManager *m_pVirtualDirectoryManager;
    // Data member of block memory pool
    CBlockMemoryPool *m_pBlockMemoryPool;
    // Function member of get compress block offset
    int GetCompressBlockOffset( int BlockIndex );
    // Function member of get compress block size
    int GetCompressBlockSize( int BlockIndex );
    // Function member of uncompress block
    bool UncompressBlock( int BlockIndex );
   public:
    // Constructor
    CCompressFileReader( void );
    // Destructor
    ~CCompressFileReader( void );
    // Function member of startup class
    bool StartupCompressFileReader(
                                   CVirtualDirectoryManager *pVirtualDirectoryManager,
                                   CBlockMemoryPool         *pBlockMemoryPool,
                                   int                      CompressBlockSize
                                  );
    // Function member of shutdown class
    void ShutdownCompressFileReader( void );
    // Function member of bind work file
    bool BindWorkFile( int Handle );
    // Function member of unbink work file
    void UnbindWorkFile( void )         { m_WorkFileHandle = -1; }
    // Function member of get work file handle
    int GetWorkFileHandle( void )       { return m_WorkFileHandle; }
    // Function member of read data
    bool ReadData( int Position, int Size, char *pDataBuffer );
  };

 // Declare class of block file manager
 class CBlockFileManager
  {
   private:
    // Data member of current path ( node index )
    int m_CurrentPathHandle;
    // Data member of system parameter
    int m_BlockSize;
    int m_CompressBlockSize;
    int m_CompressLevel;
    // Data member of import file count
    int m_ImportFileCount;
    int m_TotalFileNumber;
    // Data member of virtual directory manager
    CVirtualDirectoryManager *m_pVirtualDirectoryManager;
    // Data member of block memory pool
    CBlockMemoryPool *m_pBlockMemoryPool;
    // Data member of compress file reader
    CCompressFileReader *m_pCompressFileReader;
    // Data member of file importer
    CFileImporter *m_pFileImporter;
    // Data member of callback functions
    _DemcbFunction m_pDisplayErrorFunction;
    _DptcbFunction m_pDisplayProgressTextFunction;
    _DppcbFunction m_pDisplayProgressPercentFunction;
    // Function member of import file
    bool ImportFile( CVirtualFileInfoNode *pFileNode );
    // Function member of import directory
    bool ImportDirectory( CVirtualFileInfoNode *pParentNode );
    // Function member of export file
    bool ExportFile( CVirtualFileInfoNode *pFileNode );
    // Function member of export directory
    bool ExportDirectory( CVirtualFileInfoNode *pParentNode );
   public:
    // Constructor and destructor
    CBlockFileManager( void );
    ~CBlockFileManager( void );
    // Function member of load block file manager's data file
    bool LoadBfmFile( const char *pFilename, int ExpandFileNumber = 0, int ExpandBlockNumber = 0 );

	// Function member of save block file manager's data file
	bool SaveBfmFile( char *pFilename, bool CompactModeFlag );

	// Funciton member of startup block file manager
    bool StartupBlockFileManager(
                                 int  FileInfoNumber,
                                 int  HandleNumber,     // must >= 1
								 int  BlockNumber,      // must >= 1
                                 int  BlockSize,        // must >= 4096
								 int  CompressBlockSize,// Compress block size, must > 1024k, 0 = not compress mode,default=8192
                                 int  CompressLevel,    // Compress level, for zlib compress lib
                                 int  SplitSize,        // Split size, must >= 256, 0 = Not split mode
                                 char *pFilename        // Null = memory mapping mode
                                );
    // Function member of shutdown block file manager
    void ShutdownBlockFileManager( void );
    // Function member of get virtual directory manager
    CVirtualDirectoryManager *GetVirtualDirectoryManager( void )
     { return m_pVirtualDirectoryManager; }
    // Function member of get file handle of VDM ( NULL = error ), pFilename = directory or file name
    int GetVdmFileHandle( char *pFilename );
    // Function member of get file info node ( NULL = error )
    CVirtualFileInfoNode *GetVdmFileInfoNode( int Handle );
    // Function member of get block memory pool
    CBlockMemoryPool *GetBlockMemoryPool( void )        { return m_pBlockMemoryPool; }
    // Function member of set current path ( \ = root directory )
    bool SetCurrentPath( char *pFilename ); //必須傳入絕對路徑
    // Function member of get current path
    void GetCurrentPath( char *pFilename, bool RealPathFlag );
    // Function member of create directory ( false = error, not allow assign-dir. )
    bool CreateDirectoryB( char *pFilename );
    // Function member of create file ( return file handle, -1 = error, not allow assign-dir. )
    int CreateFileB( char *pFilename );
    // Function member of open file ( return file handle, -1 = error )
	int OpenFile( const char *pFilename, bool AutoUncompresFlag );
    // Function member of close file
    bool CloseFile( int Handle );
    // Function member of get file name
    bool GetFilename( int Handle, char *pFilename, bool FullPathFlag, bool RealPathFlag );
    // Function member of get file size
    int GetFileSize( int Handle );
    // Function member of adjust file size ( Only ForceAdjustFlag = true, can adjust NOT-WRITABLE file
    bool AdjustFileSize( int Handle, int Size, bool ForceAdjustFlag = false );
    // Function member of get file position ( -1 == error )
    int GetFilePosition( int Handle );
    // Function member of set file position
    bool SetFilePosition( int Handle, int Position );
    // Function member of read data
    bool ReadData( int Handle, int Size, void *pDataBuffer );
    // Function member of write data ( Only ForceWriteFlag = true, can write NOT-WRITABLE file )
    bool WriteData( int Handle, int Size, void *pDataBuffer, bool ForceWriteFlag = false );
    // Function member of flush block file manager
    bool FlushBlockFileManager( void ); //存取完畢釋放block memory
    // Function member of check files optimize mode ( check all files )
    void CheckFilesOptimizeMode( void );
    // Function member of import files ( import all files )
    bool ImportFiles(
                     _DemcbFunction pDisplayErrorFunction,
                     _DptcbFunction pDisplayProgressTextFunction,
                     _DppcbFunction pDisplayProgressPercentFunction
                    );
    // Function member of export files ( export all files )
    bool ExportFiles( char *pTargetDirectory );
  };

// End condition compile
#endif