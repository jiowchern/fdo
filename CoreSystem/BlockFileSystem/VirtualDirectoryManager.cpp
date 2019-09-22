/*
  Filename    : VirtualDirectoryManager.Cpp
  Author      : Curtis Lo
  Date        : 2003/09/18
  Descirption : Body file of virtual directory manager
*/
// Include standard header files
#include "stdafx.h"

#include <Stdlib.h>
#include <String.h>
#include <Stdio.h>

// Include private header files
#include "VirtualDirectoryManager.h"

// Define global symbol of maximum sort item number
#define _MaximumSortItemNumber          4096L

/* ---------------------------CVirtualFileInfoNode--------------------------- */

// Constructor
CVirtualFileInfoNode::CVirtualFileInfoNode( void )
 {
  // Setup this node
  SetupNode();
 }

// Destructor
CVirtualFileInfoNode::~CVirtualFileInfoNode( void )
 {

 }

// Function member of reverse string cat
void CVirtualFileInfoNode::ReverseStringAdd( char *pBuffer, char *pString )
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
  // Fill asciiz symbol
  *pTarget = 0;
 }

// Function member of reverse string
void CVirtualFileInfoNode::ReverseString( char *pString )
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
void CVirtualFileInfoNode::SetupNode( void )
 {
  // Clear self
  memset( this, 0, sizeof( CVirtualFileInfoNode ) );
  // Setup data member
  m_FileInfoType            = _vfitUnknown;
  m_UsedFlag                = false;
  m_FileOriginSize          = 0;
  m_FileSize                = 0;
  m_FileCompressBlockNumber = 0;
  m_FileOpenFlag            = false;
  m_FileAutoUncompressFlag  = false;
  m_FilePosition            = 0;
  m_BmpHandle               = -1;       // Empty handle
  m_ChildNodeNumber         = 0;
  // Clear list-link pointer
  m_pParentNode     = NULL;
  m_pFirstChildNode = NULL;
  m_pLastChildNode  = NULL;
  m_pPreviousNode   = NULL;
  m_pNextNode       = NULL;
 }

// Function member of assign node
void CVirtualFileInfoNode::AssignNode( CVirtualFileInfoNode *pNode )
 {
  // Copy node to data member
  m_FileInfoType = pNode->m_FileInfoType;
  strcpy( m_pFilename, pNode->m_pFilename );
 }

#ifdef __BORLANDC__
// Function member of assign node
void CVirtualFileInfoNode::AssignNode( CFileInfoNode *pNode )
 {
  // Copy node to data member
  m_FileInfoType = (_VirtualFileInfoType)pNode->GetFileInfoType();
  strcpy( m_pFilename, pNode->GetFilename() );
  // Copy file information to data member
  m_FileDate = pNode->GetFileInfo()->ff_fdate;
  m_FileTime = pNode->GetFileInfo()->ff_ftime;
 }
#endif

// Function member of adjust pointer address ( special function for save node )
void CVirtualFileInfoNode::AddBaseAddress( void *pAddress )
 {
  // Adjust pointer address ( parent node )
  if( m_pParentNode != NULL )
   *((long *)&m_pParentNode) += (*((long *)&pAddress) - 1);
  // Adjust pointer address ( child node )
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
void CVirtualFileInfoNode::SubBaseAddress( void *pAddress )
 {
  // Adjust pointer address ( parent node )
  if( m_pParentNode != NULL )
   *((long *)&m_pParentNode) -= (*((long *)&pAddress) - 1);
  // Adjust pointer address ( child node )
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

// Function member of get filename
char *CVirtualFileInfoNode::GetFilename( void )
 {
  // Return filename
  return m_pFilename;
 }

// Function member of set filename
void CVirtualFileInfoNode::SetFilename( char *pFilename )
 {
  // Store filename
  strcpy( m_pFilename, pFilename );
 }

// Function member of get full filename
void CVirtualFileInfoNode::GetFullFilename( char *pPath, bool RealPathFlag )
 {
  // Define local variable - node pointer
  CVirtualFileInfoNode *pNode;

  // Fill ascii symbol
  *pPath = 0;
  // Catch all parent name to buffer
  pNode = this;
  while( pNode != NULL )
   {
    // Is directory ?
    if( pNode->m_FileInfoType == _vfitDirectory )
     ReverseStringAdd( pPath, "\\" );
    // Is root directory ?
    if( (RealPathFlag == true) || (pNode->m_pParentNode != NULL) )
     {
      // Append filename
      ReverseStringAdd( pPath, pNode->GetFilename() );
     } 
    // To parent node
    pNode = pNode->m_pParentNode;
   }
  // Reverse string
  ReverseString( pPath );
 }

// Function memebr of get file size
int CVirtualFileInfoNode::GetFileSize( void )
 {
   // Define local variable - file size
  int FileSize;

  // Is auto uncompress mode ?
  if( m_FileAutoUncompressFlag == true )
   {
    // During initial ?
    if( m_FileOriginSize == 0 )
     FileSize = m_FileSize;
    else
     FileSize = m_FileOriginSize;
   }
  else          // Normal mode
   FileSize = m_FileSize;
  // Return file size
  return FileSize;
 }

// Function member of set file size
void CVirtualFileInfoNode::SetFileSize( int Size )
 {
  // Just store physical-size
  SetFilePhysicalSize( Size ); 
 }

// Function member of get file year
int CVirtualFileInfoNode::GetFileYear( void )
 {
  // Return file year
  return ((m_FileDate >> 9) + 1980);
 }

// Function member of set file year
void CVirtualFileInfoNode::SetFileYear( int Year )
 {

 }

// Function member of get file month
int CVirtualFileInfoNode::GetFileMonth( void )
 {
  // Return file month
  return ((m_FileDate >> 5) & 0xF);
 }

// Function member of set file month
void CVirtualFileInfoNode::SetFileMonth( int Month )
 {

 }

// Function member of get file day
int CVirtualFileInfoNode::GetFileDay( void )
 {
  // Return file day
  return (m_FileDate & 0x1F);
 }

// Function member of set file day
void CVirtualFileInfoNode::SetFileDay( int Day )
 {

 }

// Function member of get file hour
int CVirtualFileInfoNode::GetFileHour( void )
 {
  // Return file hour
  return (m_FileTime >> 11);
 }

// Function member of set file hour
void CVirtualFileInfoNode::SetFileHour( int Hour )
 {

 }

// Function member of get file minute
int CVirtualFileInfoNode::GetFileMinute( void )
 {
  // Return file minute
  return ((m_FileTime >> 5) & 0x3F);
 }

// Function member of set file minute
void CVirtualFileInfoNode::SetFileMinute( int Minute )
 {

 }

// Function member of get file second
int CVirtualFileInfoNode::GetFileSecond( void )
 {
  // Return file second
  return ((m_FileTime & 0x1F) * 2);
 }

// Function member of set file second
void CVirtualFileInfoNode::SetFileSecond( int Second )
 {

 }

// Function member of set file position
bool CVirtualFileInfoNode::SetFilePosition( int Position )
 {
  // File opened ?
  if( m_FileOpenFlag == false )
   return false;
  // Invalid position ?
  if( (Position < 0) || (Position > GetFileSize()) )
   return false;
  // Store position
  m_FilePosition = Position;
  // Set file position successfully
  return true;
 }

// Function member of append child node
void CVirtualFileInfoNode::AppendChildNode( CVirtualFileInfoNode *pNode )
 {
  // Set parent node
  pNode->m_pParentNode = this;
  // First node ?
  if( m_ChildNodeNumber == 0 )
   {
    // Store child node ( first node )
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
void CVirtualFileInfoNode::InsertNodeToPrevious( CVirtualFileInfoNode *pNode )
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
   m_pParentNode->m_ChildNodeNumber += 1;
 }

// Function member of insert node to next space
void CVirtualFileInfoNode::InsertNodeToNext( CVirtualFileInfoNode *pNode )
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
    // Is last node ?
    if( pNode->m_pNextNode == NULL )
     m_pParentNode->m_pLastChildNode = pNode;
    // Adjust parent's child number
    m_pParentNode->m_ChildNodeNumber += 1;
   }
 }

// Function member of get brother node count
int CVirtualFileInfoNode::GetBrotherNodeCount( void )
 {
  // Define local variable - node pointer
  CVirtualFileInfoNode *pNode;
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
CVirtualFileInfoNode *CVirtualFileInfoNode::GetFirstBrotherNode( void )
 {
  // Define local variable - node pointer
  CVirtualFileInfoNode *pNode;

  // Initialize node pointer
  pNode = this;
  // Empty pointer ?
  while( pNode->m_pPreviousNode != NULL )
   pNode = pNode->m_pPreviousNode;
  // Return search result
  return pNode;
 }

// Function member of get brother node ( NULL = error )
CVirtualFileInfoNode *CVirtualFileInfoNode::GetBrotherNode( int OrderId )
 {
  // Define local variable - node pointer
  CVirtualFileInfoNode *pNode;
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
CVirtualFileInfoNode *CVirtualFileInfoNode::SearchBrotherNode( char *pFilename )
 {
  // Define local variable - node pointer
  CVirtualFileInfoNode *pNode;

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

/* -------------------------CVirtualDirectoryManager------------------------- */

// Constructor
CVirtualDirectoryManager::CVirtualDirectoryManager( void )
 {
  // Setup data members
  m_pFileInfoPool = NULL;
 }

// Destructor
CVirtualDirectoryManager::~CVirtualDirectoryManager( void )
 {
  // Shutdown directory manager
  ShutdownVirtualDirectoryManager();
 }

// Function member of allocate file info node
CVirtualFileInfoNode *CVirtualDirectoryManager::AllocFileInfoNode( void )
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
  else          // Clear free node index
   m_FreeNodeIndex = 0;
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
void CVirtualDirectoryManager::FreeFileInfoNode( CVirtualFileInfoNode *pNode )
 {
  // Free node
  pNode->SetUsedFlag( false );
  // Need re-setup free node index ?
  if( m_UsedFileInfoNumber == m_MaxFileInfoNumber )
   {
    // Calculate new free node index
    m_FreeNodeIndex = (*((long *)&pNode) - *((long *)&m_pFileInfoPool))
                      / sizeof( CVirtualFileInfoNode );
   }
  // Adjust node number
  m_UsedFileInfoNumber--;
 }

#ifdef __BORLANDC__
// Function member of append file
CVirtualFileInfoNode *CVirtualDirectoryManager::AppendFile(
                                                           CFileInfoNode        *pFileNode,
                                                           CVirtualFileInfoNode *pStoreNode,
                                                           bool                 ChildFlag
                                                          )
 {
  // Define local variable - file info node pointers
  CVirtualFileInfoNode *pNode;

  // Allocate empty node
  pNode = AllocFileInfoNode();
  // Allocate ok ?
  if( pNode == NULL )
   return NULL;
  // Assign file information from file node
  pNode->AssignNode( pFileNode );
  // Is child node ?
  if( ChildFlag == true )
   {
    // Store it like child node
    pStoreNode->AppendChildNode( pNode );
   }
  else
   {
    // Store it like brother node
    pStoreNode->InsertNodeToNext( pNode );
   }
  // Incremnt file number
  m_FileNumber++;
  // Append successfully
  return pNode;
 }

// Function member of append directory
CVirtualFileInfoNode *CVirtualDirectoryManager::AppendDirectory(
                                                                CFileInfoNode        *pDirectoryNode,
                                                                CVirtualFileInfoNode *pStoreNode,
                                                                bool                  ChildFlag
                                                               )
 {
  // Define local variable - file info node pointers
  CVirtualFileInfoNode *pNode;

  // Allocate empty node
  pNode = AllocFileInfoNode();
  // Allocate ok ?
  if( pNode == NULL )
   return NULL;
  // Assign file information from directory node
  pNode->AssignNode( pDirectoryNode );
  // Is child node ?
  if( ChildFlag == true )
   {
    // Store it like child node
    pStoreNode->AppendChildNode( pNode );
   }
  else
   {
    // Store it like brother node
    pStoreNode->InsertNodeToNext( pNode );
   }
  // Incremnt file number
  m_DirectoryNumber++;
  // Append successfully
  return pNode;
 }

// Function member of generate directory manager
bool CVirtualDirectoryManager::GenerateDirectoryManager(
                                                        CFileInfoNode        *pSourceNode,
                                                        CVirtualFileInfoNode *pStoreNode,
                                                        bool                 UpdateModeFlag
                                                       )
 {
  // Define local variable - file info node pointers
  CFileInfoNode        *pSrcWorkNode;
  CVirtualFileInfoNode *pNode;

  // Scan all child node
  pSrcWorkNode = pSourceNode->GetFirstChildNode();
  while( pSrcWorkNode != NULL )
   {
    // Is directory node ?
    if( pSrcWorkNode->GetFileInfoType() == _fitDirectory )
     {
      // Is update mode ?
      if( UpdateModeFlag == true )
       {
        // Is new directory, or child node be modifed ?
        if( (pSrcWorkNode->GetFileInfoStatus() == _fisNew) ||
            (pSrcWorkNode->GetChildFileUpdateFlag() == true) )
         {
          // Append directory
          if( (pNode = AppendDirectory( pSrcWorkNode, pStoreNode, true )) == NULL )
           return false;
          // Scan this directory
          if( GenerateDirectoryManager( pSrcWorkNode, pNode, UpdateModeFlag ) == false )
           return false;
         }
       }
      else      // Normal mode, bind all item
       {
        // Append directory
        if( (pNode = AppendDirectory( pSrcWorkNode, pStoreNode, true )) == NULL )
         return false;
        // Scan this directory
        if( GenerateDirectoryManager( pSrcWorkNode, pNode, UpdateModeFlag ) == false )
         return false;
       }
     }
    else        // File
     {
      // Is update mode ?
      if( UpdateModeFlag == true )
       {
        // Is update or new file ?
        if( (pSrcWorkNode->GetFileInfoStatus() == _fisUpdate) ||
            (pSrcWorkNode->GetFileInfoStatus() == _fisNew) )
         {
          // Append file
          if( AppendFile( pSrcWorkNode, pStoreNode, true ) == NULL )
           return false;
         }
       }
      else      // Normal mode, bind all item
       {
        // Append file
        if( AppendFile( pSrcWorkNode, pStoreNode, true ) == NULL )
         return false;
       }
     }
    // Process next node
    pSrcWorkNode = pSrcWorkNode->GetNextNode();
   }
  // Generate directory manager successfully
  return true;
 }
#endif

// Function member of sort directory function ( qsort, static function )
int CVirtualDirectoryManager::SortDirectoryFunction( const void *pItem1, const void *pItem2 )
 {
  // Define local variable - sort item pointer
  _vdmSortItem *pSortItem1, *pSortItem2;

  // Convert to correct type
  pSortItem1 = (_vdmSortItem *)pItem1;
  pSortItem2 = (_vdmSortItem *)pItem2;
  // Same type ?
  if( pSortItem1->m_FileInfoType == pSortItem2->m_FileInfoType )
   {
    // Sort by filename
    return stricmp( pSortItem1->m_Filename, pSortItem2->m_Filename );
   }
  else
   {
    // Make directory at forward
    if( pSortItem1->m_FileInfoType == _vfitDirectory )
     return -1;
    else
     return 1;
   }
 }

// Function member of sort and relink sub directory tree
bool CVirtualDirectoryManager::RelinkSubDirectoryTree( CVirtualFileInfoNode *pParentNode )
 {
  // Define local variable - file info node pointers
  CVirtualFileInfoNode *pBaseNode, *pWorkNode;
  // Define local variable - loop control
  int Index, Count;

  // Get first child node
  pBaseNode = pParentNode->GetFirstChildNode();
  if( pBaseNode == NULL )       // Empty directory, not include any file or directory
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
    (m_pSortItemTable + Index)->m_FileInfoType = (_VirtualFileInfoType)pWorkNode->GetFileInfoType();
    strcpy( (m_pSortItemTable + Index)->m_Filename, pWorkNode->GetFilename() );
    (m_pSortItemTable + Index)->m_pNode = (void *)pWorkNode;
   }
  // Sort all filename by quick sort
  qsort( m_pSortItemTable, Count, sizeof( _vdmSortItem ), SortDirectoryFunction );
  // Relink all node
  for( Index = 0; Index < Count; Index++ )
   {
    // Get work node
    pWorkNode = (CVirtualFileInfoNode *)(m_pSortItemTable + Index)->m_pNode;
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
      pWorkNode->SetPreviousNode( (CVirtualFileInfoNode *)(m_pSortItemTable + (Index - 1))->m_pNode );
     }
    // Link next node
    if( Index != (Count - 1) )  // Not last node
     {
      // Link to new next node
      pWorkNode->SetNextNode( (CVirtualFileInfoNode *)(m_pSortItemTable + (Index + 1))->m_pNode );
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
bool CVirtualDirectoryManager::SortSubDirectoryTree( CVirtualFileInfoNode *pParentNode )
 {
  // Define local variable - file info node pointers
  CVirtualFileInfoNode *pWorkNode;

  // Sort and relik sub directory tree
  if( RelinkSubDirectoryTree( pParentNode ) == false )
   return false;
  // Scan all child node
  pWorkNode = pParentNode->GetFirstChildNode();
  while( pWorkNode != NULL )
   {
    // Is directory node ?
    if( pWorkNode->GetFileInfoType() == _vfitDirectory )
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

// Function member of add base address ( convert all node )
void CVirtualDirectoryManager::AddBaseAddress_FileInfoPool( int NodeNumber, void *Address )
 {
  // Define local variable - node pointer
  CVirtualFileInfoNode *pNode;
  // Define local variable - loop control
  int Index;

  // Process all node
  for( Index = 0; Index < NodeNumber; Index++ )
   {
    // Get process node
    pNode = m_pFileInfoPool + Index;
    // Add base address
    pNode->AddBaseAddress( Address );
   }
 }

// Function member of sub base address ( convert all node )
void CVirtualDirectoryManager::SubBaseAddress_FileInfoPool( int NodeNumber, void *Address )
 {
  // Define local variable - node pointer
  CVirtualFileInfoNode *pNode;
  // Define local variable - loop control
  int Index;

  // Process all node
  for( Index = 0; Index < NodeNumber; Index++ )
   {
    // Get process node
    pNode = m_pFileInfoPool + Index;
    // Sub base address
    pNode->SubBaseAddress( Address );
   }
 }

// Function member of get node number of compact mode
int CVirtualDirectoryManager::GetNodeNumberOfCompactMode( void )
 {
  // Define local variable - loop control
  int Index;

  // Scan all node
  for( Index = m_MaxFileInfoNumber - 1; Index >= 0; Index-- )
   {
    // Be used ?
    if( (m_pFileInfoPool + Index)->GetUsedFlag() == true )
     break;                   // Exit search loop
   }
  // Return node number of compact mode
  return Index + 1;
 }

// Function member of get max number of compress block
int CVirtualDirectoryManager::GetMaxNumberOfCompressBlock( void )
 {
  // Define local variable - block number
  int BlockNumber;
  // Define local variable - loop control
  int Index;

  // Clar variable
  BlockNumber = 0;
  // Scan all node
  for( Index = 0; Index < m_MaxFileInfoNumber; Index++ )
   {
    // Is be-used node ?
    if( (m_pFileInfoPool + Index)->GetUsedFlag() == true )
     {
      // Is biger number ?
      if( BlockNumber < (m_pFileInfoPool + Index)->GetFileCompressBlockNumber() )
       BlockNumber = (m_pFileInfoPool + Index)->GetFileCompressBlockNumber();
     }
   }
  // Return block number
  return BlockNumber;
 }

// Function member of import virtual directory manager file
bool CVirtualDirectoryManager::ImportVdmFile( FILE* pFile, int ExpandFileInfoNumber )
 {
  // Define local variable - vdm file header
  _CVdmFileHeader FileHeader;
  // Define local variable - node pointer
  CVirtualFileInfoNode *pNode;
  // Define local variable - loop control
  int Index, Count;

  // Parameter error ?
  if( ExpandFileInfoNumber < 0 )
   return false;

  // Read file header
  if( fread( &FileHeader, 1, sizeof( FileHeader ), pFile ) != sizeof( FileHeader ) )
	return false;

  // Restore to data member
  m_MaxFileInfoNumber      = FileHeader.m_MaxFileInfoNumber;
  m_UsedFileInfoNumber     = FileHeader.m_UsedFileInfoNumber;
  m_FreeNodeIndex          = FileHeader.m_FreeNodeIndex;
  m_DirectoryNumber        = FileHeader.m_DirectoryNumber;
  m_FileNumber             = FileHeader.m_FileNumber;
  m_MaxCompressBlockNumber = FileHeader.m_MaxCompressBlockNumber;
  // Allocate file information pool
  m_pFileInfoPool = new CVirtualFileInfoNode[ m_MaxFileInfoNumber + ExpandFileInfoNumber ];
  if( m_pFileInfoPool == NULL )
   return false;                        // Return error code
  // Read file info pool
  unsigned int ReadSize = m_MaxFileInfoNumber * sizeof( CVirtualFileInfoNode );

  if( fread( m_pFileInfoPool, 1, ReadSize, pFile ) != ReadSize )
	return false;  

  // Convert to address mode
  AddBaseAddress_FileInfoPool( m_MaxFileInfoNumber, m_pFileInfoPool );
  // Assign root node
  m_pFileInfoRootNode = m_pFileInfoPool;
  // Process all node
  for( Index = 0; Index < m_MaxFileInfoNumber; Index++ )
   {
    // Get process node
    pNode = m_pFileInfoPool + Index;
    // Clear file status
    pNode->SetFileOpenFlag( false );
    pNode->SetFileAutoUncompressFlag( false );
    pNode->SetFilePosition( 0 );
   }
  // Have expand file-info number ?
  if( ExpandFileInfoNumber > 0 )
   {
    // Initialize all node in pool
    Count = m_MaxFileInfoNumber + ExpandFileInfoNumber;
    for( Index = m_MaxFileInfoNumber; Index < Count; Index++ )
     {
      // Setup node
      (m_pFileInfoPool + Index)->SetupNode();
     }
    // Adjust data members
    if( m_FreeNodeIndex == 0 )          // Full or compact-mode
     m_FreeNodeIndex = m_MaxFileInfoNumber;
    m_MaxFileInfoNumber += ExpandFileInfoNumber;
   }
  // Import virtual directory manager file successfully
  return true;
 }

// Function member of export virtual directory manager file
bool CVirtualDirectoryManager::ExportVdmFile( FILE* pFile, bool CompactModeFlag )
 {
  // Define local variable - vdm file header
  _CVdmFileHeader FileHeader;
  // Define local variable - node number
  int NodeNumber;
  // Define local variable - write size
  unsigned int WriteSize;

  // Fill vdm information to file header
  if( CompactModeFlag == true )         // Compact mode ?
   {
    // Get node number of compact mode
    NodeNumber = GetNodeNumberOfCompactMode();
    // Set to compact mode
    FileHeader.m_MaxFileInfoNumber  = NodeNumber;
    FileHeader.m_UsedFileInfoNumber = NodeNumber;
    FileHeader.m_FreeNodeIndex      = 0;
   }
  else          // Normal mode
   {
    // Store number from origin setup, ( keep free node for future )
    FileHeader.m_MaxFileInfoNumber  = m_MaxFileInfoNumber;
    FileHeader.m_UsedFileInfoNumber = m_UsedFileInfoNumber;
    FileHeader.m_FreeNodeIndex      = m_FreeNodeIndex;
   }
  FileHeader.m_DirectoryNumber = m_DirectoryNumber;
  FileHeader.m_FileNumber      = m_FileNumber;
  // Calculate max. number of file compress block
  FileHeader.m_MaxCompressBlockNumber = GetMaxNumberOfCompressBlock();

  // Write file header
  if( fwrite( &FileHeader, sizeof( FileHeader ), 1, pFile ) != sizeof( FileHeader ) )
	return false;

  // Convert all node's pointer to offset address
  SubBaseAddress_FileInfoPool( FileHeader.m_MaxFileInfoNumber, m_pFileInfoPool );
  // Write file info pool
  WriteSize = FileHeader.m_MaxFileInfoNumber * sizeof( CVirtualFileInfoNode );  

  if( fwrite( m_pFileInfoPool, WriteSize, 1, pFile ) != WriteSize )
   {
    // Recover file info pool
    AddBaseAddress_FileInfoPool( FileHeader.m_MaxFileInfoNumber, m_pFileInfoPool );
    // Return error code
    return false;
   }
  // Recover file info pool
  AddBaseAddress_FileInfoPool( FileHeader.m_MaxFileInfoNumber, m_pFileInfoPool );
  // Export virtual directory manager file successfully
  return true;
 }

// Function member of startup directory manager
bool CVirtualDirectoryManager::StartupVirtualDirectoryManager( int FileInfoNumber )
 {
  // Invalid parameter ?
  if( FileInfoNumber <= 0 )
   return false;
  // Store argument to data member
  m_MaxFileInfoNumber = FileInfoNumber;
  // Allocate file information pool
  m_pFileInfoPool = new CVirtualFileInfoNode[ FileInfoNumber ];
  if( m_pFileInfoPool == NULL )
   return false;                        // Return error code
  // Clear virtual directory manager
  ClearVirtualDirectoryManager();
  // Startup directory manager successfully
  return true;
 }

// Function member of shutdown directory manager
void CVirtualDirectoryManager::ShutdownVirtualDirectoryManager( void )
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
void CVirtualDirectoryManager::ClearVirtualDirectoryManager( void )
 {
  // Define local variable - loop control
  int Index;

  // Clear data member
  m_UsedFileInfoNumber     = 0;
  m_DirectoryNumber        = 0;
  m_FreeNodeIndex          = 0;
  m_FileNumber             = 0;
  m_MaxCompressBlockNumber = 0;
  m_pFileInfoRootNode      = NULL;
  // Initialize all node in pool
  for( Index = 0; Index < m_MaxFileInfoNumber; Index++ )
   {
    // Setup node
    (m_pFileInfoPool + Index)->SetupNode();
   }
 }

#ifdef __BORLANDC__
// Function member of assign virtual directory manager
bool CVirtualDirectoryManager::AssignVirtualDirectoryManager(
                                                             CDirectoryManager *pDirectoryManager,
                                                             bool              UpdateModeFlag
                                                            )
 {
  // Define local variable - file info node pointers
  CFileInfoNode        *pSourceNode;
  CVirtualFileInfoNode *pStoreNode;

  // Clear virtual directory manager
  ClearVirtualDirectoryManager();
  // Get source root node
  pSourceNode = pDirectoryManager->GetFileInfoRootNode();
  // Allocate root node
  pStoreNode = CreateFileInfoRootNode();
  // Assign to origin node
  pStoreNode->AssignNode( pSourceNode );
  // Increment directory number
  m_DirectoryNumber++;
  // Generate directory manager
  if( GenerateDirectoryManager( pSourceNode, pStoreNode, UpdateModeFlag ) == false )
   return false;
  // Sort directory tree
  if( SortDirectoryTree() == false )
   return false;
  // Assign directory manager successfully
  return true;
 }
#endif  

// Function member of sort directory tree
bool CVirtualDirectoryManager::SortDirectoryTree( void )
 {
  // Allocate sort item table
  m_pSortItemTable = (_vdmSortItem *)malloc( sizeof( _vdmSortItem ) *
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

// Function member of search file name
int CVirtualDirectoryManager::SearchFilename( int ParentPathHandle, const char *pFilename )
 {
  // Define local variable - node pointer
  CVirtualFileInfoNode *pParentNode, *pNode;
  // Define local variable - text buffer
  char TextBuffer[ _MaxFilenameSize + 1 ];
  char *pText;
  // Define local variable - found flag
  bool FoundFlag;

  // Clear found flag
  FoundFlag = false;
  // Search node by filename
  pParentNode = m_pFileInfoPool + ParentPathHandle;
  while( 1 )
   {
    // Is directory symbol ?
    if( *pFilename == '\\' )    // Skip this symbol
     pFilename++;
    // Copy directory or filename to text buffer
    pText = TextBuffer;
    while( 1 )
     {
      // Is end symbol ?
      if( (*pFilename == '\\') || (*pFilename == 0) )
       break;
      // Fill to text buffer
      *pText++ = *pFilename++;
     }
    // Fill asciiz symbol
    *pText = 0;
    // Search end ?
    if( TextBuffer[ 0 ] == 0 )          // Asciiz ?
     break;
    // Get first child node
    pNode = pParentNode->GetFirstChildNode();
    // Have any child node ?
    if( pNode == NULL )
     {
      // Clear found flag
      FoundFlag = false;
      // Exit loop
      break;
     }
    // Search all child node by filename
    pParentNode = pNode->SearchBrotherNode( TextBuffer );
    // Filename exist ?
    if( pParentNode == NULL )
     {
      // Clear found flag
      FoundFlag = false;
      // Exit loop
      break;
     }
    else        // Found
     {
      // Set found flag
      FoundFlag = true;
      // Is invalid search ?
      if( pParentNode->GetFileInfoType() == _vfitFile )
       if( *pFilename == '\\' )         // Error
        {
         // Clear found flag
         FoundFlag = false;
         // Exit loop
         break;
        }
     }
   }
  // Found ?
  if( FoundFlag == true )
   {
    // Calculate handle value ( node index value )
    return ((*((long *)&pParentNode) - *((long *)&m_pFileInfoPool)) /
           sizeof( CVirtualFileInfoNode ));
   }
  else          // Not found
   return -1;
 }

// Function member of get file info node
CVirtualFileInfoNode *CVirtualDirectoryManager::GetFileInfoNode( int NodeIndex )
 {
  // Overflow ?
  if( NodeIndex >= m_MaxFileInfoNumber )
   return NULL;
  // Return pointer of file info node
  return m_pFileInfoPool + NodeIndex;
 }

// Function member of get node index
int CVirtualDirectoryManager::GetNodeIndex( CVirtualFileInfoNode *pNode )
 {
  // Calculate handle value ( node index value )
  return ((*((long *)&pNode) - *((long *)&m_pFileInfoPool)) /
          sizeof( CVirtualFileInfoNode ));
 }

// Function member of create root node
CVirtualFileInfoNode *CVirtualDirectoryManager::CreateFileInfoRootNode( void )
 {
  // Allocate root node ( always ok )
  m_pFileInfoRootNode = AllocFileInfoNode();
  // Return root node
  return m_pFileInfoRootNode;
 }

// Fucntion member of create root directory
CVirtualFileInfoNode *CVirtualDirectoryManager::CreateRootDirectoryNode( char *pFilename )
 {
  // Define local variable - node pointer
  CVirtualFileInfoNode *pNode;

  // Create root node
  pNode = CreateFileInfoRootNode();
  // Set file info type.
  pNode->SetFileInfoType( _vfitDirectory );
  // Store file name ( don't care )
  pNode->SetFilename( pFilename );
  // Return pointer of node
  return pNode;
 }

// Function memebr of create directory node ( NULL = error )
CVirtualFileInfoNode *CVirtualDirectoryManager::CreateDirectoryNode(
                                                                    int ParentPathHandle,
                                                                    char *pFilename
                                                                   )
 {
  // Define local variable - node pointer
  CVirtualFileInfoNode *pParentNode, *pNode;

  // Get parent node pointer
  pParentNode = m_pFileInfoPool + ParentPathHandle;
  // Duplicate name ?
  if( pParentNode->GetFirstChildNode() != NULL )        // Have any child node ?
   if( pParentNode->GetFirstChildNode()->SearchBrotherNode( pFilename ) != NULL )
    return NULL;
  // Alloc new node
  pNode = AllocFileInfoNode();
  // Alloc error ?
  if( pNode == NULL )
   return NULL;
  // Set file info type.
  pNode->SetFileInfoType( _vfitDirectory );
  // Store file name
  pNode->SetFilename( pFilename );
  // Adjust to child node
  pParentNode->AppendChildNode( pNode );
  // Return pointer of node
  return pNode;
 }

// Function member of create file node ( NULL = error )
CVirtualFileInfoNode *CVirtualDirectoryManager::CreateFileNode(
                                                               int ParentPathHandle,
                                                               char *pFilename
                                                              )
 {
  // Define local variable - node pointer
  CVirtualFileInfoNode *pParentNode, *pNode;

  // Get parent node pointer
  pParentNode = m_pFileInfoPool + ParentPathHandle;
  // Duplicate name ?
  if( pParentNode->GetFirstChildNode() != NULL )        // Have any child node ?
   if( pParentNode->GetFirstChildNode()->SearchBrotherNode( pFilename ) != NULL )
    return NULL;
  // Alloc new node
  pNode = AllocFileInfoNode();
  // Alloc error ?
  if( pNode == NULL )
   return NULL;
  // Set file info type.
  pNode->SetFileInfoType( _vfitFile );
  // Store file name
  pNode->SetFilename( pFilename );
  // Adjust to child node
  pParentNode->AppendChildNode( pNode );
  // Return pointer of node
  return pNode;
 }

