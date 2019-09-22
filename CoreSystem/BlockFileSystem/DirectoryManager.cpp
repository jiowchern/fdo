/*
  Filename    : DirectoryManager.Cpp
  Author      : Curtis Lo
  Date        : 2003/05/08
  Descirption : Body file of directory manager
*/
// Include standard header files
#include "stdafx.h"

#include <Stdlib.h>
#include <mAlloc.h>
#include <Stdio.h>
#include <conio.h>
//#include <Mem.h>
#include <string.h>
#include <io.h>

// Include extra 3rd party libray's header files
#include "Zlib\\Zlib.h"

// Include private header files
#include "DirectoryManager.h"

// Define global symbol of file mark's block size
#define _FileMarkDataBlockSize          (1024L * 128L)

// Define global symbol of maximum sort item number
#define _MaximumSortItemNumber          4096L

/* ------------------------------CFileInfoNode------------------------------- */

// Constructor
CFileInfoNode::CFileInfoNode( void )
 {
  // Setup this node
  SetupNode();
 }

// Destructor
CFileInfoNode::~CFileInfoNode( void )
 {

 }

// Function member of reverse string cat
void CFileInfoNode::ReverseStringAdd( char *pBuffer, char *pString )
 {
  // Define local variable - pointers
  char *pSource, *pTarget;
  // Define local variable - loop count
  int Index, Count;

  // Copy string to buffer by reverse mode
  pSource = pString + strlen( pString ) - 1;
  pTarget = pBuffer + strlen( pBuffer );
  Count = strlen( pString );
  for( Index = 0; Index < Count; Index++ )
   {
    // Copy character
    *pTarget = *pSource;
    pSource--;
    pTarget++;
   }
 }

// Function member of reverse string
void CFileInfoNode::ReverseString( char *pString )
 {
  // Define local variable - pointers
  char *pSource, *pTarget;
  // Define local variable - character
  char Character; 
  // Define local variable - loop count
  int Index, Count;

  // Reverse string
  pSource = pString;
  pTarget = pString + strlen( pString ) - 1;
  Count = strlen( pString ) / 2;
  for( Index = 0; Index < Count; Index++ )
   {
    // Exchange character
    Character = *pSource;
    *pSource = *pTarget;
    *pTarget = Character;
    pSource++;
    pTarget--;
   }
 }

// Function member of setup node
void CFileInfoNode::SetupNode( void )
 {
  // Clear self
  memset( this, 0, sizeof( CFileInfoNode ) );
  // Setup data member
  m_UsedFlag            = false;
  m_ProcessedFlag       = false;
  m_FileInfoType        = _fitUnknown;
  m_FileInfoStatus      = _fisUnknown;
  m_ChildFileUpdateFlag = false;
  m_ChildNodeNumber     = 0;
  // Clear file mark information
  m_SumMark64_H32 = 0;
  m_SumMark64_L32 = 0;
  m_XorMark       = 0;
  m_Adler32       = adler32( 0L, Z_NULL, 0 );
  m_Crc32         = crc32( 0L, Z_NULL, 0 );;
  // Clear list-link pointer
  m_pParentNode     = NULL;
  m_pFirstChildNode = NULL;
  m_pLastChildNode  = NULL;
  m_pPreviousNode   = NULL;
  m_pNextNode       = NULL;
 }

// Function member of assign node
void CFileInfoNode::AssignNode( CFileInfoNode *pNode )
 {
  // Copy node to data member
  m_FileInfoType  = pNode->m_FileInfoType;
  memcpy( &m_FileInfo, &(pNode->m_FileInfo), sizeof(ffblk ) );
 }

// Function member of adjust pointer address ( special function for save node )
void CFileInfoNode::AddBaseAddress( void *pAddress )
 {
  // Adjust pointer address ( parent node )
  if( m_pParentNode != NULL )
   *((long *)&m_pParentNode) += (*((long *)&pAddress) - 1);
  // Adjust pointer address ( first child node )
  if( m_pFirstChildNode != NULL )
   *((long *)&m_pFirstChildNode) += (*((long *)&pAddress) - 1);
  // Adjust pointer address ( last child node )
  if( m_pLastChildNode != NULL )
   *((long *)&m_pLastChildNode) += (*((long *)&pAddress) - 1);
  // Adjust pointer address ( previous node )
  if( m_pPreviousNode != NULL )
   *((long *)&m_pPreviousNode) += (*((long *)&pAddress) - 1);
  // Adjust pointer address ( next node )
  if( m_pNextNode != NULL )
   *((long *)&m_pNextNode) += (*((long *)&pAddress) - 1);
 }

// Function member of adjust pointer address ( special function for save node )
void CFileInfoNode::SubBaseAddress( void *pAddress )
 {
  // Adjust pointer address ( parent node )
  if( m_pParentNode != NULL )
   *((long *)&m_pParentNode) -= (*((long *)&pAddress) - 1);
  // Adjust pointer address ( first child node )
  if( m_pFirstChildNode != NULL )
   *((long *)&m_pFirstChildNode) -= (*((long *)&pAddress) - 1);
  // Adjust pointer address ( last child node )
  if( m_pLastChildNode != NULL )
   *((long *)&m_pLastChildNode) -= (*((long *)&pAddress) - 1);
  // Adjust pointer address ( previous node )
  if( m_pPreviousNode != NULL )
   *((long *)&m_pPreviousNode) -= (*((long *)&pAddress) - 1);
  // Adjust pointer address ( next node )
  if( m_pNextNode != NULL )
   *((long *)&m_pNextNode) -= (*((long *)&pAddress) - 1);
 }

// Function member of set file infomation
void CFileInfoNode::SetFileInfo( ffblk *pFileInfo )
 {
  // Store file information
  memcpy( &m_FileInfo, pFileInfo, sizeof( ffblk ) );
 }

// Function member of get filename
char *CFileInfoNode::GetFilename( void )
 {
  // Return filename
	 return m_FileInfo.name;
 }

// Function member of set filename
void CFileInfoNode::SetFilename( char *pFilename )
 {
  // Store filename
  strcpy( m_FileInfo.name, pFilename );
 }

// Function member of get file year
int CFileInfoNode::GetFileYear( void )
 {
  // Return file year
	 return ((m_FileInfo.time_access >> 9) + 1980);
 }

// Function member of get file month
int CFileInfoNode::GetFileMonth( void )
 {
  // Return file month
  return ((m_FileInfo.time_access >> 5) & 0xF);
 }

// Function member of get file day
int CFileInfoNode::GetFileDay( void )
 {
  // Return file day
  return (m_FileInfo.time_access & 0x1F);
 }

// Function member of get file hour
int CFileInfoNode::GetFileHour( void )
 {
  // Return file hour
  return (m_FileInfo.time_access >> 11);
 }

// Function member of get file minute
int CFileInfoNode::GetFileMinute( void )
 {
  // Return file minute
  return ((m_FileInfo.time_access >> 5) & 0x3F);
 }

// Function member of get file second
int CFileInfoNode::GetFileSecond( void )
 {
  // Return file second
	 return ((m_FileInfo.time_access & 0x1F) * 2); 
 }

// Function member of set an update flag
void CFileInfoNode::SetAncestorsUpdateFlag( void )
 {
  // Define local variable - node pointer
  CFileInfoNode *pNode;

  // Catch all parent name to buffer
  pNode = m_pParentNode;
  while( pNode != NULL )
   {
    // Is directory ?
    if( pNode->m_FileInfoType == _fitDirectory )
     pNode->SetChildFileUpdateFlag( true );
    // To parent node
    pNode = pNode->m_pParentNode;
   }
 }

// Function member of get full filename
void CFileInfoNode::GetFullFilename( char *pPath )
 {
  // Define local variable - node pointer
  CFileInfoNode *pNode;

  // Catch all parent name to buffer
  pNode = this;
  while( pNode != NULL )
   {
    // Is directory ?
    if( pNode->m_FileInfoType == _fitDirectory )
     ReverseStringAdd( pPath, "\\" );
    // Append filename
    ReverseStringAdd( pPath, pNode->GetFilename() );
    // To parent node
    pNode = pNode->m_pParentNode;
   }
  // Reverse string
  ReverseString( pPath );
 }

// Function member of append child node
void CFileInfoNode::AppendChildNode( CFileInfoNode *pNode )
 {
  // Set parent node
  pNode->m_pParentNode = this;
  // First node ?
  if( m_ChildNodeNumber == 0 )
   {
    // Store child node ( first child node )
    m_pFirstChildNode = pNode;
    // Store last child node
    m_pLastChildNode = pNode;
    // Setup borther list-link
    pNode->m_pPreviousNode = NULL;
    pNode->m_pNextNode     = NULL;
    // Adjust child number
    m_ChildNodeNumber++;
   }
  else  // Append child node to last link
   {
    // Insert node to last node
    m_pLastChildNode->InsertNodeToNext( pNode ); 
   }
 }

// Function member of insert node to previous space
void CFileInfoNode::InsertNodeToPrevious( CFileInfoNode *pNode )
 {
  // Change previous node's next node
  if( m_pPreviousNode != NULL )
   m_pPreviousNode->m_pNextNode = pNode;
  // Setup new node's list-link
  pNode->m_pPreviousNode = m_pPreviousNode;
  pNode->m_pNextNode     = this;
  // Change self's previous node
  m_pPreviousNode = pNode;
  // Adjust parent's child number
  if( m_pParentNode != NULL )
   {
    // Store parent node
    pNode->m_pParentNode = m_pParentNode;
    // Adjust parent's child number
    m_pParentNode->m_ChildNodeNumber += 1;
   }
 }

// Function member of insert node to next space
void CFileInfoNode::InsertNodeToNext( CFileInfoNode *pNode )
 {
  // Change next node's previous node
  if( m_pNextNode != NULL )
   m_pNextNode->m_pPreviousNode = pNode;
  // Setup new node's list-link
  pNode->m_pPreviousNode = this;
  pNode->m_pNextNode     = m_pNextNode;
  // Change self's next node
  m_pNextNode = pNode;
  // Have parent node ?
  if( m_pParentNode != NULL )
   {
    // Store parent node
    pNode->m_pParentNode = m_pParentNode;
    // Is last node ?
    if( pNode->m_pNextNode == NULL )
     m_pParentNode->m_pLastChildNode = pNode;
    // Adjust parent's child number
    m_pParentNode->m_ChildNodeNumber += 1;
   }
 }

// Function member of get brother node count
int CFileInfoNode::GetBrotherNodeCount( void )
 {
  // Define local variable - node pointer
  CFileInfoNode *pNode;
  // Define local variable - count
  int Count;

  // Get first node
  pNode = GetFirstBrotherNode();
  // Initialize count
  Count = 0;
  // Calculate brother node count
  while( pNode != NULL )
   {
    // Increment count
    Count++;
    // To next node
    pNode = pNode->m_pNextNode;
   }
  // Return count value
  return Count;
 }

// Function member of get first brother node
CFileInfoNode *CFileInfoNode::GetFirstBrotherNode( void )
 {
  // Define local variable - node pointer
  CFileInfoNode *pNode;

  // Initialize node pointer
  pNode = this;
  // Empty pointer ?
  while( pNode->m_pPreviousNode != NULL )
   pNode = pNode->m_pPreviousNode;
  // Return search result
  return pNode;
 }

// Function member of get brother node ( NULL = error )
CFileInfoNode *CFileInfoNode::GetBrotherNode( int OrderId )
 {
  // Define local variable - node pointer
  CFileInfoNode *pNode;
  // Define local variable - loop control
  int Index;

  // Get first node
  pNode = GetFirstBrotherNode();
  // Get node by index value
  for( Index = 0; Index < OrderId; Index++ )
   {
    // To next node
    pNode = pNode->m_pNextNode;
    // Order id overflow ?
    if( pNode == NULL )
     break;
   }
  // Return node pointer
  return pNode;
 }

// Function member of search brother node ( NULL = not found )
CFileInfoNode *CFileInfoNode::SearchBrotherNode( char *pFilename )
 {
  // Define local variable - node pointer
  CFileInfoNode *pNode;

  // Search first node
  pNode = this;
  while( pNode->m_pPreviousNode != NULL )
   pNode = pNode->m_pPreviousNode;
  // Scan all node
  while( pNode != NULL )
   {
    // Is same ?
    if( stricmp( pNode->GetFilename(), pFilename ) == 0 )
     break;
    // To next node
    pNode = pNode->m_pNextNode;
   }
  // Return search result
  return pNode;
 }

// Function member of record file mark
bool CFileInfoNode::RecordFileMark( void )
 {
  // Define local variable - full path
  char FullPath[ MAXPATH + MAXFILE + 1 ];
  // Define local variable - data buffer
  unsigned char *pDataBuffer;
  // Define local variable - result value
  unsigned long Result;
  // Define local variable - file size
  long FileSize;
  long ReadSize;
  // Define local variable - loop control
  int Index;
  // Define local variable - file pointer
  FILE *Fp;

  // Clear fullpath buffer
  memset( FullPath, 0, MAXPATH + MAXFILE + 1 );
  // Get full path name
  GetFullFilename( FullPath );
  // Allocate data buffer
  pDataBuffer = (unsigned char *)malloc( (size_t)_FileMarkDataBlockSize );
  if( pDataBuffer == NULL )
   return false;
  // Open file
  Fp = fopen( FullPath, "rb" );
  if( Fp == NULL )
   {
    // Free data buffer
    free( pDataBuffer );
    // Return error code
    return false;
   }
  // Calculate file length
  FileSize = filelength( fileno( Fp ) );
  // Read all file data for make file mark information
  while( FileSize > 0 )
   {
    // Calculate read size
    if( FileSize >= _FileMarkDataBlockSize )
     ReadSize = _FileMarkDataBlockSize;
    else
     ReadSize = FileSize;
    FileSize -= ReadSize;
    // Read file data
    if( fread( pDataBuffer, sizeof( char ), ReadSize, Fp ) != (unsigned long)ReadSize )
     {
      // Close file
      fclose( Fp );
      // Free data buffer
      free( pDataBuffer );
      // Return error code
      return false;
     }
    // Mark sum-mark
    for( Index = 0; Index < ReadSize; Index++ )
     {
      // By sum-method
      Result = m_SumMark64_L32 + (unsigned long)*(pDataBuffer + Index);
      if( m_SumMark64_L32 > Result )    // Overflow ?
       m_SumMark64_H32 += 1;
      m_SumMark64_L32 = Result;
      // By xor-method
      m_XorMark ^= (((unsigned long)*(pDataBuffer + Index)) << (8 * (Index % 4)));
     }
    // Calculate next alder32 value
    m_Adler32 = adler32( m_Adler32, pDataBuffer, ReadSize );
    // Calculate next crc32 value
    m_Crc32 = crc32( m_Crc32, pDataBuffer, ReadSize );
   }
  // Close file
  fclose( Fp );
  // Free data buffer
  free( pDataBuffer );
  // Record file mark successfully
  return true;
 }

// Function member of check same
bool CFileInfoNode::IsSame( CFileInfoNode *pNode )
 {
  /* Check date
  ff_ftime:
    Bits 0 to 4	        The result of seconds divided by 2 (for example 10 here means 20 seconds)
    Bits 5 to 10	Minutes
    Bits 11 to 15	Hours
  ff_fdate:
    Bits 0-4	        Day
    Bits 5-8	        Month
    Bits 9-15	        Years since 1980 (for example 9 here means 1989)
  */
	 if( (m_FileInfo.time_access != pNode->m_FileInfo.time_access) ||
      (m_FileInfo.time_access != pNode->m_FileInfo.time_access) )
   return false;
  // Check all file mark information
  if( (m_SumMark64_H32 != pNode->m_SumMark64_H32) ||
      (m_SumMark64_L32 != pNode->m_SumMark64_L32) ||
      (m_Adler32 != pNode->m_Adler32) ||
      (m_Crc32 != pNode->m_Crc32) )
   return false;
  // Return true = same
  return true;
 }

/* ----------------------------CDirectoryManager----------------------------- */

// Constructor
CDirectoryManager::CDirectoryManager( void )
 {
  // Setup data members
  m_pFileInfoPool  = NULL;
  m_pSortItemTable = NULL;
 }

// Destructor
CDirectoryManager::~CDirectoryManager( void )
 {
  // Shutdown directory manager
  ShutdownDirectoryManager();
 }

// Function member of allocate file info node
CFileInfoNode *CDirectoryManager::AllocFileInfoNode( void )
 {
  // Define local variable - node index
  int NodeIndex;

  // Not enough ?
  if( m_UsedFileInfoNumber == m_MaxFileInfoNumber )
   return NULL;
  // Store free node index
  NodeIndex = m_FreeNodeIndex;
  // Search next free node
  if( (m_UsedFileInfoNumber + 1) < m_MaxFileInfoNumber )
   {
    // Search ...
    while( 1 )
     {
      // Adjust free node index
      m_FreeNodeIndex++;
      // Overflow, back to head
      if( m_FreeNodeIndex >= m_MaxFileInfoNumber )
       m_FreeNodeIndex = 0; 
      // Is free node ?
      if( (m_pFileInfoPool + m_FreeNodeIndex)->GetUsedFlag() == false )
       break;                   // Exit search loop
     }
   }
  // Setup node to empty node
  (m_pFileInfoPool + NodeIndex)->SetupNode();
  // Mark node to used status
  (m_pFileInfoPool + NodeIndex)->SetUsedFlag( true );
  // Adjust node number
  m_UsedFileInfoNumber++;
  // Return pointer of free node
  return (m_pFileInfoPool + NodeIndex);
 }

// Function member of free file info node
void CDirectoryManager::FreeFileInfoNode( CFileInfoNode *pNode )
 {
  // Free node
  pNode->SetUsedFlag( false );
  // Need re-setup free node index
  if( m_UsedFileInfoNumber == m_MaxFileInfoNumber )
   {
    // Calculate new free node index
    m_FreeNodeIndex = (*((long *)&pNode) - *((long *)&m_pFileInfoPool))
                      / sizeof( CFileInfoNode );
   }
  // Adjust node number
  m_UsedFileInfoNumber--;
 }

// Function member of make sub directory tree
bool CDirectoryManager::MakeSubDirectoryTree(
                                             CFileInfoNode *pParentNode,
                                             bool           FileMarkFlag
                                            )
 {
  // Define local variable - full path
  char FullPath[ MAXPATH + MAXFILE + 1 ];
  // Define local variable - file information
  ffblk FileInfo;
  // Define local variable - node pointer
  CFileInfoNode *pNode;
  // Define local variable - search flag
  bool SearchFlag;

  // Make full path, and add wild-character
  memset( FullPath, 0, MAXPATH + MAXFILE + 1 );
  pParentNode->GetFullFilename( FullPath );
  strcat( FullPath, "*.*" );
  // Clear file info
  memset( &FileInfo, 0, sizeof( ffblk ) );
  // Search first node
  intptr_t ffret ;
  if( (ffret = _findfirst( FullPath,
                 &FileInfo)) != -1 )
   {
    // Search all file and directory
    SearchFlag = true;
    while( SearchFlag )
     {
      // Directory '.' or '..' is invalid
		 if( (stricmp( FileInfo.name, "." ) != 0) &&
          (stricmp( FileInfo.name, ".." ) != 0) )
       {
        // Allocate empty node
        pNode = AllocFileInfoNode();
        if( pNode == NULL )
         return false;
        // Append child node ?
        pParentNode->AppendChildNode( pNode );
        // Store this filename
        pNode->SetFileInfo( &FileInfo );
        // Clear path buffer
        memset( FullPath, 0, MAXPATH + MAXFILE + 1 );
        // Is directory ?
        if( FileInfo.attrib & _A_SUBDIR )
         {
          // Set file infomation type
          pNode->SetFileInfoType( _fitDirectory );
          // Get full path name
          pNode->GetFullFilename( FullPath );
          // Display scan infomation ?
          if( m_pDisplayFunction != NULL )
           {
            // Display file name
            m_pDisplayFunction( FullPath );
           }
          // Increment directory number
          m_DirectoryNumber++;
          // Make sub directory
          if( MakeSubDirectoryTree( pNode, FileMarkFlag ) == false )
           return false;
         }
        else      // File, archive
         {
          // Set file infomation type
          pNode->SetFileInfoType( _fitFile );
          // Get full path name
          pNode->GetFullFilename( FullPath );
          // Display scan infomation ?
          if( m_pDisplayFunction != NULL )
           {
            // Display file name
            m_pDisplayFunction( FullPath );
           }
          // Need record file mark ?
          if( FileMarkFlag == true )
           {
            // Record file mark successfully ?
            if( pNode->RecordFileMark() == false )
             return false;
           }
          // Increment file number
          m_FileNumber++;
         }
       }
      // Find next file
      if( _findnext(ffret, &FileInfo ) != 0 )
       SearchFlag = false;
     }
   }
  // Make sub-directory tree successfully
  return true;
 }

// Function member of sort directory function ( qsort, static function )
int CDirectoryManager::SortDirectoryFunction( const void *pItem1, const void *pItem2 )
 {
  // Define local variable - sort item pointer
  _dmSortItem *pSortItem1, *pSortItem2;

  // Convert to correct type
  pSortItem1 = (_dmSortItem *)pItem1;
  pSortItem2 = (_dmSortItem *)pItem2;
  // Same type ?
  if( pSortItem1->m_FileInfoType == pSortItem2->m_FileInfoType )
   {
    // Sort by filename
    return stricmp( pSortItem1->m_Filename, pSortItem2->m_Filename ); 
   }
  else
   {
    // Make directory at forward
    if( pSortItem1->m_FileInfoType == _fitDirectory )
     return -1;
    else
     return 1;
   }
 }

// Function member of sort and relink sub directory tree
bool CDirectoryManager::RelinkSubDirectoryTree( CFileInfoNode *pParentNode )
 {
  // Define local variable - file info node pointers
  CFileInfoNode *pBaseNode, *pWorkNode;
  // Define local variable - loop control
  int Index, Count;

  // Get first child node
  pBaseNode = pParentNode->GetFirstChildNode();
  if( pBaseNode == NULL )       // Empty directory, no include any file or directory
   return true;
  // Get all child number
  Count = pBaseNode->GetBrotherNodeCount();
  // Overflow ?
  if( Count > _MaximumSortItemNumber )
   return false;
  // Process all origin node
  for( Index = 0; Index < Count; Index++ )
   {
    // Get work node
    pWorkNode = pBaseNode->GetBrotherNode( Index );
    // Copy work node information to sort item
    (m_pSortItemTable + Index)->m_FileInfoType = pWorkNode->GetFileInfoType();
    strcpy( (m_pSortItemTable + Index)->m_Filename, pWorkNode->GetFilename() );
    (m_pSortItemTable + Index)->m_pNode = (void *)pWorkNode;
   }
  // Sort all filename by quick sort
  qsort( m_pSortItemTable, Count, sizeof( _dmSortItem ), SortDirectoryFunction );
  // Relink all node
  for( Index = 0; Index < Count; Index++ )
   {
    // Get work node
    pWorkNode = (CFileInfoNode *)(m_pSortItemTable + Index)->m_pNode;
    // Is first node ?
    if( Index == 0 )
     {
      // Assign to parent's first child node
      pParentNode->SetFirstChildNode( pWorkNode );
      // Clear this node's previous node pointer
      pWorkNode->SetPreviousNode( NULL );
     }
    // Link previous node
    if( Index > 0 )             // Not first node
     {
      // Link to new previous node
      pWorkNode->SetPreviousNode( (CFileInfoNode *)(m_pSortItemTable + (Index - 1))->m_pNode );
     }
    // Link next node
    if( Index != (Count - 1) )  // Not last node
     {
      // Link to new next node
      pWorkNode->SetNextNode( (CFileInfoNode *)(m_pSortItemTable + (Index + 1))->m_pNode );
     }
    // Is last node ?
    if( Index == (Count - 1) )
     {
      // Assign to parent's last child node
      pParentNode->SetLastChildNode( pWorkNode );
      // Clear this node's next node pointer
      pWorkNode->SetNextNode( NULL );
     }
   }
  // Sort and relink sub-directory tree
  return true;
 }

// Function member of sort sub directory tree
bool CDirectoryManager::SortSubDirectoryTree( CFileInfoNode *pParentNode )
 {
  // Define local variable - file info node pointers
  CFileInfoNode *pWorkNode;

  // Sort and relik sub directory tree
  if( RelinkSubDirectoryTree( pParentNode ) == false )
   return false;
  // Scan all child node
  pWorkNode = pParentNode->GetFirstChildNode();
  while( pWorkNode != NULL )
   {
    // Is directory node ?
    if( pWorkNode->GetFileInfoType() == _fitDirectory )
     {
      // Sort this directory
      if( SortSubDirectoryTree( pWorkNode ) == false )
       return false;
     }
    // Process next node
    pWorkNode = pWorkNode->GetNextNode();
   }
  // Sort sub-directory tree successfully
  return true;
 }

// Function member of startup directory manager
bool CDirectoryManager::StartupDirectoryManager( int FileInfoNumber )
 {
  // Invalid parameter ?
  if( FileInfoNumber <= 0 )
   return false;
  // Store argument to data member
  m_MaxFileInfoNumber = FileInfoNumber;
  // Allocate file information pool
  m_pFileInfoPool = new CFileInfoNode[ FileInfoNumber ];
  if( m_pFileInfoPool == NULL )
   return false;                        // Return error code
  // Clear directory manager
  ClearDirectoryManager();
  // Startup directory manager successfully
  return true;
 }

// Function member of shutdown directory manager
void CDirectoryManager::ShutdownDirectoryManager( void )
 {
  // Free file infomation pool
  if( m_pFileInfoPool != NULL )
   {
    // Delete file information pool
    delete[] m_pFileInfoPool;
    // Clear data member
    m_pFileInfoPool = NULL;
   }
 }

// Function member of clear directory manager
void CDirectoryManager::ClearDirectoryManager( void )
 {
  // Define local variable - loop control
  int Index;

  // Clear data member
  m_FreeNodeIndex      = 0;
  m_UsedFileInfoNumber = 0;
  m_DirectoryNumber    = 0;
  m_FileNumber         = 0;
  m_pFileInfoRootNode  = NULL;
  // Initialize all node in pool
  for( Index = 0; Index < m_MaxFileInfoNumber; Index++ )
   {
    // Setup node
    (m_pFileInfoPool + Index)->SetupNode();
   }
 }

// Function member of clear processed flag
void CDirectoryManager::ClearProcessedFlag( void )
 {
  // Define local variable - loop control
  int Index;

  // Setup all processed flag  
  for( Index = 0; Index < m_MaxFileInfoNumber; Index++ )
   {
    // Setup processed flag
    (m_pFileInfoPool + Index)->SetProcessedFlag( false );
   }
 }

// Function member of make directory tree
bool CDirectoryManager::MakeDirectoryTree(
                                          char           *pPath,
                                          bool           FileMarkFlag,
                                          _DsfcbFunction pDisplayFunction
                                         )
 {
  // Store argument to data member
  m_pDisplayFunction = pDisplayFunction;
  // Clear directory manager
  ClearDirectoryManager();
  // Allocate root node ( always ok )
  m_pFileInfoRootNode = AllocFileInfoNode();
  // Store path name
  m_pFileInfoRootNode->SetFileInfoType( _fitDirectory );
  m_pFileInfoRootNode->SetFilename( pPath );
  // Increment directory number
  m_DirectoryNumber++;
  // Display scan infomation ?
  if( m_pDisplayFunction != NULL )
   m_pDisplayFunction( pPath );
  // Make sub-directory tree
  if( MakeSubDirectoryTree( m_pFileInfoRootNode, FileMarkFlag ) == false )
   return false;
  // Make directory tree successfully
  return true;
 }

// Function member of sort directory tree
bool CDirectoryManager::SortDirectoryTree( void )
 {
  // Allocate sort item table
  m_pSortItemTable = (_dmSortItem *)malloc( sizeof( _dmSortItem ) *
                                            _MaximumSortItemNumber );
  if( m_pSortItemTable == NULL )
   return false;
  // Sort root-directory
  if( SortSubDirectoryTree( m_pFileInfoRootNode ) == false )
   {
    // Free sort item table
    free( m_pSortItemTable );
    // Return error code
    return false;
   }
  // Free sort item table
  free( m_pSortItemTable );
  // Sort directory tree successfully
  return true;
 }

// Function member of create root node
CFileInfoNode *CDirectoryManager::CreateFileInfoRootNode( void )
 {
  // Allocate root node ( always ok )
  m_pFileInfoRootNode = AllocFileInfoNode();
  // Return root node
  return m_pFileInfoRootNode;
 }
