/*
  Filename    : BlockFileManager.Cpp
  Author      : Curtis Lo
  Date        : 2003/09/17
  Descirption : Body file of
*/
// Include standard header files
#include "stdafx.h"
#include <Sys\Stat.h>
#include <Stdlib.h>
#include <String.h>
#include <Fcntl.h>
#include <io.h>

// Include private header files
#include "BlockFileManager.h"
#include "Zlib\\Zlib.h"

//----------------------------CFileImporter-------------------------------------

// Constructor
CFileImporter::CFileImporter( void )
 {
  // Setup all data member to default value
  m_BlockSize      = 0;
  m_CompressLevel  = -1;
  m_FileSize       = 0;
  m_CompressSize   = 0;
  m_pOffsetTable   = NULL;
  m_OffsetNumber   = 0;
  m_pFileBlock     = NULL;
  m_pCompressBlock = NULL;
  m_pFile          = NULL;
 }

// Destructor
CFileImporter::~CFileImporter( void )
 {
  // Shutdown file importer
  ShutdownFileImporter();
 }

// Function member of startup file importer ( CompressLevel = -1, not compress )
bool CFileImporter::StartupFileImporter(
                                        int BlockSize,
                                        int CompressBlockSize,
                                        int CompressLevel
                                       )
 {
  // Define local variable - alloc size
  int AllocSize;

  // Store argument to data member
  m_BlockSize         = BlockSize;
  m_CompressBlockSize = CompressBlockSize;
  m_CompressLevel     = CompressLevel;
  // Convert to zlib's compress level, 0 = not compress, 1 = fast, 9 = best
  if( m_CompressLevel != -1 )
   {
    // Analy level
    switch( m_CompressLevel )
     {
      // Fast
      case 0 : m_CompressLevel = 1;
               break;
      // Balance
      case 1 : m_CompressLevel = 5;
               break;
      // Best
      case 2 : m_CompressLevel = 9;
               break;
     }
   }
  // Calculate alloc size
  if( m_BlockSize >= m_CompressBlockSize )
   AllocSize = m_BlockSize;
  else
   AllocSize = m_CompressBlockSize;
  // Allocate file block buffer
  m_pFileBlock = (char *)malloc( AllocSize );
  if( m_pFileBlock == NULL )
   return false;
  // Allow compress operation ?
  if( CompressLevel != -1 )
   {
   // Allocate compress block buffer ( for zlib's request )
    m_pCompressBlock = (char *)malloc( AllocSize + (AllocSize / 10) + 12 );
    if( m_pCompressBlock == NULL )
     return false;
   }
  // Startup file importer successfully
  return true;
 }

// Function member of shutdown file importer
void CFileImporter::ShutdownFileImporter( void )
 {
  // Need close file ?
  if( m_pFile != NULL )
   {
	// Close file handle
	fclose( m_pFile );
	m_pFile = NULL;
    // Need free offset table
    if( m_pOffsetTable != NULL )
     {
      // Free offset table
      free( m_pOffsetTable );
     }
    // Clear data member to default value  
    m_FileSize     = 0;
    m_CompressSize = 0;
    m_pOffsetTable = NULL;
    m_OffsetNumber = 0;
   }
  // Need free file block buffer
  if( m_pFileBlock != NULL )
   {
    // Free buffer
    free( m_pFileBlock );
    // Clear pointer
    m_pFileBlock = NULL;
   }
  // Need free compress block buffer
  if( m_pCompressBlock != NULL )
   {
    // Free buffer
    free( m_pCompressBlock );
    // Clear pointer
	m_pCompressBlock = NULL;
   }
 }

// Function member of import file
bool CFileImporter::ImportFile( char *pFilename, bool CompressFlag )
 {
  // File alreay open ?
   if( m_pFile != NULL )
   {
	// Close file handle
	fclose( m_pFile );
	m_pFile = NULL;
	// Need free offset table
	if( m_pOffsetTable != NULL )
     {
      // Free offset table
      free( m_pOffsetTable );
     }
	// Clear data member to default value  
    m_FileSize     = 0;
    m_CompressSize = 0;
    m_pOffsetTable = NULL;
    m_OffsetNumber = 0;
   }
  // Store argument to data member
  m_FileCompressFlag = CompressFlag;
  // Allow compress ?
  if( m_CompressLevel == -1 )
   {
    // Force disable compress flag
    m_FileCompressFlag = false;
   }
  // Initialize crc32 value
  m_Crc32 = adler32( 0L, Z_NULL, 0 );

  // Open file
  if( ( m_pFile = fopen( pFilename, "rb" ) ) == 0 )
		return false;

  // Get file length
  m_FileSize = filelength( fileno( m_pFile ) );

  // Store remain size
  m_RemainSize = m_FileSize;
  // Clear block index
  m_BlockIndex = 0;
  // In compress mode, need allocate offset table ?
  if( m_FileCompressFlag == true )
   {
    // Calculate offset number
    m_OffsetNumber = m_FileSize / m_CompressBlockSize;
    if( m_FileSize % m_CompressBlockSize )      // Remainder ?
     m_OffsetNumber++;
    // Valid number ?
    if( m_OffsetNumber != 0 )
     {
      // Allocate offset table
      m_pOffsetTable = (long *)malloc( m_OffsetNumber * sizeof( long ) );
      if( m_pOffsetTable == NULL )
       return false;
      // Clear offset table
      memset( m_pOffsetTable, 0, m_OffsetNumber * sizeof( long ) );
     }
   }
  // Import file successfully
  return true;
 }

// Function member of import block ( if DataSize == 0, then read end. )
bool CFileImporter::ImportBlock( char **pDataBuffer, int *pDataSize )
 {
  // Define local variable - compress size
  int CompressSize;
  // Define local variable - read size
  unsigned int ReadSize;
  // Define local variable - block size
  int BlockSize;

  // Open file ?
  if( m_pFile == NULL )
   return false;

  // Allow compress ? if ok, then using compress block size
  if( m_FileCompressFlag == true )
   BlockSize = m_CompressBlockSize;
  else
   BlockSize = m_BlockSize;
  // Calculate read size
  if( m_RemainSize >= BlockSize )
   ReadSize = BlockSize;
  else
   ReadSize = m_RemainSize;
  // End of file ?
  if( ReadSize == 0 )
   {
    // Store return value
    *pDataBuffer = NULL;
    *pDataSize   = 0;
    // Immediate return
    return true;
   }
  // Read block size
  if( fread( m_pFileBlock, 1, ReadSize, m_pFile ) != ReadSize )
	return false;

  // Need compress ?
  if( m_FileCompressFlag == true )
   {
    // Compress data
    if( compress2( (unsigned char *)m_pCompressBlock,
                   (unsigned long *)&CompressSize,
                   (unsigned char *)m_pFileBlock,
                   (unsigned long)ReadSize,
                   m_CompressLevel ) != Z_OK )
     return false;
    // Store return value
    *pDataBuffer = m_pCompressBlock;
    *pDataSize   = CompressSize;
    // Store offset value
    *(m_pOffsetTable + m_BlockIndex) = m_CompressSize;
    // Add to total compress size
    m_CompressSize += CompressSize;
    // Increment block index
    m_BlockIndex++;
   }
  else          // Normal mode
   {
    // Store return value
    *pDataBuffer = m_pFileBlock;
    *pDataSize   = ReadSize;
   }
  // Calculate crc32
  m_Crc32 = adler32( m_Crc32, (unsigned char *)*pDataBuffer, *pDataSize );
  // Adjust remain size
  m_RemainSize -= ReadSize;
  // Import block successfully
  return true;
 }

//---------------------------CCompressFileReader--------------------------------

// Constructor
CCompressFileReader::CCompressFileReader( void )
 {
  // Clear pointer of offset table
  m_pOffsetTable = NULL;
  // Clear pointer of work block
  m_pUncompressBlock = NULL;
  m_pCompressBlock   = NULL;
  // Clear work file handle
  m_WorkFileHandle = -1;
 }

// Destructor
CCompressFileReader::~CCompressFileReader( void )
 {
  // Shutdown class
  ShutdownCompressFileReader();
 }

// Function member of get compress block offset
int CCompressFileReader::GetCompressBlockOffset( int BlockIndex )
 {
  // Return offset value of block
  return *(m_pOffsetTable + BlockIndex);
 }

// Function member of get compress block size
int CCompressFileReader::GetCompressBlockSize( int BlockIndex )
 {
  // Last block ?
  if( (BlockIndex + 1) == m_FileCompressBlockNumber )
   {
    // Return size
    return (m_FilePhysicalSize - *(m_pOffsetTable + BlockIndex))-m_OffsetTableSize;
   }
  else  // Other block
   {
    // Return size
    return (*(m_pOffsetTable + BlockIndex + 1) - *(m_pOffsetTable + BlockIndex));
   }
 }

// Function member of uncompress block
bool CCompressFileReader::UncompressBlock( int BlockIndex )
 {
  // Define local variable - uncompress size
  uLong UncompressSize;

  // Read compress data
  if( m_pBlockMemoryPool->ReadData( m_BmpHandle,
                                    m_OffsetTableSize + GetCompressBlockOffset( BlockIndex ),
                                    GetCompressBlockSize( BlockIndex ),
                                    m_pCompressBlock ) == false )
   return false;
  // Uncompress it
  UncompressSize = m_CompressBlockSize;
  if( uncompress( (Bytef *)m_pUncompressBlock,
                  &UncompressSize,
                  (Bytef *)m_pCompressBlock,
                  GetCompressBlockSize( BlockIndex ) ) != Z_OK )
   return false;
  // Uncompress block successfully
  return true;
 }

// Function member of startup class
bool CCompressFileReader::StartupCompressFileReader(
                                                    CVirtualDirectoryManager *pVirtualDirectoryManager,
                                                    CBlockMemoryPool         *pBlockMemoryPool,
                                                    int                      CompressBlockSize
                                                   )
 {
  // Store argument to data member
  m_pVirtualDirectoryManager = pVirtualDirectoryManager;
  m_pBlockMemoryPool         = pBlockMemoryPool;
  m_CompressBlockSize        = CompressBlockSize;
  // Allocate offset table
  m_MaxCompressBlockNumber = pVirtualDirectoryManager->GetMaxNumberOfCompressBlock();
  // Need alloc table ?
  if( m_MaxCompressBlockNumber != 0 )
   {
    // Allocate memory
    m_pOffsetTable = (long *)malloc( sizeof( long ) * m_MaxCompressBlockNumber );
    // Error ?
    if( m_pOffsetTable == NULL )
     return false;
   }
  // Allocate compress block buffer ( for zlib's request )
  m_pUncompressBlock = (char *)malloc( CompressBlockSize + 12 );
  if( m_pUncompressBlock == NULL )
   return false;
  // Allocate compress block buffer ( for zlib's request )
  m_pCompressBlock = (char *)malloc( CompressBlockSize + (CompressBlockSize / 10) + 12 );
  if( m_pCompressBlock == NULL )
   return false;
  // Startup class successfully
  return true;
 }

// Function member of shutdown class
void CCompressFileReader::ShutdownCompressFileReader( void )
 {
  // Need free offset table ?
  if( m_pOffsetTable != NULL )
   {
    // Free it
    free( m_pOffsetTable );
    // Clear it
    m_pOffsetTable = NULL;
   }
  // Need free uncompress block ?
  if( m_pUncompressBlock != NULL )
   {
    // Free it
    free( m_pUncompressBlock );
    // Clear it
    m_pUncompressBlock = NULL;
   }
  // Need free compress block ?
  if( m_pCompressBlock != NULL )
   {
    // Free it
    free( m_pCompressBlock );
    // Clear it
    m_pCompressBlock = NULL;
   }
 }

// Function member of bind file handle
bool CCompressFileReader::BindWorkFile( int Handle )
 {
  // Define local variable - node pointer
  CVirtualFileInfoNode *pNode;
  // Define local variable - bmp handle
  int BmpHandle;

  // Is same work file ?
  if( m_WorkFileHandle == Handle )
   return true;
  // Get file info node
  pNode = m_pVirtualDirectoryManager->GetFileInfoNode( Handle );
  // Invalid handle ?
  if( pNode == NULL )
   return false;
  // File is close ?
  if( pNode->GetFileOpenFlag() == false )
   return false;
  // Is compressed file ?
  if( pNode->IsCompressedFile() == false )
   return false;
  // Too much comress block ?
  if( pNode->GetFileCompressBlockNumber() > m_MaxCompressBlockNumber )
   return false;
  // Get bmp handle
  BmpHandle = pNode->GetBmpHandle();
  // Invalid handle ?
  if( BmpHandle == -1 )
   return false;
  // Calculate size of offset table
  m_OffsetTableSize = pNode->GetFileCompressBlockNumber() * sizeof( long );
  // Read from block memory pool
  if( m_pBlockMemoryPool->ReadData( BmpHandle,
                                    0,
                                    m_OffsetTableSize,
                                    m_pOffsetTable ) == false )
   return false;
  // Store work file handle to data member
  m_WorkFileHandle          = Handle;
  m_BmpHandle               = pNode->GetBmpHandle();
  m_FileOriginSize          = pNode->GetFileOriginSize();
  m_FilePhysicalSize        = pNode->GetFilePhysicalSize();
  m_FileCompressBlockNumber = pNode->GetFileCompressBlockNumber();
  // Bind work file successfully
  return true;
 }

// Function member of read data
bool CCompressFileReader::ReadData( int Position, int Size, char *pDataBuffer )
 {
  // Define local variable - remain size
  int RemainSize;
  // Define local variiable - read control
  int BlockIndex;
  int BlockOffset;
  int ValidSize;

  // Position ok ?
  if( (Position < 0) || (Position >= m_FileOriginSize) )
   return false;
  // Size ok ?
  if( Size < 0 )
   return false;
  // Overflow ?
  if( (Position + Size) > m_FileOriginSize )
   return false;
  // Store remain size
  RemainSize = Size;
  // Calculate first block
  BlockIndex  = Position / m_CompressBlockSize;
  BlockOffset = Position % m_CompressBlockSize;
  ValidSize   = m_CompressBlockSize - BlockOffset;
  // Overflow ?
  if( ValidSize > RemainSize )
   ValidSize = RemainSize;
  // Read all data
  while( ValidSize )
   {
    // Uncompress block
    if( UncompressBlock( BlockIndex ) == false )
     return false;
    // Copy valid data
    memcpy( pDataBuffer, m_pUncompressBlock + BlockOffset, ValidSize );
    // Adjust target pointer
    pDataBuffer += ValidSize;
    // Adjust remain size
    RemainSize -= ValidSize;
    // Adjust read control
    BlockIndex++;
    BlockOffset = 0;
    if( RemainSize >= m_CompressBlockSize )
     ValidSize = m_CompressBlockSize;
    else
     ValidSize = RemainSize;
   }
  // Read data successfully
  return true;
 }

//----------------------------CBlockFileManager---------------------------------

// Constructor
CBlockFileManager::CBlockFileManager( void )
 {
  // Setup all data members
  m_CurrentPathHandle        = 0;               // Root path
  m_pVirtualDirectoryManager = NULL;
  m_pBlockMemoryPool         = NULL;
  m_pCompressFileReader      = NULL;
  m_pFileImporter            = NULL;
 }

// Destructor
CBlockFileManager::~CBlockFileManager( void )
 {
  // Shutdown block file manager
  ShutdownBlockFileManager();
 }

// Function member of import file
bool CBlockFileManager::ImportFile( CVirtualFileInfoNode *pFileNode )
 {
  // Define local variable - filename buffer
  char pFilename[ _MaxFilenameSize + 1 ];
  // Define local variable - file handle
  int FileHandle;
  // Define local variable - data buffer
  char *pDataBuffer;
  int DataSize;

  // Get full filename ( virtual directory )
  pFileNode->GetFullFilename( pFilename, false );
  // Open file ( always success )
  FileHandle = OpenFile( pFilename, false );
  // Get full filename ( real directory )
  pFileNode->GetFullFilename( pFilename, true );
  // Import file by importer
  if( m_pFileImporter->ImportFile( pFilename, true ) == false )
   return false;
  // Empty file ?
  if( m_pFileImporter->GetFileSize() == 0 )
   return true;
  // Is compress mode, write offset table
  if( m_pFileImporter->GetOffsetNumber() > 0 )
   {
    // Write compress block offset value
    if( WriteData( FileHandle,
                   m_pFileImporter->GetOffsetNumber() * sizeof( long ),
                   m_pFileImporter->GetOffsetTable() ) == false )
     return false;
   }
  // Import file data to archive file
  while( 1 )
   {
    // Import block
    if( m_pFileImporter->ImportBlock( &pDataBuffer, &DataSize ) == false )
     return false;
    // End of file ?
    if( DataSize == 0 )
     break;
    // Write to archive file
    if( WriteData( FileHandle, DataSize, pDataBuffer ) == false )
     return false;
   }
  // Store general file info to file node
  pFileNode->SetFileOriginSize( m_pFileImporter->GetFileSize() );
  // Is compressed mode ?
  if( m_pFileImporter->GetOffsetNumber() > 0 )
   {
    // Move to start position of offset table
    SetFilePosition( FileHandle, 0 );
    // Write compress block offset value
    if( WriteData( FileHandle,
                   m_pFileImporter->GetOffsetNumber() * sizeof( long ),
                   m_pFileImporter->GetOffsetTable() ) == false )
     return false;
    // Setup compress info to file node
    pFileNode->SetFileCompressBlockNumber( m_pFileImporter->GetOffsetNumber() );
   }
  // Close this file
  CloseFile( FileHandle );
  // Import file successfully
  return true;
 }

// Function member of import directory
bool CBlockFileManager::ImportDirectory( CVirtualFileInfoNode *pParentNode )
 {
  // Define local variable - file info node pointers
  CVirtualFileInfoNode *pWorkNode;
  // Define local variable - text buffer
  char TextBuffer[ _MaxFilenameSize + 1 ];

  // Scan all child node
  pWorkNode = pParentNode->GetFirstChildNode();
  while( pWorkNode != NULL )
   {
    // Is directory node ?
    if( pWorkNode->GetFileInfoType() == _vfitDirectory )
     {
      // Scan this directory
      if( ImportDirectory( pWorkNode ) == false )
       return false;
     }
    else        // File
     {
      // Increment import file count
      m_ImportFileCount++;
      // Export progress text and percent
      if( m_pDisplayProgressTextFunction != NULL )
       m_pDisplayProgressTextFunction( pWorkNode->GetFilename() );
      if( m_pDisplayProgressPercentFunction != NULL )
       m_pDisplayProgressPercentFunction( (m_ImportFileCount * 100) / m_TotalFileNumber );
      // Import file data to archive file
	  if( ImportFile( pWorkNode ) == false )
       {
        // Display error message
        sprintf( TextBuffer, "The procedure makes a mistake while channelling into the file.\n- %s", pWorkNode->GetFilename() ); //程式在匯入檔案時發生錯誤
        if( m_pDisplayErrorFunction != NULL )
         m_pDisplayErrorFunction( TextBuffer );
        // Immediate return
        return false;
       }
     }
    // Process next node
    pWorkNode = pWorkNode->GetNextNode();
   }
  // Import directory successfully
  return true;
 }

// Function member of export file
bool CBlockFileManager::ExportFile( CVirtualFileInfoNode *pFileNode )
 {
  // Export file successfully
  return true;
 }

// Function member of export directory
bool CBlockFileManager::ExportDirectory( CVirtualFileInfoNode *pParentNode )
 {
  // Define local variable - file info node pointers
  CVirtualFileInfoNode *pWorkNode;

  // Scan all child node
  pWorkNode = pParentNode->GetFirstChildNode();
  while( pWorkNode != NULL )
   {
    // Is directory node ?
    if( pWorkNode->GetFileInfoType() == _vfitDirectory )
     {
      // Scan this directory
      if( ExportDirectory( pWorkNode ) == false )
       return false;
     }
    else        // File
     {
      // Export file data from archive file
      if( ExportFile( pWorkNode ) == false )
       {
        // Immediate return
        return false;
       }
     }
    // Process next node
    pWorkNode = pWorkNode->GetNextNode();
   }
  // Export directory successfully
  return true;
 }

// Function member of load block file manager's data file
bool CBlockFileManager::LoadBfmFile(
                                    const char *pFilename,
                                    int  ExpandFileNumber,
                                    int  ExpandBlockNumber
                                   )
 {
  // Define local variable - file header
  _CBfmFileHeader FileHeader;

  // Parameter error ?
  if( (ExpandFileNumber < 0) || (ExpandBlockNumber < 0) )
   return false;

  // Open block-file-manager file
  FILE* pFile = fopen( pFilename, "rb" );
  if( pFile == NULL )
	return false;

  // Read file header
  if( fread( &FileHeader, 1, sizeof( FileHeader ), pFile ) != sizeof( FileHeader ) )
	return false;

  // Allocate virtual directory manager
  m_pVirtualDirectoryManager = new CVirtualDirectoryManager;
  if( m_pVirtualDirectoryManager == NULL )
   {
	// Immediate close file
	fclose( pFile );
	// Return error code
    return false;
   }
  // Read part 1 : virtual directory manager
  if( m_pVirtualDirectoryManager->ImportVdmFile( pFile, ExpandFileNumber ) == false )
   {
	// Immediate close file
	fclose( pFile );
    // Return error code
    return false;
   }
  // Allocate block memory pool
  m_pBlockMemoryPool = new CBlockMemoryPool;
  if( m_pBlockMemoryPool == NULL )
   {
	// Immediate close file
	fclose( pFile );
	// Return error code
    return false;
   }
  // Read part 2 :  block memory pool
  if( m_pBlockMemoryPool->ImportBmpFile( pFile,
                                         pFilename,
                                         ExpandFileNumber,
                                         ExpandBlockNumber ) == false )
   {
	// Immediate close file
	fclose( pFile );
    // Return error code
    return false;
   }
  // Close save file
  fclose( pFile );

  // Setup system parameter
  m_BlockSize         = m_pBlockMemoryPool->GetBlockSize();
  m_CompressBlockSize = FileHeader.m_CompressBlockSize;
  m_CompressLevel     = FileHeader.m_CompressLevel;
  // Create compress file reader
  m_pCompressFileReader = new CCompressFileReader();
  if( m_pCompressFileReader == NULL )
   return false;
  // Startup reader
  if( m_pCompressFileReader->StartupCompressFileReader( m_pVirtualDirectoryManager,
                                                        m_pBlockMemoryPool,
                                                        m_CompressBlockSize ) == false )
   return false;
  // Load bfm file successfully
  return true;
 }

// Function member of save block file manager's data file
bool CBlockFileManager::SaveBfmFile( char *pFilename, bool CompactModeFlag )
 {
  // Define local variable - file header
  _CBfmFileHeader FileHeader;
  // Define local variable - file handle

  // Create save file
  FILE* pFile = fopen( pFilename, "wb" );
  if( pFile == NULL )
	return false;

  // Setup file-header's all field
  FileHeader.m_CompressBlockSize = m_CompressBlockSize;
  FileHeader.m_CompressLevel     = m_CompressLevel;

  // Write file header
  if( fwrite( &FileHeader, sizeof( FileHeader ), 1, pFile ) != sizeof( FileHeader ) )
  {
    fclose( pFile );
	return false;
  }

  // Write part 1 : virtual directory manager
  if( m_pVirtualDirectoryManager->ExportVdmFile( pFile, CompactModeFlag )== false )
   {
    // Immediate close file
	fclose( pFile );
	// Return error code
    return false;
   }
  // Write part 2 : block memory pool
  if( m_pBlockMemoryPool->ExportBmpFile( pFile, CompactModeFlag ) == false )
   {
    // Immediate close file
	fclose( pFile );
    // Return error code
    return false;
   }
  // Close save file
  fclose( pFile );
  // Save bfm file successfully
  return true;
 }

// Funciton member of startup block file manager
bool CBlockFileManager::StartupBlockFileManager(
                                                int  FileInfoNumber,
                                                int  HandleNumber,      // must >= 1
                                                int  BlockNumber,       // must >= 1
                                                int  BlockSize,         // must >= 1024
                                                int  CompressBlockSize, // Compress block size, must > 1024k, 0 = not compress mode
                                                int  CompressLevel,     // Compress level, for zlib compress lib
                                                int  SplitSize,         // Split size, must >= 256, 0 = not split mode
                                                char *pFilename         // Null = memory mapping mode
                                               )
 {
  // Store argument to data member
  m_BlockSize         = BlockSize;
  m_CompressBlockSize = CompressBlockSize;
  m_CompressLevel     = CompressLevel;
  // Allocate virtual directory manager
  m_pVirtualDirectoryManager = new CVirtualDirectoryManager;
  if( m_pVirtualDirectoryManager == NULL )
   return false;
  // Startup virtual directory manager
  if( m_pVirtualDirectoryManager->StartupVirtualDirectoryManager( FileInfoNumber ) == false )
   return false;
  // Allocate block memory pool
  m_pBlockMemoryPool = new CBlockMemoryPool;
  if( m_pBlockMemoryPool == NULL )
   return false;
  // Memory mapping mode ?
  if( pFilename == NULL )
   {
    // Startup block memory pool by memory mapping mode
    if( m_pBlockMemoryPool->StartupBlockMemoryPool( HandleNumber,
                                                    BlockNumber,
                                                    BlockSize ) == false )
     return false;
   }
  else          // File mapping mode
   {
    // Startup block memory pool by file mapping mode
    if( m_pBlockMemoryPool->StartupBlockMemoryPool( HandleNumber,
                                                    BlockNumber,
                                                    BlockSize,
                                                    pFilename,
                                                    SplitSize ) == false )
     return false;
   }
  // Startup block file manager successfully
  return true;
 }

// Function member of shutdown block file manager
void CBlockFileManager::ShutdownBlockFileManager( void )
 {
  // Flush block file manager
  FlushBlockFileManager();
  // Set current path to root
   m_CurrentPathHandle = 0;             
  // Free virtual directory manager ?
  if( m_pVirtualDirectoryManager != NULL )
   {
    // Delete object
    delete m_pVirtualDirectoryManager;
    // Clearup pointer
    m_pVirtualDirectoryManager = NULL;
   }
  // Free block memory pool ?
  if( m_pBlockMemoryPool != NULL )
   {
    // Delete object
    delete m_pBlockMemoryPool;
    // Clearup pointer
    m_pBlockMemoryPool = NULL;
   }
  // Free compress file reader
  if( m_pCompressFileReader != NULL )
   {
    // Delete object
    delete m_pCompressFileReader;
    // Clearup pointer
    m_pCompressFileReader = NULL;
   }
  // Free file importer
  if( m_pFileImporter != NULL )
   {
    // Delete object
    delete m_pFileImporter;
    // Clearup pointer
    m_pFileImporter = NULL;
   }
 }

// Function member of get handle of virtual-directory-manager ( -1 = error )
int CBlockFileManager::GetVdmFileHandle( char *pFilename )
 {
  // Define local variable - temporary handle
  int ParentPathHandle, Handle;

  // Is root path ?
  if( stricmp( pFilename, "\\" ) == 0 )
   {
    // Return handle ( root directory always = 0 )
    return 0;
   }
  // Is absolute-path ?
  if( *pFilename == '\\' )
   ParentPathHandle = 0;        // Root path
  else
   ParentPathHandle = m_CurrentPathHandle;
  // Get current path's handle
  Handle = m_pVirtualDirectoryManager->SearchFilename( ParentPathHandle, pFilename );
  // Return vdm handle ( -1 = error )
  return Handle;
 }

// Function member of get file info node
CVirtualFileInfoNode *CBlockFileManager::GetVdmFileInfoNode( int Handle )
 {
  // Get node from virtual directory manager and return it
  return m_pVirtualDirectoryManager->GetFileInfoNode( Handle );
 }

// Function member of set current path
bool CBlockFileManager::SetCurrentPath( char *pFilename )
 {
  // Define local variable - temporary handle
  int ParentPathHandle, Handle;

  // Is root path ?
  if( stricmp( pFilename, "\\" ) == 0 )
   {
    // Set current path handle to root
    m_CurrentPathHandle = 0;
    // Set current path successfully
    return true;
   }
  // Is absolute-path ?
  if( *pFilename == '\\' )
   ParentPathHandle = 0;        // Root path
  else
   ParentPathHandle = m_CurrentPathHandle;
  // Get current path's handle
  Handle = m_pVirtualDirectoryManager->SearchFilename( ParentPathHandle, pFilename );
  // Path exist ?
  if( Handle == -1 )
   return false;
  // Is directory ?
  if( m_pVirtualDirectoryManager->GetFileInfoNode( Handle )->GetFileInfoType() != _vfitDirectory )
   return false;
  // Store to current path handle
  m_CurrentPathHandle = Handle;
  // Set current path successfully
  return true;
 }

// Function member of get current path
void CBlockFileManager::GetCurrentPath( char *pFilename, bool RealPathFlag )
 {
  // Get full path name
  m_pVirtualDirectoryManager->GetFileInfoNode( m_CurrentPathHandle )->
   GetFullFilename( pFilename, RealPathFlag );
 }

// Function member of create directory ( false = error, not allow assign-dir. )
bool CBlockFileManager::CreateDirectoryB( char *pFilename )
 {
  // Define local variable - loop control
  int Index, Count;

  // Not allow multiple dir.
  Count = strlen( pFilename );
  for( Index = 0; Index < Count; Index++ )
   if( *(pFilename + Index) == '\\' )
    break;
  // Be assigned-dir. ?
  if( Index != Count )
   return false;
  // Empty VDM ?
  if( m_pVirtualDirectoryManager->GetFileInfoRootNode() == NULL )
   {
    // Create root node
    if( m_pVirtualDirectoryManager->CreateRootDirectoryNode( "C:\\Root" ) == NULL )
     return false;
   }
  // Create directory
  if( m_pVirtualDirectoryManager->CreateDirectoryNode( m_CurrentPathHandle, pFilename ) == NULL )
   return false;
  // Create directory successfully
  return true;
 }

// Function member of create file ( return file handle, -1 = error, not allow assign-dir. )
int CBlockFileManager::CreateFileB( char *pFilename )
 {
  // Define local variable - pointer of node
  CVirtualFileInfoNode *pNode;
  // Define local variable - loop control
  int Index, Count;

  // Not allow multiple dir.
  Count = strlen( pFilename );
  for( Index = 0; Index < Count; Index++ )
   if( *(pFilename + Index) == '\\' )
    break;
  // Be assigned-dir. ?
  if( Index != Count )
   return -1;
  // Empty VDM ?
  if( m_pVirtualDirectoryManager->GetFileInfoRootNode() == NULL )
   {
    // Create root node
    if( m_pVirtualDirectoryManager->CreateRootDirectoryNode( "C:\\Root" ) == NULL )
     return -1;
   }
  // Create file
  pNode = m_pVirtualDirectoryManager->CreateFileNode( m_CurrentPathHandle, pFilename );
  // Create error
  if( pNode == NULL )
   return -1;
  // Create file successfully
  return m_pVirtualDirectoryManager->GetNodeIndex( pNode );
 }

// Function member of open file ( return file handle, -1 = error )
int CBlockFileManager::OpenFile( const char *pFilename, bool AutoUncompressFlag )
 {
  // Define local variable - temporary handle
  int ParentPathHandle, Handle;
  // Define local variable - node pointer
  CVirtualFileInfoNode *pNode;

  // Is absolute-path ?
  if( *pFilename == '\\' )
   ParentPathHandle = 0;        // Root path
  else
   ParentPathHandle = m_CurrentPathHandle;
  // Get file handle from VDM
  Handle = m_pVirtualDirectoryManager->SearchFilename( ParentPathHandle, pFilename );
  // Found it ?
  if( Handle == -1 )
   return -1;
  // Get file info node
  pNode = m_pVirtualDirectoryManager->GetFileInfoNode( Handle );
  // Is file ?
  if( pNode->GetFileInfoType() != _vfitFile )
   return -1;
  // Set open flag
  pNode->SetFileOpenFlag( true );
  // Set auto uncompress flag
  pNode->SetFileAutoUncompressFlag( AutoUncompressFlag );
  // Clear file position
  pNode->SetFilePosition( 0 );
  // Return file handle
  return Handle;
 }

// Function member of close file
bool CBlockFileManager::CloseFile( int Handle )
 {
  // Define local variable - node pointer
  CVirtualFileInfoNode *pNode;

  // Get file info node
  pNode = m_pVirtualDirectoryManager->GetFileInfoNode( Handle );
  // Invalid handle ?
  if( pNode == NULL )
   return false;
  // File is close ?
  if( pNode->GetFileOpenFlag() == false )
   return true;
  // Clear open flag
  pNode->SetFileOpenFlag( false );
  // Clar auto uncompress flag
  pNode->SetFileAutoUncompressFlag( false );
  // Close file successfully
  return true;
 }

// Function member of get file name
bool CBlockFileManager::GetFilename(
                                    int  Handle,
                                    char *pFilename,
                                    bool FullPathFlag,
                                    bool RealPathFlag
                                   )
 {
  // Define local variable - node pointer
  CVirtualFileInfoNode *pNode;

  // Get file info node
  pNode = m_pVirtualDirectoryManager->GetFileInfoNode( Handle );
  // Invalid handle ?
  if( pNode == NULL )
   return false;
  // File is close ?
  if( pNode->GetFileOpenFlag() == false )
   return false;
  // Is fullname ?
  if( FullPathFlag == true )    // Include path + filename
   {
    // Get full filename
    pNode->GetFullFilename( pFilename, RealPathFlag ); 
   }
  else  // Only filename
   {
    // Copy filename to buffer
    strcpy( pFilename, pNode->GetFilename() );
   }
  // Get filename successfully
  return true;
 }

// Function member of get file size
int CBlockFileManager::GetFileSize( int Handle )
 {
  // Define local variable - node pointer
  CVirtualFileInfoNode *pNode;

  // Get file info node
  pNode = m_pVirtualDirectoryManager->GetFileInfoNode( Handle );
  // Invalid handle ?
  if( pNode == NULL )
   return -1;
  // File is close ?
  if( pNode->GetFileOpenFlag() == false )
   return -1;
  // Return file size
  return pNode->GetFileSize();
 }

// Function member of adjust file size
bool CBlockFileManager::AdjustFileSize( int Handle, int Size, bool ForceAdjustFlag )
 {
  // Define local variable - node pointer
  CVirtualFileInfoNode *pNode;
  // Define local variable - bmp handle
  int BmpHandle;

  // Get file info node
  pNode = m_pVirtualDirectoryManager->GetFileInfoNode( Handle );
  // Invalid handle ?
  if( pNode == NULL )
   return false;
  // File is close ?
  if( pNode->GetFileOpenFlag() == false )
   return false;
  // Is writable file ?
  if( pNode->IsWritableFile() == false )
   if( ForceAdjustFlag == false )
    return false; 
  // Invalid size ?
  if( Size < 0 )
   return false;
  // Get bmp handle
  BmpHandle = pNode->GetBmpHandle();
  // Empty file ?
  if( Size == 0 )
   {
    // Have bmp handle ?
    if( BmpHandle != -1 )       // Free it
     {
      // Free this handle
      if( m_pBlockMemoryPool->FreeHandle( BmpHandle ) == false )
       return false;
     }
    // Clear bmp handle
    pNode->SetBmpHandle( -1 );
    // Clear file size
    pNode->SetFileSize( 0 );
   }
  else          // Entity file
   {
    // Empty handle ?
    if( BmpHandle == -1 )
     {
      // Allocate handle of block memory pool
      if( (BmpHandle = m_pBlockMemoryPool->AllocHandle( 1 )) == -1 )
       return false;
      // Store to file info node
      pNode->SetBmpHandle( BmpHandle );
     }
    // Adjust file size
    if( m_pBlockMemoryPool->AdjustSize( BmpHandle, Size ) == false )
     return false;
    // Store file size
    pNode->SetFileSize( Size );
   }
  // Adjust file size successfully
  return true;
 }

// Function member of get file position ( -1 == error )
int CBlockFileManager::GetFilePosition( int Handle )
 {
  // Define local variable - node pointer
  CVirtualFileInfoNode *pNode;

  // Get file info node
  pNode = m_pVirtualDirectoryManager->GetFileInfoNode( Handle );
  // Invalid handle ?
  if( pNode == NULL )
   return -1;
  // File is close ?
  if( pNode->GetFileOpenFlag() == false )
   return -1;
  // Return file position
  return pNode->GetFilePosition();
 }

// Function member of set file position
bool CBlockFileManager::SetFilePosition( int Handle, int Position )
 {
  // Define local variable - node pointer
  CVirtualFileInfoNode *pNode;

  // Get file info node
  pNode = m_pVirtualDirectoryManager->GetFileInfoNode( Handle );
  // Invalid handle ?
  if( pNode == NULL )
   return false;
  // Set file position
  return pNode->SetFilePosition( Position );
 }

// Function member of read data
bool CBlockFileManager::ReadData( int Handle, int Size, void *pDataBuffer )
 {
  // Define local variable - node pointer
  CVirtualFileInfoNode *pNode;
  // Define local variable - bmp handle
  int BmpHandle;
  // Define local variable - file position
  int Position;

  // Get file info node
  pNode = m_pVirtualDirectoryManager->GetFileInfoNode( Handle );
  // Invalid handle ?
  if( pNode == NULL )
   return false;
  // File is close ?
  if( pNode->GetFileOpenFlag() == false )
   return false;
  // Get bmp handle
  BmpHandle = pNode->GetBmpHandle();
  // Invalid handle ?
  if( BmpHandle == -1 )
   return false;
  // Get file position
  Position = pNode->GetFilePosition();
  // Invalid size ?
  if( Size <= 0 )
   return false;
  if( (Position + Size) > pNode->GetFileSize() )
   return false;
  // Is normal file or disable auto-uncompress ?
  if( (pNode->IsCompressedFile() == false) || (pNode->GetFileAutoUncompressFlag() == false) )
   {
    // Read from block memory pool
    if( m_pBlockMemoryPool->ReadData( BmpHandle, Position, Size, pDataBuffer ) == false )
     return false;
   }
  else  // Auto uncompress mode
   {
    // Compress file reader ready ?
    if( m_pCompressFileReader == NULL )
     return false;
    // Bind handle to reader
    if( m_pCompressFileReader->BindWorkFile( Handle ) == false )
     return false;
    // Read data by reader
    if( m_pCompressFileReader->ReadData( Position, Size, (char *)pDataBuffer ) == false )
     return false;
   }
  // Adjust file position
  Position += Size;
  pNode->SetFilePosition( Position );
  // Read data successfully
  return true;
 }

// Function member of write data
bool CBlockFileManager::WriteData(
                                  int  Handle,
                                  int  Size,
                                  void *pDataBuffer,
                                  bool ForceWriteFlag
                                 )
 {
  // Define local variable - node pointer
  CVirtualFileInfoNode *pNode;
  // Define local variable - bmp handle
  int BmpHandle;
  // Define local variable - file position
  int Position;
  // Define local variable - append size
  int AppendSize;

  // Get file info node
  pNode = m_pVirtualDirectoryManager->GetFileInfoNode( Handle );
  // Invalid handle ?
  if( pNode == NULL )
   return false;
  // File is close ?
  if( pNode->GetFileOpenFlag() == false )
   return false;
  // Writable ?
  if( pNode->IsWritableFile() == false )
   if( ForceWriteFlag == false )
    return false;
  // Invalid size ?
  if( Size <= 0 )
   return false;
  // Get bmp handle
  BmpHandle = pNode->GetBmpHandle();
  // Empty handle ?
  if( BmpHandle == -1 )
   {
    // Allocate handle of block memory pool
    if( (BmpHandle = m_pBlockMemoryPool->AllocHandle( 1 )) == -1 )
     return false;
    // Store to file info node
    pNode->SetBmpHandle( BmpHandle );
   }
  // Get file position
  Position = pNode->GetFilePosition();
  // Write to block memory pool
  if( m_pBlockMemoryPool->WriteData( BmpHandle, Position, Size, pDataBuffer, true ) == false )
   return false;
  // Adjust file length
  if( (Position + Size) <= pNode->GetFileSize() )
   AppendSize = 0;
  else
   AppendSize = (Position + Size) - pNode->GetFileSize();
  pNode->SetFileSize( pNode->GetFileSize() + AppendSize );
  // Adjust file position
  Position += Size;
  pNode->SetFilePosition( Position );
  // Write data successfully
  return true;
 }

// Function member of flush block file manager
bool CBlockFileManager::FlushBlockFileManager( void )
 {
    if( m_pBlockMemoryPool == NULL )
        return true;

  // Flush block memory pool
  return m_pBlockMemoryPool->FlushBlockMemoryPool();
 }

// Function member of check files optimize mode ( all file )
void CBlockFileManager::CheckFilesOptimizeMode( void )
 {
  // Just call function
  if( m_pBlockMemoryPool )
    m_pBlockMemoryPool->CheckHandlesOptimizeMode();
 }

// Function member of import files ( import all files )
bool CBlockFileManager::ImportFiles(
                                    _DemcbFunction pDisplayErrorFunction,
                                    _DptcbFunction pDisplayProgressTextFunction,
                                    _DppcbFunction pDisplayProgressPercentFunction
                                   )
 {
  // Define local variable - file info node pointers
  CVirtualFileInfoNode *pRootNode;
  // Define local variable - return flag
  bool ReturnFlag;

  // Create file impoter
  m_pFileImporter = new CFileImporter();
  if( m_pFileImporter == NULL )
   return false;
  // Startup importer
  if( m_pFileImporter->StartupFileImporter( m_BlockSize,
                                            m_CompressBlockSize,
                                            m_CompressLevel ) == false )
   return false;
  // Store callback functions
  m_pDisplayErrorFunction           = pDisplayErrorFunction;
  m_pDisplayProgressTextFunction    = pDisplayProgressTextFunction;
  m_pDisplayProgressPercentFunction = pDisplayProgressPercentFunction;
  // Clear import file count
  m_ImportFileCount = 0;
  // Store total file number
  m_TotalFileNumber = m_pVirtualDirectoryManager->GetFileNumber();
  // Get root node
  pRootNode = m_pVirtualDirectoryManager->GetFileInfoRootNode();
  // Start import files
  if( ImportDirectory( pRootNode ) == false )
   {
    // Return error code
    ReturnFlag = false;
   }
  else
   {
    // Return success code
    ReturnFlag = true;
   }
  // Delete object
  delete m_pFileImporter;
  m_pFileImporter = NULL;
  // Import all files successfully
  return ReturnFlag;
 }

// Function member of export files ( export all files )
bool CBlockFileManager::ExportFiles( char *pTargetDirectory )
 {
  // Define local variable - file info node pointers
  CVirtualFileInfoNode *pRootNode;

  // Get root node
  pRootNode = m_pVirtualDirectoryManager->GetFileInfoRootNode();
  // Start export files
  if( ExportDirectory( pRootNode ) == false )
   return false;
  // Export all files successfully
  return true;
 }
