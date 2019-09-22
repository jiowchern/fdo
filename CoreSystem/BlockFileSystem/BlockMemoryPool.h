/*
  Filename    : BlockMemoryPool.H
  Author      : Curtis Lo
  Date        : 2003/09/18
  Descirption : Header file of block memory pool 
*/
// Avoid recurrence include this file, Start condition compile
#ifndef __BlockMemoryPool_H
#define __BlockMemoryPool_H

 // Include private header files
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

 // Declare structure of BmpBlockNode
 struct _CBmpBlockNode
  {
   // Data member of block single-direction link-list
   _CBmpBlockNode *m_pNextNode;
  };

 // Declare structure of BmpHandle
 struct _CBmpHandle
  {
   // Data member of total block number
   int m_TotalBlockNumber;
   // Data member of optimize mode flag
   bool m_OptimizeModeFlag;
   // Data member of first/last block index
   _CBmpBlockNode *m_pFirstBlockNode;
   _CBmpBlockNode *m_pLastBlockNode;
  };

 // Declare structure of BmpFileHeader
 struct _CBmpFileHeader
  {
   // Data member of block memory pool information
   int m_HandleNumber;
   int m_BlockNumber;
   int m_BlockSize;
   // Data memory of block memory pool's current state
   int m_UsedHandleNumber;
   int m_UsedBlockNumber;
   // Data member of mapping file split size
   int m_MappingFileSplitSize;
   // Data member of free handle index
   int m_FreeHandleIndex;
   // Data member of free block node ( first node of free block )
   _CBmpBlockNode *m_pFreeBlockNode;
  };

 #ifdef __BORLANDC__
  // Back to default alignment
  #pragma option pop
 #else
  // Back to default alignment
  #pragma pack( pop )
 #endif

 // Declare class of block memory pool
 class CBlockMemoryPool
  {
   private:
    // Data member of block memory pool information
    int m_HandleNumber;
    int m_BlockNumber;
    int m_BlockSize;
    // Data memory of block memory pool's current state
    int m_UsedHandleNumber;
    int m_UsedBlockNumber;
    // Data member of memory mapping flag
    bool m_MemoryMappingFlag;
    // Data member of mapping block information
    int  m_MappingBlockHandle;
    int  m_MappingBlockIndex; 
    bool m_MappingBlockUpdateFlag;
    // Data member of block memory mapping filename
    char m_MappingFilename[ _MaxFilenameSize + 1 ];
    char *m_pMappingFilenameExt;
    // Data member of mapping file information
    int m_MappingFileSplitSize;
	int m_MappingFileHandle;

	FILE* m_pFile;

    int m_MappingFileId;
    int m_MappingFileBlockNumber;
    // Data member of free block node ( first node of free block )
    _CBmpBlockNode *m_pFreeBlockNode;
    // Data member of handle table
    _CBmpHandle *m_pHandleTable;
    int m_FreeHandleIndex;
    // Data member of block node table
    _CBmpBlockNode *m_pBlockNodeTable;
    // Data member of block memory pool pointer
    char *m_pBlockMemoryPool;
    // Data member of dummy block buffer
    char *m_pDummyBlockBuffer;
    // Data member of mapping block buffer
    char *m_pMappingBlockBuffer;
    // Function member of alloc block for handle
    bool AllocHandleBlock( int Handle, int BlockNumber );
    // Function member of free block for handle
    void FreeHandleBlock( int Handle );
    // Function member of extend block for handle
    bool ExtendHandleBlock( int Handle, int BlockNumber );
    // Function member of reduce block for handle
    void ReduceHandleBlock( int Handle, int BlockNumber );
    // Function member of make mapping filename
    void MakeMappingFilename( int FileId );
    // Function member of read mapping block ( only for file-mapping mode )
    bool ReadMappingBlock( int BlockHandle, int BlockIndex );
    // Function member of write mapping block ( only for file-mapping mode )
    bool WriteMappingBlock( int BlockIndex );
    // Function member of adjust base address - handle table
    void AddBaseAddress_HandleTable( int HandleNumber, void *pAddress );
    void SubBaseAddress_HandleTable( int HandleNumber, void *pAddress );
    // Function member of adjust base address - block node table
    void AddBaseAddress_BlockNodeTable( int BlockNodeNumber, void *pAddress );
    void SubBaseAddress_BlockNodeTable( int BlockNodeNumber, void *pAddress );
    // Function member of get handle number of compact mode
    int GetHandleNumberOfCompactMode( void );
    // Function member of get block node number of compact mode
    int GetBlockNodeNumberOfCompactMode( void );
    // Function member of check data's tutbo status
    bool CheckDataTurboStatus( int Handle, int Offset, int Size );
   public:
    // Constructor
    CBlockMemoryPool( void );
    // Destructor
    ~CBlockMemoryPool( void );
    // Function member of import block memory pool's data file
    bool ImportBmpFile(
					   FILE* pFile,
                       const char *pFilename,
                       int  ExpandHandleNumber,
                       int  ExpandBlockNumber
                      );
    // Function member of export block memory pool's data file
	bool ExportBmpFile( FILE* pFile, bool CompactModeFlag );
    // Function member of startup block memory pool ( BlockSize unit = bytes )
    // Memory-mapping mode
    bool StartupBlockMemoryPool(
                                int HandleNumber,              // must >= 1
                                int BlockNumber,               // must >= 1
                                int BlockSize                  // must >= 4096
                               );
    // Function member of startup block memory pool ( BlockSize unit = bytes )
    // File-mapping mode
    bool StartupBlockMemoryPool(
                                int  HandleNumber,              // must >= 1
                                int  BlockNumber,               // must >= 1
                                int  BlockSize,                 // must >= 4096
                                char *pFilename,
                                int  SplitSize                  // Split size, must >= 256
                               );
    // Function member of shutdown block memory pool
    void ShutdownBlockMemoryPool( void );
    // Function memory of alloc handle ( Size unit = bytes ), -1 = error
    int AllocHandle( int Size );
    // Function memory of free handle
    bool FreeHandle( int Handle );
    // Function member of check handle optimize status
    bool CheckHandleOptimizeStatus( int Handle );
    // Function member of check handles optimize mode ( all handle )
    void CheckHandlesOptimizeMode( void );
    // Function member of adjust size ( Size unit = bytes, must >= 1 )
    bool AdjustSize( int Handle, int Size );
    // Function member of read data ( size unit = bytes )
    bool ReadData( int Handle, int Offset, int Size, void *pDataBuffer );
    // Funcion member of write data ( size unit = bytes )
    bool WriteData( int Handle, int Offset, int Size, void *pDataBuffer, bool AutoExtendFlag );
    // Function member of mapping block buffer to mapping file ( only active by mapping file mode )
    bool FlushBlockMemoryPool( void );
    // Function member of get block memory pool information
    int GetHandleNumber( void )        { return m_HandleNumber; }
    int GetBlockNumber( void )         { return m_BlockNumber; }
    int GetBlockSize( void )           { return m_BlockSize; }
    // Function member of get free handle number
    int GetFreeHandleNumber( void )    { return m_HandleNumber - m_UsedHandleNumber; }
    // Function member of get free block number
    int GetFreeBlockNumber( void )     { return m_BlockNumber - m_UsedBlockNumber; }
    // Function member of get handle's block number
    int GetHandleBlockNumber( int Handle )
     { return (m_pHandleTable + Handle)->m_TotalBlockNumber; }
  };

// End condition compile
#endif
