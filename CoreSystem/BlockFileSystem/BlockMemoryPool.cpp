/*
   Filename    : BlockMemoryPool.Cpp
   Author      : Curtis Lo
   Date        : 2003/09/18
   Descirption : Body file of block memory pool
*/
// Include standard header files
#include "stdafx.h"
#include <Sys\Stat.h>
#include <String.h>
#include <Stdlib.h>
#include <Fcntl.h>
#include <stdio.h>
#include <io.h>

// Include private header files
#include "BlockMemoryPool.h"

// Constructor
CBlockMemoryPool::CBlockMemoryPool( void )
 {
  // Setup data member
  m_pHandleTable        = NULL;
  m_pBlockNodeTable     = NULL;
  m_pBlockMemoryPool    = NULL;
  m_pDummyBlockBuffer   = NULL;
  m_pMappingBlockBuffer = NULL;
  m_MappingFileHandle   = -1;   // File handle, for identify File-Mapping mode
  m_MemoryMappingFlag = false ; 
 }

// Destructor
CBlockMemoryPool::~CBlockMemoryPool( void )
 {
  // Shutdown block memory pool
  ShutdownBlockMemoryPool();
 }

// Function member of alloc block for handle
bool CBlockMemoryPool::AllocHandleBlock( int Handle, int BlockNumber )
 {
  // Define local variable - node pointer
  _CBmpBlockNode *pFirstNode;
  // Define local variable - optimize status
  int NotSeqCount;
  // Define local variable - loop control
  int Index;

  // Store first block node to handle structure
  (m_pHandleTable + Handle)->m_TotalBlockNumber = BlockNumber;
  (m_pHandleTable + Handle)->m_pFirstBlockNode  = m_pFreeBlockNode;
  // Setup variable for optimize status
  NotSeqCount = 0;
  pFirstNode  = m_pFreeBlockNode;
  // Alloc block
  for( Index = 0; Index < BlockNumber; Index++ )
   {
    // Is last node ?
    if( Index == (BlockNumber - 1) )
     {
      // Store last block node
      (m_pHandleTable + Handle)->m_pLastBlockNode = m_pFreeBlockNode;
      // Is not-squence ?
      if( (pFirstNode + Index) != m_pFreeBlockNode )
       {
        // Increment not-sequence count
        NotSeqCount++;
       }
      // To next node
      m_pFreeBlockNode = m_pFreeBlockNode->m_pNextNode;
      // Clear last node's next node pointer
      (m_pHandleTable + Handle)->m_pLastBlockNode->m_pNextNode = NULL;
     }
    else        // General node
     {
      // Is not-squence ?
      if( (pFirstNode + Index) != m_pFreeBlockNode )
       {
        // Increment not-sequence count
        NotSeqCount++;
       }
      // To next node
      m_pFreeBlockNode = m_pFreeBlockNode->m_pNextNode;
     }
   }
  // Add allocated block number to data member
  m_UsedBlockNumber += BlockNumber;
  // Return optimize status
  if( NotSeqCount == 0 )
   return true;
  else
   return false;
 }

// Function member of free block for handle
void CBlockMemoryPool::FreeHandleBlock( int Handle )
 {
  // Link allocaed block to free block's link list ( free it )
  (m_pHandleTable + Handle)->m_pLastBlockNode->m_pNextNode = m_pFreeBlockNode;
  m_pFreeBlockNode = (m_pHandleTable + Handle)->m_pFirstBlockNode;
  // Adjust total used block number
  m_UsedBlockNumber -= (m_pHandleTable + Handle)->m_TotalBlockNumber;
  // Clear handle structure
  (m_pHandleTable + Handle)->m_TotalBlockNumber = -1;
 }

// Function member of extend block for handle
bool CBlockMemoryPool::ExtendHandleBlock( int Handle, int BlockNumber )
 {
  // Define local variable - node pointer
  _CBmpBlockNode *pFirstNode;
  // Define local variable - optimize status
  int NotSeqCount;
  // Define local variable - loop control
  int Index;

  // Link last node to free block
  (m_pHandleTable + Handle)->m_pLastBlockNode->m_pNextNode = m_pFreeBlockNode;
  // Setup variable for optimize status
  if( ((m_pHandleTable + Handle)->m_pLastBlockNode + 1) != m_pFreeBlockNode )
   NotSeqCount = 1;
  else
   NotSeqCount = 0; 
  pFirstNode  = m_pFreeBlockNode;
  // Alloc block
  for( Index = 0; Index < BlockNumber; Index++ )
   {
    // Is last node ?
    if( Index == (BlockNumber - 1) )
     {
      // Store last block node
      (m_pHandleTable + Handle)->m_pLastBlockNode = m_pFreeBlockNode;
      // Is not-squence ?
      if( (pFirstNode + Index) != m_pFreeBlockNode )
       {
        // Increment not-sequence count
        NotSeqCount++;
       }
      // To next node
      m_pFreeBlockNode = m_pFreeBlockNode->m_pNextNode;
      // Clear last node's next node pointer
      (m_pHandleTable + Handle)->m_pLastBlockNode->m_pNextNode = NULL;
     }
    else        // General node
     {
      // Is squence ?
      if( (pFirstNode + Index) != m_pFreeBlockNode )
       {
        // Increment not-sequence count
        NotSeqCount++;
       }
      // To next node
      m_pFreeBlockNode = m_pFreeBlockNode->m_pNextNode;
     }
   }
  // Adjust total block number in handle structure
  (m_pHandleTable + Handle)->m_TotalBlockNumber += BlockNumber;
  // Adjust total used block number
  m_UsedBlockNumber += BlockNumber;
  // Return optimize status
  if( NotSeqCount == 0 )
   return true;
  else
   return false;
 }

// Function member of reduce block for handle
void CBlockMemoryPool::ReduceHandleBlock( int Handle, int BlockNumber )
 {
  // Define local variable - node pointer
  _CBmpBlockNode *pNode, *pLastNode;
  // Define local variable - loop control
  int Index, Count;

  // Free blcok
  pNode = (m_pHandleTable + Handle)->m_pFirstBlockNode;
  Count = (m_pHandleTable + Handle)->m_TotalBlockNumber - BlockNumber;
  for( Index = 0; Index < Count; Index++ )
   {
    // Is last node ?
    if( Index == (Count - 1) )
     {
      // Temporary store last node's pointer
      pLastNode = (m_pHandleTable + Handle)->m_pLastBlockNode;
      // Store last block node
      (m_pHandleTable + Handle)->m_pLastBlockNode = pNode;
      // To next node
      pNode = pNode->m_pNextNode;
      // Clear last node's next node pointer
      (m_pHandleTable + Handle)->m_pLastBlockNode->m_pNextNode = NULL;
     }
    else        // General node
     {
      // To next node ( Search begin node by reduce size )
      pNode = pNode->m_pNextNode;
     }
   }
  // Free it
  pLastNode->m_pNextNode = m_pFreeBlockNode;
  m_pFreeBlockNode       = pNode;
  // Adjust total block number in handle structure
  (m_pHandleTable + Handle)->m_TotalBlockNumber -= BlockNumber;
  // Adjust total used block number
  m_UsedBlockNumber -= BlockNumber;
 }

// Function member of make mapping filename
void CBlockMemoryPool::MakeMappingFilename( int FileId )
 {
  // Fill extend filename by id
  sprintf( m_pMappingFilenameExt, "%03d", FileId );
 }

// Function member of read mapping block ( only for file-mapping mode )
bool CBlockMemoryPool::ReadMappingBlock( int BlockHandle, int BlockIndex )
 {
  // Define local variable - mapping file information
  int MfFileId, MfBlockId;
  // Define local variable - mapping file size
  int FileSize;

  // Is same block, always in memory ?
  if( m_MappingBlockIndex == BlockIndex )
   return true;
  // Update to mapping file
  if( m_MappingBlockIndex != -1 )
   {
    // Is file mapping mode
    if( m_MemoryMappingFlag == false )
     {
      // Update to mapping file
      if( WriteMappingBlock( m_MappingBlockIndex ) == false )
       return false;
     }
   }
  // Is split mode ?
  if( m_MappingFileSplitSize > 0 )
   {
    // Convert block index
    MfFileId  = BlockIndex / m_MappingFileSplitSize;
    MfBlockId = BlockIndex % m_MappingFileSplitSize;
   }
  else          // Single file mode
   {
    // Setup information
    MfFileId  = 0;
    MfBlockId = BlockIndex;
   }
  // File be opened ?
  if( m_MappingFileId != MfFileId )
   {
    // Make mapping filename
    MakeMappingFilename( MfFileId );
    // File exist ?
    if( access( m_MappingFilename, 0 ) != 0 )         // Not exist
     {
      // Clear mapping block buffer
      memset( m_pMappingBlockBuffer, 0, m_BlockSize );
      // Setup mapping block information
      m_MappingBlockHandle     = BlockHandle;
      m_MappingBlockIndex      = BlockIndex;
      m_MappingBlockUpdateFlag = false;
      // Like read block successfully
      return true;
     }
    // Need close file ?
    if( m_MappingFileId != -1 )
     {
      // Close mapping file
      close( m_MappingFileHandle );
      // Clearup relate data
      m_MappingFileHandle      = -1;
      m_MappingFileId          = -1;
      m_MappingFileBlockNumber = 0;
     }
    // Open mapping file
    m_MappingFileHandle = open( m_MappingFilename,
                                O_CREAT | O_BINARY | O_RDWR,
                                S_IREAD | S_IWRITE );
    if( m_MappingFileHandle == -1 )
     return false;
    // Get file length
    FileSize = filelength( m_MappingFileHandle );
    // Incorrect mapping file ?
    if( FileSize % m_BlockSize )
     return false;
    // Setup relate data
    m_MappingFileId = MfFileId;
    m_MappingFileBlockNumber = FileSize / m_BlockSize;
   }
  // Block exist ?
  if( m_MappingFileBlockNumber > MfBlockId )
   {
    // Move file position to block start position
    if( lseek( m_MappingFileHandle, MfBlockId * m_BlockSize, SEEK_SET ) == -1 )
     return false;
    // Write data to mapping file
    if( read( m_MappingFileHandle, m_pMappingBlockBuffer, m_BlockSize ) != m_BlockSize )
     return false;
   }
  else
   {
    // Clear mapping block buffer
    memset( m_pMappingBlockBuffer, 0, m_BlockSize );
   }
  // Setup mapping block information
  m_MappingBlockHandle     = BlockHandle;
  m_MappingBlockIndex      = BlockIndex;
  m_MappingBlockUpdateFlag = false;
  // Read mapping block from file successfully
  return true;
 }

// Function member of write mapping block ( only for file-mapping mode )
bool CBlockMemoryPool::WriteMappingBlock( int BlockIndex )
 {
  // Define local variable - mapping file information
  int MfFileId, MfBlockId;
  // Define local variable - mapping file size
  int FileSize;
  // Define local variable - loop control
  int Index, Count;

  // Invalid block index ?
  if( m_MappingBlockIndex == -1 )
   return false;
  // Need write ?
  if( m_MappingBlockUpdateFlag == false )
   return true;
  // Is split mode ?
  if( m_MappingFileSplitSize > 0 )
   {
    // Convert block index
    MfFileId  = BlockIndex / m_MappingFileSplitSize;
    MfBlockId = BlockIndex % m_MappingFileSplitSize;
   }
  else          // Single file mode
   {
    // Setup information
    MfFileId  = 0;
    MfBlockId = BlockIndex;
   }
  // File be opened ?
  if( m_MappingFileId != MfFileId )
   {
    // Need close file ?
    if( m_MappingFileId != -1 )
     {
      // Close mapping file
      close( m_MappingFileHandle );
      // Clearup relate data
      m_MappingFileHandle      = -1;
      m_MappingFileId          = -1;
      m_MappingFileBlockNumber = 0;
     }
    // Make mapping filename
    MakeMappingFilename( MfFileId );
    // Open mapping file
    m_MappingFileHandle = open( m_MappingFilename,
                                O_CREAT | O_BINARY | O_RDWR,
                                S_IREAD | S_IWRITE );
    if( m_MappingFileHandle == -1 )
     return false;
    // Get file length
    FileSize = filelength( m_MappingFileHandle );
    // Incorrect mapping file ?
    if( FileSize % m_BlockSize )
     return false;
    // Setup relate data
    m_MappingFileId = MfFileId;
    m_MappingFileBlockNumber = FileSize / m_BlockSize;
   }
  // Block exist ?
  if( m_MappingFileBlockNumber > MfBlockId )
   {
    // Move file position to block start position
    if( lseek( m_MappingFileHandle, MfBlockId * m_BlockSize, SEEK_SET ) == -1 )
     return false;
    // Write data to mapping file
    if( write( m_MappingFileHandle, m_pMappingBlockBuffer, m_BlockSize ) != m_BlockSize )
     return false;
   }
  else  // Extend block file size, because block not exist
   {
    // Caclulate block number
    Count = MfBlockId - m_MappingFileBlockNumber;
    // Need extend ?
    if( Count > 0 )
     {
      // Move file position to file end point
      if( lseek( m_MappingFileHandle, 0, SEEK_END ) == -1 )
       return false;
      // Append block to mapping file
      for( Index = 0; Index < Count; Index++ )
       {
        // Write dummy data to mapping file
        if( write( m_MappingFileHandle, m_pDummyBlockBuffer, m_BlockSize ) != m_BlockSize )
         return false;
       }
      // Add total block number
      m_MappingFileBlockNumber++;
     }
    // Move to last block's file position
    if( lseek( m_MappingFileHandle, 0, SEEK_END ) == -1 )
     return false;
    // Write block data
    if( write( m_MappingFileHandle, m_pMappingBlockBuffer, m_BlockSize ) != m_BlockSize )
     return false;
    // Add total block number
    m_MappingFileBlockNumber++;
   }
  // Setup mapping block information
  m_MappingBlockUpdateFlag = false;
  // Write mapping block from file successfully
  return true;
 }

// Function member of adjust base address - handle table
void CBlockMemoryPool::AddBaseAddress_HandleTable( int HandleNumber,void *pAddress )
 {
  // Define local variable - block node pointer
  _CBmpHandle *pHandle;
  // Define local variable - loop control
  int Index;

  // Adjust all handle
  for( Index = 0; Index < HandleNumber; Index++ )
   {
    // Get active handle
    pHandle = m_pHandleTable + Index;
    // Adjust pointers to offset value
    if( pHandle->m_pFirstBlockNode != NULL )
     *((long *)&(pHandle->m_pFirstBlockNode)) += (*((long *)&pAddress) - 1);
    if( pHandle->m_pLastBlockNode != NULL )
     *((long *)&(pHandle->m_pLastBlockNode)) += (*((long *)&pAddress) - 1);
   }
 }

// Function member of adjust base address - handle table
void CBlockMemoryPool::SubBaseAddress_HandleTable( int HandleNumber, void *pAddress )
 {
  // Define local variable - block node pointer
  _CBmpHandle *pHandle;
  // Define local variable - loop control
  int Index;

  // Adjust all handle
  for( Index = 0; Index < HandleNumber; Index++ )
   {
    // Get active handle
    pHandle = m_pHandleTable + Index;
    // Adjust pointers to offset value
    if( pHandle->m_pFirstBlockNode != NULL )
     *((long *)&(pHandle->m_pFirstBlockNode)) -= (*((long *)&pAddress) - 1);
    if( pHandle->m_pLastBlockNode != NULL )
     *((long *)&(pHandle->m_pLastBlockNode)) -= (*((long *)&pAddress) - 1);
   }
 }

// Function member of adjust base address - block node table
void CBlockMemoryPool::AddBaseAddress_BlockNodeTable( int BlockNodeNumber, void *pAddress )
 {
  // Define local variable - block node pointer
  _CBmpBlockNode *pNode;
  // Define local variable - loop control
  int Index;

  // Adjust all block node
  for( Index = 0; Index < BlockNodeNumber; Index++ )
   {
    // Get active node
    pNode = m_pBlockNodeTable + Index;
    // Adjust pointer to offset value
    if( pNode->m_pNextNode != NULL )
     *((long *)&(pNode->m_pNextNode)) += (*((long *)&pAddress) - 1);
   }
 }

// Function member of adjust base address - block node table
void CBlockMemoryPool::SubBaseAddress_BlockNodeTable( int BlockNodeNumber, void *pAddress )
 {
  // Define local variable - block node pointer
  _CBmpBlockNode *pNode;
  // Define local variable - loop control
  int Index;

  // Adjust all block node
  for( Index = 0; Index < BlockNodeNumber; Index++ )
   {
    // Get active node
    pNode = m_pBlockNodeTable + Index;
    // Adjust pointer to offset value
    if( pNode->m_pNextNode != NULL )
     *((long *)&(pNode->m_pNextNode)) -= (*((long *)&pAddress) - 1);
   }
 }

// Function member of get handle number of compact mode
int CBlockMemoryPool::GetHandleNumberOfCompactMode( void )
 {
  // Define local variable - loop control
  int Index;

  // Search all handle table
  for( Index = m_HandleNumber - 1; Index >= 0; Index-- )
   if( (m_pHandleTable + Index)->m_TotalBlockNumber != -1 )     // Not empty handle
    break;
  // Return handle number of compact mode
  return Index + 1;
 }

// Function member of get block node number of compact mode ( -1 = error )
int CBlockMemoryPool::GetBlockNodeNumberOfCompactMode( void )
 {
  // Define local variable - node pointer
  _CBmpBlockNode *pNode;
  // Define local variable - status table
  char *pStatusTable;
  // Define local variable - loop control
  int Index;

  // Allocate status table
  pStatusTable = (char *)malloc( m_BlockNumber );
  if( pStatusTable == NULL )
   return -1;
  // Clear status table
  memset( pStatusTable, 0, m_BlockNumber );
  // Setup status table
  pNode = m_pFreeBlockNode;
  while( pNode != NULL )
   {
    // Calculate index value
    Index = (*((long *)&pNode) - *((long *)&m_pBlockNodeTable))
            / sizeof( _CBmpBlockNode );
    // Setup status table
    *(pStatusTable + Index) = 1;
    // To next free node
    pNode = pNode->m_pNextNode;
   }
  // Scan all node status
  for( Index = m_BlockNumber - 1; Index >= 0; Index-- )
   {
    // Be used ?
    if( *(pStatusTable + Index) != 1 )
     break;
   }
  // Free status table
  free( pStatusTable );
  // Return block number of compact mode
  return Index + 1;
 }

// Function member of check data's tutbo status
bool CBlockMemoryPool::CheckDataTurboStatus( int Handle, int Offset, int Size )
 {
  // Define local variable - node pointer
  _CBmpBlockNode *pNode;
  // Define local variable - memory size
  int MemorySize;
  // Define local variable - loop control
  int Index, Count;

  // Move to source block
  pNode = (m_pHandleTable + Handle)->m_pFirstBlockNode;
  Count = Offset / m_BlockSize;
  for( Index = 0; Index < Count; Index++ )
   pNode = pNode->m_pNextNode;
  // Copy block memory to data buffer
  Offset = Offset % m_BlockSize;
  while( Size )
   {
    // Calculate memory size
    if( Offset != 0 )   // First time
     {
      // Setup copy size ( remain )
      MemorySize = m_BlockSize - Offset;
      // Clear offset value
      Offset = 0;
     }
    else        // Other block
     {
      // Setup copy size
      MemorySize = m_BlockSize;
     }
    // Adjust memory size
    if( MemorySize > Size )
     MemorySize = Size;
    // Is sequence ?
    if( (Size - MemorySize) > 0 )
     if( (pNode + 1) != pNode->m_pNextNode )
      break;
    // To next block node
    pNode = pNode->m_pNextNode;
    // Adjust size ( remainder )
    Size -= MemorySize;
   }
  // Not sequence ?
  if( Size != 0 )
   return false;
  // Allow use turbo mode for read and write data, return true
  return true;
 }

// Function member of import block memory pool's data file
bool CBlockMemoryPool::ImportBmpFile(
									 FILE* pFile,
									 const char *pFilename,
                                     int  ExpandHandleNumber,
                                     int  ExpandBlockNumber
                                    )
 {
  // Define local variable - file header
  _CBmpFileHeader FileHeader;
  // Define local variable - node pointer
  _CBmpBlockNode *pNode;
  // Define local variable - loop control
  int Index, Count;
  // Define local variable - read size
  unsigned int ReadSize;

  // Read file header
  if( fread( &FileHeader, 1, sizeof( FileHeader ), pFile ) != sizeof( FileHeader ) )
	return false; 

  // Restore file header to data member
  m_HandleNumber         = FileHeader.m_HandleNumber;
  m_BlockNumber          = FileHeader.m_BlockNumber;
  m_BlockSize            = FileHeader.m_BlockSize;
  m_UsedHandleNumber     = FileHeader.m_UsedHandleNumber;
  m_UsedBlockNumber      = FileHeader.m_UsedBlockNumber;
  m_FreeHandleIndex      = FileHeader.m_FreeHandleIndex;
  m_MappingFileSplitSize = FileHeader.m_MappingFileSplitSize;
  m_pFreeBlockNode       = FileHeader.m_pFreeBlockNode;
  // Setup data members
  m_MappingBlockHandle     = -1;
  m_MappingBlockIndex      = -1;
  m_MappingBlockUpdateFlag = false;
  m_MappingFileBlockNumber = 0;
  m_MappingFileId          = -1;
  m_MappingFileHandle      = -1;
  // Allocate handle table
  m_pHandleTable = (_CBmpHandle *)malloc( (size_t)(m_HandleNumber + ExpandHandleNumber) *
                                           sizeof( _CBmpHandle ) );
  if( m_pHandleTable == NULL )
   return false;
  // Alloc block node table
  m_pBlockNodeTable = (_CBmpBlockNode *)malloc( (size_t)(m_BlockNumber + ExpandBlockNumber) *
                                                sizeof( _CBmpBlockNode ) );
  if( m_pBlockNodeTable == NULL )
   return false;
  // Allocate dummy block buffer
  m_pDummyBlockBuffer = (char *)malloc( (size_t)m_BlockSize );
  if( m_pDummyBlockBuffer == NULL )
   return false;
  // Clear dummy block buffer
  memset( m_pDummyBlockBuffer, 0, (size_t)m_BlockSize );
  // Allocate mapping block buffer
  m_pMappingBlockBuffer = (char *)malloc( (size_t)m_BlockSize );
  if( m_pMappingBlockBuffer == NULL )
   return false;

  // Read handle table
  ReadSize = m_HandleNumber * sizeof( _CBmpHandle );
  if( fread( m_pHandleTable, 1, ReadSize, pFile ) != ReadSize )
	return false;

  // Read block node table
  ReadSize = m_BlockNumber * sizeof( _CBmpBlockNode );
  if( fread( m_pBlockNodeTable, 1, ReadSize, pFile ) != ReadSize )
	return false;  

  // Assign free block node pointer
  if( m_pFreeBlockNode != NULL )
   *((long *)&m_pFreeBlockNode) += (*((long *)&m_pBlockNodeTable) - 1);
  // Recover handle table ( offset -> memory address )
  AddBaseAddress_HandleTable( m_HandleNumber, m_pBlockNodeTable );
  // Recover block node table ( offset -> memory address )
  AddBaseAddress_BlockNodeTable( m_BlockNumber, m_pBlockNodeTable );
  // Have expand handle number ?
  if( ExpandHandleNumber > 0 )
   {
    // Clear all new handle
    Count = m_HandleNumber + ExpandHandleNumber;
    for( Index = m_HandleNumber; Index < Count; Index++ )
     {
      // Clear handle data ( become a empty handle )
      (m_pHandleTable + Index)->m_TotalBlockNumber = -1;
      (m_pHandleTable + Index)->m_OptimizeModeFlag = false;
     }
    // Adjust data members
    if( m_FreeHandleIndex == 0 )        // Full or compact-mode
     m_FreeHandleIndex = m_HandleNumber;
    m_HandleNumber += ExpandHandleNumber;
   }
  // Have expand block number ?
  if( ExpandBlockNumber > 0 )
   {
    // Setup all new block node
    Count = m_BlockNumber + ExpandBlockNumber;
    for( Index = m_BlockNumber; Index < Count; Index++ )
     {
      // Is last node ?
      if( Index == (Count - 1) )
       {
        // Clear last node's next node pointer
        (m_pBlockNodeTable + Index)->m_pNextNode = NULL;
       }
      else        // Other node, point to next node
       (m_pBlockNodeTable + Index)->m_pNextNode = m_pBlockNodeTable + Index + 1;
     }
    // Adjust pointer of free-block-node
    if( m_pFreeBlockNode == NULL )              // Full or compact-mode
     {
      // Link to pointer of free-block-node
      m_pFreeBlockNode = m_pBlockNodeTable + m_BlockNumber;
     }
    else        // Link to last free node's next node
     {
      // Search last mode
      pNode = m_pFreeBlockNode;
      while( 1 )
       {
        // Is last node ?
        if( pNode->m_pNextNode == NULL )
         break;
        // To next node
        pNode = pNode->m_pNextNode;
       }
      // Link it
      pNode->m_pNextNode = m_pBlockNodeTable + m_BlockNumber;
     }
    // Adjust total block number
    m_BlockNumber += ExpandBlockNumber;
   }
  // Process mapping filename
  memset( m_MappingFilename, 0, _MaxFilenameSize + 1 );
  strcpy( m_MappingFilename, pFilename );
  m_pMappingFilenameExt = NULL;
  // Scan all character
  Count = strlen( pFilename );
  for( Index = (Count - 1); Index >= 0; Index-- )
   {
    // Extend filename's prefix symbol ?
    if( *(pFilename + Index) == '.' )
     {
      // Store extend filename's start address
      m_pMappingFilenameExt = m_MappingFilename + Index + 1;
      // Exit loop
      break;
     }
    // '\\' symbol, file end ?
    if( *(pFilename + Index) == '\\' )
     break;
   }
  // Not find '.' symbol ?
  if( m_pMappingFilenameExt == NULL )
   {
    // Append '.' symbol
    m_MappingFilename[ Count ] = '.';
    // Just move to last character
    m_pMappingFilenameExt = m_MappingFilename + Count + 1;
   }
  // Load block memory pool file successfully
  return true;
 }

// Function member of export block memory pool's data file
bool CBlockMemoryPool::ExportBmpFile( FILE* pFile, bool CompactModeFlag )
 {
  // Define local variable - file header
  _CBmpFileHeader FileHeader;
  // Define local variable - handle number, block number
  int HandleNumber, BlockNumber;
  // Define local variable - write size
  unsigned int WriteSize;

  // Fill data to file header structure
  if( CompactModeFlag == true )
   {
    // Get handle number of compact mode
    HandleNumber = GetHandleNumberOfCompactMode();
    // Get block node number of compact mode
    BlockNumber = GetBlockNodeNumberOfCompactMode();
    // Setup file header by compact mode
    FileHeader.m_HandleNumber     = HandleNumber;
    FileHeader.m_UsedHandleNumber = HandleNumber;
    FileHeader.m_BlockNumber      = BlockNumber;
    FileHeader.m_UsedBlockNumber  = BlockNumber;
    FileHeader.m_FreeHandleIndex  = 0;
    FileHeader.m_pFreeBlockNode   = NULL;
   }
  else  // Normal, reserve free space for future
   {
    // Normal setup data
    FileHeader.m_HandleNumber     = m_HandleNumber;
    FileHeader.m_BlockNumber      = m_BlockNumber;
    FileHeader.m_UsedHandleNumber = m_UsedHandleNumber;
    FileHeader.m_UsedBlockNumber  = m_UsedBlockNumber;
    FileHeader.m_FreeHandleIndex  = m_FreeHandleIndex;
    FileHeader.m_pFreeBlockNode   = m_pFreeBlockNode;
   }
  FileHeader.m_BlockSize            = m_BlockSize;
  FileHeader.m_MappingFileSplitSize = m_MappingFileSplitSize;
  // Adjust address to offset value
  if( FileHeader.m_pFreeBlockNode != NULL )
   *((long *)&(FileHeader.m_pFreeBlockNode)) -= (*((long *)&m_pBlockNodeTable) - 1);

  // Write to file
  if( fwrite( &FileHeader, sizeof( FileHeader ), 1, pFile ) != sizeof( FileHeader ) )
	return false;

  // Adjust memory address to offset value
  SubBaseAddress_HandleTable( FileHeader.m_HandleNumber, m_pBlockNodeTable );
  // Write handle table
  WriteSize = FileHeader.m_HandleNumber * sizeof( _CBmpHandle );
  if( fwrite( m_pHandleTable, WriteSize, 1, pFile ) != WriteSize )
  {
	// Recover handle table
	AddBaseAddress_HandleTable( FileHeader.m_HandleNumber, m_pHandleTable );
	// Return error code
	return false;
  }
  // Recover handle table
  AddBaseAddress_HandleTable( FileHeader.m_HandleNumber, m_pBlockNodeTable );
  // Adjust memory address to offset value
  SubBaseAddress_BlockNodeTable( FileHeader.m_BlockNumber, m_pBlockNodeTable );
  // Write block node table
  WriteSize = FileHeader.m_BlockNumber * sizeof( _CBmpBlockNode );
  if( fwrite( m_pBlockNodeTable, WriteSize, 1, pFile ) != WriteSize )
   {
    // Recover block node table
    AddBaseAddress_BlockNodeTable( FileHeader.m_BlockNumber, m_pBlockNodeTable );
    // Return error code
    return false;
   }
  // Recover block node table
  AddBaseAddress_BlockNodeTable( FileHeader.m_BlockNumber, m_pBlockNodeTable );
  // Save block memory pool file successfully
  return true;
 }

// Function member of startup block memory pool
// Memory-mapping mode
bool CBlockMemoryPool::StartupBlockMemoryPool(
                                              int HandleNumber,
                                              int BlockNumber,
                                              int BlockSize
                                             )
 {
  // Define local variable - loop control
  int Index;

  // Invalid parameter ?
  if( (HandleNumber < 1) || (BlockNumber < 1) || (BlockSize < 4096) )
   return false;
  // Store parameter to data member
  m_HandleNumber = HandleNumber;
  m_BlockNumber  = BlockNumber;
  m_BlockSize    = BlockSize;
  // Setup data mmebers
  m_MemoryMappingFlag = true;
  m_UsedHandleNumber  = 0;
  m_UsedBlockNumber   = 0;
  m_FreeHandleIndex   = 0;
  // Allocate handle table
  m_pHandleTable = (_CBmpHandle *)malloc( (size_t)HandleNumber *
                                           sizeof( _CBmpHandle ) );
  if( m_pHandleTable == NULL )
   return false;
  // Alloc block node table
  m_pBlockNodeTable = (_CBmpBlockNode *)malloc( (size_t)BlockNumber *
                                                sizeof( _CBmpBlockNode ) );
  if( m_pBlockNodeTable == NULL )
   return false;
  // Allocate block memory pool
  m_pBlockMemoryPool = (char *)malloc( (size_t)BlockNumber * (size_t)BlockSize );
  if( m_pBlockMemoryPool == NULL )
   return false;
  // Initialize handle table
  for( Index = 0; Index < HandleNumber; Index++ )
   {
    // Clear handle data ( become a empty handle )
    (m_pHandleTable + Index)->m_TotalBlockNumber = -1;
    (m_pHandleTable + Index)->m_OptimizeModeFlag = false;
   }
  // Initialize block node table
  m_pFreeBlockNode = m_pBlockNodeTable;         // Point to first node
  for( Index = 0; Index < BlockNumber; Index++ )
   {
    // Is last node ?
    if( Index == (BlockNumber - 1) )
     {
      // Clear last node's next node pointer
      (m_pBlockNodeTable + Index)->m_pNextNode = NULL;
     }
    else        // Other node, point to next node
     (m_pBlockNodeTable + Index)->m_pNextNode = m_pBlockNodeTable + Index + 1;
   }
  // Startup block memory pool successfully
  return true;
 }

// Function member of startup block memory pool ( BlockSize unit = bytes )
// File-mapping mode
bool CBlockMemoryPool::StartupBlockMemoryPool(
                                              int  HandleNumber,
                                              int  BlockNumber,
                                              int  BlockSize,
                                              char *pFilename,
                                              int  SplitSize
                                             )
 {
  // Define local variable - loop control
  int Index, Count;

  // Invalid parameter ?
  if( (HandleNumber < 1) || (BlockNumber < 1) || (BlockSize < 4096) )
   return false;
  // Invalid split size ?
  if( SplitSize != 0 )
   if( SplitSize < 256 )
    return false;
  // Store parameter to data member
  m_HandleNumber         = HandleNumber;
  m_BlockNumber          = BlockNumber;
  m_BlockSize            = BlockSize;
  m_MappingFileSplitSize = SplitSize;
  // Setup data members
  m_MemoryMappingFlag      = false;
  m_UsedHandleNumber       = 0;
  m_UsedBlockNumber        = 0;
  m_FreeHandleIndex        = 0;
  m_MappingBlockHandle     = -1;
  m_MappingBlockIndex      = -1;
  m_MappingBlockUpdateFlag = false;
  m_MappingFileHandle      = -1;
  m_MappingFileId          = -1;
  m_MappingFileBlockNumber = 0;
  // Process mapping filename
  memset( m_MappingFilename, 0, _MaxFilenameSize + 1 );
  strcpy( m_MappingFilename, pFilename );
  m_pMappingFilenameExt = NULL;
  // Scan all character
  Count = strlen( pFilename );
  for( Index = (Count - 1); Index >= 0; Index-- )
   {
    // Extend filename's prefix symbol ?
    if( *(pFilename + Index) == '.' )
     {
      // Store extend filename's start address
      m_pMappingFilenameExt = m_MappingFilename + Index + 1;
      // Exit loop
      break;
     }
    // '\\' symbol, file end ?
    if( *(pFilename + Index) == '\\' )
     break;
   }
  // Not find '.' symbol ?
  if( m_pMappingFilenameExt == NULL )
   {
    // Append '.' symbol
    m_MappingFilename[ Count ] = '.';
    // Just move to last character
    m_pMappingFilenameExt = m_MappingFilename + Count + 1;
   }
  // Allocate handle table
  m_pHandleTable = (_CBmpHandle *)malloc( (size_t)HandleNumber *
                                           sizeof( _CBmpHandle ) );
  if( m_pHandleTable == NULL )
   return false;
  // Alloc block node table
  m_pBlockNodeTable = (_CBmpBlockNode *)malloc( (size_t)BlockNumber *
                                                sizeof( _CBmpBlockNode ) );
  if( m_pBlockNodeTable == NULL )
   return false;
  // Allocate dummy block buffer
  m_pDummyBlockBuffer = (char *)malloc( (size_t)BlockSize );
  if( m_pDummyBlockBuffer == NULL )
   return false;
  // Clear dummy block buffer
  memset( m_pDummyBlockBuffer, 0, (size_t)BlockSize );
  // Allocate mapping block buffer
  m_pMappingBlockBuffer = (char *)malloc( (size_t)BlockSize );
  if( m_pMappingBlockBuffer == NULL )
   return false;
  // Initialize handle table
  for( Index = 0; Index < HandleNumber; Index++ )
   {
    // Clear handle data ( become a empty handle )
    (m_pHandleTable + Index)->m_TotalBlockNumber = -1;
    (m_pHandleTable + Index)->m_OptimizeModeFlag = false;
   }
  // Initialize block node table
  m_pFreeBlockNode = m_pBlockNodeTable;         // Point to first node
  for( Index = 0; Index < BlockNumber; Index++ )
   {
    // Is last node ?
    if( Index == (BlockNumber - 1) )
     {
      // Clear last node's next node pointer
      (m_pBlockNodeTable + Index)->m_pNextNode = NULL;
     }
    else        // Other node, point to next node
     (m_pBlockNodeTable + Index)->m_pNextNode = m_pBlockNodeTable + Index + 1;
   }
  // Startup block memory pool successfully
  return true;
 }

// Function member of shutdown block memory pool
void CBlockMemoryPool::ShutdownBlockMemoryPool( void )
 {
  // Free handle table
  if( m_pHandleTable != NULL )
   free( m_pHandleTable );
  // Free block node table
  if( m_pBlockNodeTable != NULL )
   free( m_pBlockNodeTable );
  // Free block memory pool
  if( m_pBlockMemoryPool != NULL )
   free( m_pBlockMemoryPool );
  // Free dummy block buffer
  if( m_pDummyBlockBuffer != NULL )
   free( m_pDummyBlockBuffer );
  // Free mapping block buffer
  if( m_pMappingBlockBuffer != NULL )
   free( m_pMappingBlockBuffer );
  // Need close file ?
  if( m_MappingFileHandle != -1 )
   close( m_MappingFileHandle );
  // Clear data member
  m_pHandleTable        = NULL;
  m_pBlockNodeTable     = NULL;
  m_pBlockMemoryPool    = NULL;
  m_pDummyBlockBuffer   = NULL;
  m_pMappingBlockBuffer = NULL;
  m_MappingFileHandle   = -1;
 }

// Function memory of alloc handle ( -1 = error )
int CBlockMemoryPool::AllocHandle( int Size )
 {
  // Define local variable - alloc block number
  int AllocBlockNumber;
  // Define local variable - handle
  int Handle;

  // Invalid parameter ?
  if( Size <= 0 )
   return -1;
  // Have empty handle ?
  if( m_UsedHandleNumber >= m_HandleNumber )
   return -1;
  // Calculate alloc block number
  AllocBlockNumber = Size / m_BlockSize;
  if( Size % m_BlockSize )      // Have remainder ?
   AllocBlockNumber++;
  // Memory not enough ( insufficient ) ?
  if( AllocBlockNumber > (m_BlockNumber - m_UsedBlockNumber) )
   return -1;
  // Store alloced handle
  Handle = m_FreeHandleIndex;
  // Search next free node
  if( (m_UsedHandleNumber + 1) < m_HandleNumber )
   {
    // Search ...
    while( 1 )
     {
      // Adjust free handle index
      m_FreeHandleIndex++;
      // Overflow, back to head
      if( m_FreeHandleIndex >= m_HandleNumber )
       m_FreeHandleIndex = 0;
      // Is free node ?
      if( (m_pHandleTable + m_FreeHandleIndex)->m_TotalBlockNumber == -1 )
       break;                   // Exit search loop
     }
   }
  else          // Clear free node index
   m_FreeHandleIndex = 0;
  // Alloc block to handle
  (m_pHandleTable + Handle)->m_OptimizeModeFlag = AllocHandleBlock( Handle, AllocBlockNumber );
  // Adjust used handle number
  m_UsedHandleNumber++;
  // Alloc handle successfully
  return Handle;
 }

// Function memory of free handle
bool CBlockMemoryPool::FreeHandle( int Handle )
 {
  // Invalid parameter ?
  if( (Handle < 0) || (Handle >= m_HandleNumber) )
   return false;
  // Is valid node ?
  if( (m_pHandleTable + Handle)->m_TotalBlockNumber == -1 )     // Empty handle ?
   return false;
  // Free block
  FreeHandleBlock( Handle );
  // Need re-set free handle index ?
  if( m_FreeHandleIndex == 0 )
   m_FreeHandleIndex = Handle;
  // Adjust used handle number
  m_UsedHandleNumber--;
  // Free handle successfully
  return true;
 }

// Function member of check handle optimize status
bool CBlockMemoryPool::CheckHandleOptimizeStatus( int Handle )
 {
  // Define local variable - node pointer
  _CBmpBlockNode *pNode, *pFirstNode;
  // Define local variable - loop control
  int Index, Count;

  // Invalid parameter ?
  if( (Handle < 0) || (Handle >= m_HandleNumber) )
   return false;
  // Is valid node ?
  if( (m_pHandleTable + Handle)->m_TotalBlockNumber == -1 )     // Empty handle ?
   return false;
  // Get handle's first node
  pFirstNode = (m_pHandleTable + Handle)->m_pFirstBlockNode;
  pNode = pFirstNode;
  // Scan all node of handle
  Count = (m_pHandleTable + Handle)->m_TotalBlockNumber;
  for( Index = 0; Index < Count; Index++ )
   {
    // To next node
    pNode = pNode->m_pNextNode;
    // Is valid node ?
    if( pNode != NULL )
     {
      // Not sequence ?
      if( (pFirstNode + Index + 1) != pNode )
       break;
     }
   }
  // Not optimize mode ?
  if( Index != Count )
   return false;
  // Optimize mode, return true value
  return true;
 }

// Function member of check handles optimize mode ( all handle )
void CBlockMemoryPool::CheckHandlesOptimizeMode( void )
 {
  // Define local variable - count down
  int CountDown;
  // Define local variable - loop control
  int Index, Count;

  // Scan all handle
  CountDown = m_UsedHandleNumber;
  Count     = m_HandleNumber;
  for( Index = 0; Index < Count; Index++ )
   {
    // Process ok ?
    if( CountDown == 0 )
     break;
    // Is valid handle ?
    if( (m_pHandleTable + Index)->m_TotalBlockNumber != -1 )
     {
      // Check handle optimize status
      (m_pHandleTable + Index)->m_OptimizeModeFlag = CheckHandleOptimizeStatus( Index );
      // Decrement count down
      CountDown--;
     }
   }
 }

// Function member of adjust size
bool CBlockMemoryPool::AdjustSize( int Handle, int Size )
 {
  // Define local variable - optimize status
  bool OptimizeStatus;
  // Define local variable - alloc block number
  int AllocBlockNumber;

  // Invalid parameter ?
  if( (Handle < 0) || (Handle >= m_HandleNumber) || (Size <= 0) )
   return false;
  // Is valid node ?
  if( (m_pHandleTable + Handle)->m_TotalBlockNumber == -1 )     // Empty handle ?
   return false;
  // Calculate allocate block number
  AllocBlockNumber = Size / m_BlockSize;
  if( Size % m_BlockSize )
   AllocBlockNumber++;
  // Block number is match ?
  if( AllocBlockNumber != (m_pHandleTable + Handle)->m_TotalBlockNumber )
   {
    // Extend size ?
    if( AllocBlockNumber > (m_pHandleTable + Handle)->m_TotalBlockNumber )
     {
      // Memory not enough ( insufficient ) ?
      if( (AllocBlockNumber - (m_pHandleTable + Handle)->m_TotalBlockNumber) >
          (m_BlockNumber - m_UsedBlockNumber) )
       return false;
      // Extend block number from handle
      OptimizeStatus = ExtendHandleBlock( Handle,
                                          AllocBlockNumber - (m_pHandleTable + Handle)->m_TotalBlockNumber );
      // Need save optimize status ?
      if( (m_pHandleTable + Handle)->m_OptimizeModeFlag == true )
       (m_pHandleTable + Handle)->m_OptimizeModeFlag = OptimizeStatus;
     }
    else        // Reduce size
     {
      // Reduce block number from handle
      ReduceHandleBlock( Handle,
                         (m_pHandleTable + Handle)->m_TotalBlockNumber - AllocBlockNumber );
      // Optimize mode flag ? , maybe change to optimize status
      if( (m_pHandleTable + Handle)->m_OptimizeModeFlag == false )
       {
        // Check handle optimize status
        (m_pHandleTable + Handle)->m_OptimizeModeFlag = CheckHandleOptimizeStatus( Handle );
       }
     }
   }
  // Extend size successfully
  return true;
 }

// Function member of read data ( size unit = bytes )
bool CBlockMemoryPool::ReadData( int Handle, int Offset, int Size, void *pDataBuffer )
 {
  // Define local variable - node pointer
  _CBmpBlockNode *pNode;
  // Define local variable - turbo mode flag
  bool TurboModeFlag;
  // Define local variable - memory size
  char *pBlockMemory;
  int MemorySize;
  // Define local variable - range value
  int  MbStartPos, MbEndPos;
  int  StartPos, EndPos;
  bool StartInFlag, EndInFlag;
  // Define local variable - mapping file information
  int MfFileId, MfBlockId;
  int FileSize;
  int ReadSize;
  // Define local variable - loop control
  int Index, Count;

  // Invalid parameter ?
  if( (Handle < 0) || (Handle >= m_HandleNumber) )
   return false;
  // Is valid node ?
  if( (m_pHandleTable + Handle)->m_TotalBlockNumber == -1 )     // Empty handle ?
   return false;
  // Calculate memory size
  MemorySize = (m_pHandleTable + Handle)->m_TotalBlockNumber * m_BlockSize;
  // Overflow ?
  if( (Offset < 0) || (Offset >= MemorySize) )
   return false;
  if( Size > (MemorySize - Offset) )
   return false;
  // Optimize status ?
  if( (m_pHandleTable + Handle)->m_OptimizeModeFlag == true )
   TurboModeFlag = true;
  else
   {
    // Allow use turbo mode ?
    TurboModeFlag = CheckDataTurboStatus( Handle, Offset, Size );
   }
  // Move to source block
  pNode = (m_pHandleTable + Handle)->m_pFirstBlockNode;
  Count = Offset / m_BlockSize;
  for( Index = 0; Index < Count; Index++ )
   pNode = pNode->m_pNextNode;
  // Calculate new offset value
  Offset = Offset % m_BlockSize;
  // Use turbo mode ?
  if( TurboModeFlag == true )   // Turbo mode, for speed-up read and write data
   {
    // Calculate block's memory
    Index = (*((long *)&pNode) - *((long *)&m_pBlockNodeTable)) / sizeof( _CBmpBlockNode );
    // Memory mapping mode ?
    if( m_MemoryMappingFlag == true )
     {
      // Calculate block address to temporary pointer
      pBlockMemory = m_pBlockMemoryPool + (Index * m_BlockSize) + Offset;
      // Copy data form memory pool to assign pointer
      memcpy( pDataBuffer, pBlockMemory, Size );
     }
    else        // File-mapping mode
     {
      // Update to mapping file ?
      if( m_MappingBlockHandle == Handle )
       {
        // Calculate range value
        MbStartPos = m_MappingBlockIndex * m_BlockSize;
        MbEndPos   = MbStartPos + m_BlockSize;
        StartPos   = (Index * m_BlockSize) + Offset;
        EndPos     = StartPos + Size;
        // In flag
        StartInFlag = (StartPos >= MbStartPos) && (StartPos < MbEndPos);
        EndInFlag   = (EndPos >= MbStartPos) && (EndPos < MbEndPos);
        // Is range ?
        if(  StartInFlag || EndInFlag )
         {
          // Alway in mapping block's memory ?
          if( StartInFlag && EndInFlag )
           {
            // Just copy memory block to data pointer
            memcpy( pDataBuffer, m_pMappingBlockBuffer + Offset, Size );
            // Read data successfully
            return true;
           }
          else  // Flsuh memory to mapping file
           {
            // Write to mapping file
            if( WriteMappingBlock( m_MappingBlockIndex ) == false )
             return false;
            // Mark this memory block to invalid
            m_MappingBlockHandle     = -1;
            m_MappingBlockIndex      = -1;
            m_MappingBlockUpdateFlag = false;
           }
         }
       }
      // Read all data from file
      while( Size )
       {
        // Is split mode ?
        if( m_MappingFileSplitSize > 0 )
         {
          // Convert block index
          MfFileId  = Index / m_MappingFileSplitSize;
          MfBlockId = Index % m_MappingFileSplitSize;
          // Calculate allow read size
          MemorySize = (m_MappingFileSplitSize * m_BlockSize) -
                       ((MfBlockId * m_BlockSize) + Offset);
          // Allow read all data once time ?
          if( MemorySize >= Size )
           MemorySize = Size;
         }
        else          // Single file mode
         {
          // Setup information
          MfFileId  = 0;
          MfBlockId = Index;
          // Allow read data once time
          MemorySize = Size;
         }
        // File be opened ?
        if( m_MappingFileId != MfFileId )
         {
          // Make mapping filename
          MakeMappingFilename( MfFileId );
          // File exist ?
          if( access( m_MappingFilename, 0 ) != 0 )         // Not exist
           {
            // Clear memory
            memset( pDataBuffer, 0, Size );
           }
          else         // Open mapping file
           {
            // Need close file ?
            if( m_MappingFileId != -1 )
             {
              // Close mapping file
              close( m_MappingFileHandle );
              // Clearup relate data
              m_MappingFileHandle      = -1;
              m_MappingFileId          = -1;
              m_MappingFileBlockNumber = 0;
             }
            // Open mapping file
            m_MappingFileHandle = open( m_MappingFilename,
                                        O_CREAT | O_BINARY | O_RDWR,
                                        S_IREAD | S_IWRITE );
            if( m_MappingFileHandle == -1 )
             return false;
            // Get file length
            FileSize = filelength( m_MappingFileHandle );
            // Incorrect mapping file ?
            if( FileSize % m_BlockSize )
             return false;
            // Setup relate data
            m_MappingFileId = MfFileId;
            m_MappingFileBlockNumber = FileSize / m_BlockSize;
           }
         }
        // Block exist ?
        if( m_MappingFileBlockNumber > MfBlockId )
         {
          // Move file position to block start position
          if( lseek( m_MappingFileHandle,
                     (MfBlockId * m_BlockSize) + Offset,
                     SEEK_SET ) == -1 )
            return false;
          // Write data to mapping file
          ReadSize = read( m_MappingFileHandle, pDataBuffer, MemorySize );
          // Error ?
          if( ReadSize == -1 )
           return false;
          // Need clear memory ?
          if( (MemorySize - ReadSize) > 0 )
           memset( ((char *)pDataBuffer) + ReadSize, 0, MemorySize - ReadSize );
         }
        else  // Block not exist
         {
          // Clear memory
          memset( pDataBuffer, 0, MemorySize );
         }
        // Always clear offset value, only first time need this variable
        Offset = 0;
        // Adjust block index value
        Index += (MemorySize / m_BlockSize);
        if( MemorySize % m_BlockSize )
         Index++;
        // Adjust target pointer
        pDataBuffer = ((char *)pDataBuffer) + MemorySize;
        // Adjust size ( remainder )
        Size -= MemorySize;
       }
     }
   }
  else  // Normal, use block swap technique, slower
   {
    // Copy mapping memory/file to data buffer
    while( Size )
     {
      // Calculate block's memory
      Index = (*((long *)&pNode) - *((long *)&m_pBlockNodeTable)) / sizeof( _CBmpBlockNode );
      // Memory mapping mode ?
      if( m_MemoryMappingFlag == true )
       {
        // Calculate block address to temporary pointer
        pBlockMemory = m_pBlockMemoryPool + (Index * m_BlockSize);
       }
      else        // File mapping mode
       {
        // Read file data to mapping buffer
        if( ReadMappingBlock( Handle, Index ) == false )
         return false;
        // Assign to temporary pointer
        pBlockMemory = m_pMappingBlockBuffer;
       }
      // Calculate memory size
      if( Offset != 0 )   // First time
       {
        // Adjust pointer by offset value
        pBlockMemory += Offset;
        // Setup copy size ( remain )
        MemorySize = m_BlockSize - Offset;
        // Clear offset value
        Offset = 0;
       }
      else        // Other block
       {
        // Setup copy size
        MemorySize = m_BlockSize;
       }
      // Adjust memory size
      if( MemorySize > Size )
       MemorySize = Size;
      // Copy block memory to data buffer
      memcpy( pDataBuffer, pBlockMemory, MemorySize );
      // To next block node
      pNode = pNode->m_pNextNode;
      // Adjust target pointer
      pDataBuffer = ((char *)pDataBuffer) + MemorySize;
      // Adjust size ( remainder )
      Size -= MemorySize;
     }
   }
  // Read data successfully
  return true;
 }

// Funcion member of write data ( size unit = bytes )
bool CBlockMemoryPool::WriteData(
                                 int  Handle,
                                 int  Offset,
                                 int  Size,
                                 void *pDataBuffer,
                                 bool AutoExtendFlag
                                )
 {
  // Define local variable - node pointer
  _CBmpBlockNode *pNode;
  // Define local variable - turbo mode flag
  bool TurboModeFlag;
  // Define local variable - memory size
  char *pBlockMemory;
  int MemorySize;
  // Define local variable - range value
  int  MbStartPos, MbEndPos;
  int  StartPos, EndPos;
  bool StartInFlag, EndInFlag;
  // Define local variable - mapping file information
  int MfFileId, MfBlockId;
  int FileSize;
  int WriteSize;
  // Define local variable - loop control
  int Index, Count;

  // Invalid parameter ?
  if( (Handle < 0) || (Handle >= m_HandleNumber) )
   return false;
  // Is valid node ?
  if( (m_pHandleTable + Handle)->m_TotalBlockNumber == -1 )     // Empty handle ?
   return false;
  // Calculate memory size
  MemorySize = (m_pHandleTable + Handle)->m_TotalBlockNumber * m_BlockSize;
  // Overflow ?
  if( (Offset < 0) || (Offset > MemorySize) )
   return false;
  // Overflow ?
  if( Size > (MemorySize - Offset) )
   if( AutoExtendFlag == false )
    return false;
   else         // Auto extend block
    {
     // Adjust size
     if( AdjustSize( Handle, Offset + Size ) == false )          // Error ?
      return false;
    }
  // Optimize status ?
  if( (m_pHandleTable + Handle)->m_OptimizeModeFlag == true )
   TurboModeFlag = true;
  else
   {
    // Allow use turbo mode ?
    TurboModeFlag = CheckDataTurboStatus( Handle, Offset, Size );
   }
  // Move to target block
  pNode = (m_pHandleTable + Handle)->m_pFirstBlockNode;
  Count = Offset / m_BlockSize;
  for( Index = 0; Index < Count; Index++ )
   pNode = pNode->m_pNextNode;
  // Calculate new offset value
  Offset = Offset % m_BlockSize;
  // Use turbo mode ?
  if( TurboModeFlag == true )
   {
    // Calculate block's memory
    Index = (*((long *)&pNode) - *((long *)&m_pBlockNodeTable)) / sizeof( _CBmpBlockNode );
    // Memory mapping mode ?
    if( m_MemoryMappingFlag == true )
     {
      // Calculate block address to temporary pointer
      pBlockMemory = m_pBlockMemoryPool + (Index * m_BlockSize) + Offset;
      // Copy data form assign pointer to memory pool
      memcpy( pBlockMemory, pDataBuffer, Size );
     }
    else        // File-mapping mode
     {
      // Update to mapping file ?
      if( m_MappingBlockHandle == Handle )
       {
        // Calculate range value
        MbStartPos = m_MappingBlockIndex * m_BlockSize;
        MbEndPos   = MbStartPos + m_BlockSize;
        StartPos   = (Index * m_BlockSize) + Offset;
        EndPos     = StartPos + Size;
        // In flag
        StartInFlag = (StartPos >= MbStartPos) && (StartPos < MbEndPos);
        EndInFlag   = (EndPos >= MbStartPos) && (EndPos < MbEndPos);
        // Is range ?
        if(  StartInFlag || EndInFlag )
         {
          // In mapping block memory's range ?
          if( StartInFlag && EndInFlag )
           {
            // Just copy data pointer to mapping block memory
            memcpy( m_pMappingBlockBuffer + Offset, pDataBuffer, Size );
            // Set mapping block information
            m_MappingBlockUpdateFlag = true;
            // Write data successfully
            return true;
           }
          else  // Flsuh memory to mapping file
           {
            // Write to mapping file
            if( WriteMappingBlock( m_MappingBlockIndex ) == false )
             return false;
            // Mark this memory block to invalid
            m_MappingBlockHandle     = -1;
            m_MappingBlockIndex      = -1;
            m_MappingBlockUpdateFlag = false;
           }
         }
       }
      // Write all data to file
      while( Size )
       {
        // Is split mode ?
        if( m_MappingFileSplitSize > 0 )
         {
          // Convert block index
          MfFileId  = Index / m_MappingFileSplitSize;
          MfBlockId = Index % m_MappingFileSplitSize;
          // Calculate allow read size
          MemorySize = (m_MappingFileSplitSize * m_BlockSize) -
                       ((MfBlockId * m_BlockSize) + Offset);
          // Allow read all data once time ?
          if( MemorySize >= Size )
           MemorySize = Size;
         }
        else          // Single file mode
         {
          // Setup information
          MfFileId  = 0;
          MfBlockId = Index;
          // Allow read data once time
          MemorySize = Size;
         }
        // File be opened ?
        if( m_MappingFileId != MfFileId )
         {
          // Need close file ?
          if( m_MappingFileId != -1 )
           {
            // Close mapping file
            close( m_MappingFileHandle );
            // Clearup relate data
            m_MappingFileHandle      = -1;
            m_MappingFileId          = -1;
            m_MappingFileBlockNumber = 0;
           }
          // Make mapping filename
          MakeMappingFilename( MfFileId );
          // Open mapping file
          m_MappingFileHandle = open( m_MappingFilename,
                                      O_CREAT | O_BINARY | O_RDWR,
                                      S_IREAD | S_IWRITE );
          if( m_MappingFileHandle == -1 )
           return false;
          // Get file length
          FileSize = filelength( m_MappingFileHandle );
          // Incorrect mapping file ?
          if( FileSize % m_BlockSize )
           return false;
          // Setup relate data
          m_MappingFileId = MfFileId;
          m_MappingFileBlockNumber = FileSize / m_BlockSize;
         }
        // Block exist ?
        if( m_MappingFileBlockNumber > MfBlockId )
         {
          // Move file position to block start position
          if( lseek( m_MappingFileHandle, (MfBlockId * m_BlockSize) + Offset, SEEK_SET ) == -1 )
           return false;
          // Write data to mapping file
          if( write( m_MappingFileHandle, pDataBuffer, MemorySize ) != MemorySize )
           return false;
         }
        else    // Extend block file size, because block not exist
         {
          // Caclulate block number
          Count = MfBlockId - m_MappingFileBlockNumber;
          // Have offset ?
          if( Offset > 0 )
           Count++;
          // Need extend ?
          if( Count > 0 )
           {
            // Move file position to file end point
            if( lseek( m_MappingFileHandle, 0, SEEK_END ) == -1 )
             return false;
            // Append block to mapping file
            for( Index = 0; Index < Count; Index++ )
             {
              // Write dummy data to mapping file
              if( write( m_MappingFileHandle, m_pDummyBlockBuffer, m_BlockSize ) != m_BlockSize )
               return false;
             }
            // Add total block number
            m_MappingFileBlockNumber++;
           }
          // Move to last block's file position
          if( lseek( m_MappingFileHandle, (MfBlockId * m_BlockSize) + Offset, SEEK_SET ) == -1 )
           return false;
          // Write block data
          if( write( m_MappingFileHandle, pDataBuffer, MemorySize ) != MemorySize )
           return false;
         }
        // Is expend block ?
        if( ((MfBlockId * m_BlockSize) + Offset + MemorySize) >
            (m_MappingFileBlockNumber * m_BlockSize) )
         {
          // Fill dummy data for adjust file size to block bound
          FileSize = (MfBlockId * m_BlockSize) + Offset + MemorySize;
          if( FileSize % m_BlockSize )
           {
            // Calculate wirte size
            WriteSize = m_BlockSize - (FileSize % m_BlockSize);
            // Write dummy data
            if( write( m_MappingFileHandle, m_pDummyBlockBuffer, WriteSize ) != WriteSize )
             return false;
            // Adjust file size
            FileSize += WriteSize;
           }
          // Adjust total block number
          m_MappingFileBlockNumber = FileSize / m_BlockSize;
         }
        // Always clear offset value, only first time need this variable
        Offset = 0;
        // Adjust block index value
        Index += (MemorySize / m_BlockSize);
        if( MemorySize % m_BlockSize )
         Index++;
        // Adjust target pointer
        pDataBuffer = ((char *)pDataBuffer) + MemorySize;
        // Adjust size ( remainder )
        Size -= MemorySize;
       }
     }
   }
  else          // Normal mode ?
   {
    // Write data to mapping memory/file
    while( Size )
     {
      // Calculate block's memory
      Index = (*((long *)&pNode) - *((long *)&m_pBlockNodeTable)) / sizeof( _CBmpBlockNode );
      // Memory mapping mode ?
      if( m_MemoryMappingFlag == true )
       {
        // Calculate block address to temporary pointer
        pBlockMemory = m_pBlockMemoryPool + (Index * m_BlockSize);
       }
      else        // File mapping mode
       {
        // Make sure memory mapping is correct, so read this block
        if( ReadMappingBlock( Handle, Index ) == false )
         return false;
        // Assign to temporary pointer
        pBlockMemory = m_pMappingBlockBuffer;
       }
      // Calculate memory size
      if( Offset != 0 )   // First time
       {
        // Adjust pointer by offset value
        pBlockMemory += Offset;
        // Setup copy size ( remain )
        MemorySize = m_BlockSize - Offset;
        // Clear offset value
        Offset = 0;
       }
      else        // Other block
       {
        // Setup copy size
        MemorySize = m_BlockSize;
       }
      // Adjust memory size
      if( MemorySize > Size )
       MemorySize = Size;
      // Copy block memory to data buffer
      memcpy( pBlockMemory, pDataBuffer, MemorySize );
      // To next block node
      pNode = pNode->m_pNextNode;
      // Adjust target pointer
      pDataBuffer = ((char *)pDataBuffer) + MemorySize;
      // Adjust size ( remainder )
      Size -= MemorySize;
      // Set mapping block information
      m_MappingBlockUpdateFlag = true;
     }
   }
  // Write data successfully
  return true;
 }

// Function member of mapping block buffer to mapping file ( only active by mapping file mode )
bool CBlockMemoryPool::FlushBlockMemoryPool( void )
 {
  // Is memory mapping mode, just return
  if( m_MemoryMappingFlag == true )
   return true;
  // Have any active mapping block ?
  if( m_MappingBlockIndex == -1 )
   return true;
  // Update to mapping file
  if( WriteMappingBlock( m_MappingBlockIndex ) == false )
   return false;
  // Close mapping file
  close( m_MappingFileHandle );
  // Clearup relate data
  m_MappingBlockHandle     = -1;
  m_MappingBlockIndex      = -1;
  m_MappingBlockUpdateFlag = false;
  m_MappingFileHandle      = -1;
  m_MappingFileId          = -1;
  m_MappingFileBlockNumber = 0;
  // Flush block memory pool
  return true;
 }


