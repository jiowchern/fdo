/*
  Filename    : VirtualDirectoryManager.H
  Author      : Curtis Lo
  Date        : 2003/07/08
  Descirption : Header file of virtual directory manager
*/
// Avoid recurrence include this file, Start condition compile
#ifndef __VirtualDirectoryManager_H
#define __VirtualDirectoryManager_H

 // Include private header files
 #include "Defines.h"
 #include <stdio.h>

 #ifdef __BORLANDC__
  // Include private header files
  #include "DirectoryManager.h"
 #endif

 #ifdef __BORLANDC__
  // Set enum type to int ( 4 byte ) for fit visual c++
  #pragma option push
  #pragma option -b
 #endif

 // Define symbol of file information type
 enum _VirtualFileInfoType { _vfitUnknown, _vfitDirectory, _vfitFile };

 #ifdef __BORLANDC__
  // Back to default alignment
  #pragma option pop
 #endif

 // Define global structure of sort item
 struct _vdmSortItem
  {
   // Declare data member of file info type
   _VirtualFileInfoType m_FileInfoType;
   char                 m_Filename[ _MaxFilenameSize + 1 ];
   void                 *m_pNode;
  };

 #ifdef __BORLANDC__
  // Set data member's align byte to 1 bytes
  #pragma option push
  #pragma option -a1
 #else
  // Adjust structure alignment to 1 byte ( default is 8 byte )
  #pragma pack( push )
  #pragma pack( 1 )
 #endif

 // Declare structure of virtual directory manager's file header
 struct _CVdmFileHeader
  {
   // Data member of pool information
   int m_MaxFileInfoNumber;
   int m_UsedFileInfoNumber;
   // Data member of virtual directory manager information
   int m_FreeNodeIndex;
   // Data member of item number
   int m_DirectoryNumber;
   int m_FileNumber;
   // Data member of max. number of file compress block
   int m_MaxCompressBlockNumber;
  };

 // Declare class of virtual file infomation node
 class CVirtualFileInfoNode
  {
   private:
    // Data member of file information
    _VirtualFileInfoType m_FileInfoType;
    // Data member of file name
    char m_pFilename[ _MaxFilenameSize + 1 ];
    // Data member of used flag
    bool m_UsedFlag;
    // Data member of general file status
    int m_FileOriginSize;
    int m_FileSize;
    // Data member of compress file status
    int m_FileCompressBlockNumber;
    // Data member of file data/time information
    unsigned short m_FileDate;
    unsigned short m_FileTime;
    // Data member of file open status
    bool m_FileOpenFlag;
    bool m_FileAutoUncompressFlag;
    int  m_FilePosition;
    // Data member of block memory pool's handle
    int m_BmpHandle;
    // Data member of hierarchy list-link
    CVirtualFileInfoNode *m_pParentNode;
    CVirtualFileInfoNode *m_pFirstChildNode;
    CVirtualFileInfoNode *m_pLastChildNode;
    int m_ChildNodeNumber;
    // Data member of borther list-link
    CVirtualFileInfoNode *m_pPreviousNode;
    CVirtualFileInfoNode *m_pNextNode;
    // Function member of reverse string and append to buffer
    void ReverseStringAdd( char *pBuffer, char *pString );
    // Function member of reverse string
    void ReverseString( char *pString );
   public:
    // Constructor
    CVirtualFileInfoNode( void );
    // Destructor
    ~CVirtualFileInfoNode( void );
    // Function member of setup node
    void SetupNode( void );
    // Function member of assign node
    void AssignNode( CVirtualFileInfoNode *pNode );
    #ifdef __BORLANDC__
     void AssignNode( CFileInfoNode *pNode );
    #endif
    // Function member of adjust pointer address ( special function for save node )
    void SubBaseAddress( void *pAddress );
    void AddBaseAddress( void *pAddress );
    // Function member of file info type
    _VirtualFileInfoType GetFileInfoType( void )        { return m_FileInfoType; }
    void SetFileInfoType( _VirtualFileInfoType Type )   { m_FileInfoType = Type; }
    // Function member of file name
    char *GetFilename( void );
    void SetFilename( char *pFilename );
    // Function member of get full filename
    void GetFullFilename( char *pPath, bool RealPathFlag );
    // Function member of get/set used flag
    bool GetUsedFlag( void )            { return m_UsedFlag; }
    void SetUsedFlag( bool Flag )       { m_UsedFlag = Flag; }
    // Function member of get/set general file status
    int  GetFileOriginSize( void )      { return m_FileOriginSize; }
    void SetFileOriginSize( int Size )  { m_FileOriginSize = Size; }
    int  GetFilePhysicalSize( void )    { return m_FileSize; }
    void SetFilePhysicalSize( int Size ){ m_FileSize = Size; }
    int  GetFileSize( void );
    void SetFileSize( int Size );
    // Function member of get/set compress file status
    int  GetFileCompressBlockNumber( void )             { return m_FileCompressBlockNumber; }
    void SetFileCompressBlockNumber( int Number )       { m_FileCompressBlockNumber = Number; }
    // Function member of get/set date/time information
    int  GetFileYear( void );
    void SetFileYear( int Year );
    int  GetFileMonth( void );
    void SetFileMonth( int Month );
    int  GetFileDay( void );
    void SetFileDay( int Day );
    int  GetFileHour( void );
    void SetFileHour( int Hour );
    int  GetFileMinute( void );
    void SetFileMinute( int Minute );
    int  GetFileSecond( void );
    void SetFileSecond( int Second );
    // Function memebr of get/set file open status
    bool GetFileOpenFlag( void )                { return m_FileOpenFlag; }
    void SetFileOpenFlag( bool Flag )           { m_FileOpenFlag = Flag; }
    bool GetFileAutoUncompressFlag( void )      { return m_FileAutoUncompressFlag; }
    void SetFileAutoUncompressFlag( bool Flag ) { m_FileAutoUncompressFlag = Flag; }
    int  GetFilePosition( void )                { return m_FilePosition; }
    bool SetFilePosition( int Position );
    // Function member of get/set bmp handle
    int  GetBmpHandle( void )           { return m_BmpHandle; }
    void SetBmpHandle( int Handle )     { m_BmpHandle = Handle; }
    // Function member of check file compress mode
    bool IsCompressedFile( void )       { return (m_FileCompressBlockNumber != 0); }
    // Function memebr of check writable mode
    bool IsWritableFile( void )         { return (m_FileCompressBlockNumber == 0); }
    // Function member of append child node
    void AppendChildNode( CVirtualFileInfoNode *pNode );
    // Function member of insert node ( borther node )
    void InsertNodeToPrevious( CVirtualFileInfoNode *pNode );
    void InsertNodeToNext( CVirtualFileInfoNode *pNode );
    // Function member of hierarchy list-link
    CVirtualFileInfoNode *GetParentNode( void )                 { return m_pParentNode; }
    void SetParentNode( CVirtualFileInfoNode *pNode )           { m_pParentNode = pNode; }
    CVirtualFileInfoNode *GetFirstChildNode( void )             { return m_pFirstChildNode; }
    void SetFirstChildNode( CVirtualFileInfoNode *pNode )       { m_pFirstChildNode = pNode; }
    CVirtualFileInfoNode *GetLastChildNode( void )              { return m_pLastChildNode; }
    void SetLastChildNode( CVirtualFileInfoNode *pNode )        { m_pLastChildNode = pNode; }
    int GetChildNodeNumber( void )                              { return m_ChildNodeNumber; }
    // Funciton member of borther list-link
    CVirtualFileInfoNode *GetPreviousNode( void )       { return m_pPreviousNode; }
    void SetPreviousNode( CVirtualFileInfoNode *pNode ) { m_pPreviousNode = pNode; }
    CVirtualFileInfoNode *GetNextNode( void )           { return m_pNextNode; }
    void SetNextNode( CVirtualFileInfoNode *pNode )     { m_pNextNode = pNode; }
    // Function member of get brother node count
    int GetBrotherNodeCount( void );
    // Function member of get first brother node
    CVirtualFileInfoNode *GetFirstBrotherNode( void );
    // Function member of get brother node
    CVirtualFileInfoNode *GetBrotherNode( int OrderId );
    // Function member of search brother node ( null = not found )
    CVirtualFileInfoNode *SearchBrotherNode( char *pFilename );
  };

 #ifdef __BORLANDC__
  // Back to default alignment
  #pragma option pop
 #else
  // Back to default alignment
  #pragma pack( pop )
 #endif

 // Declare class of virtual directory manager
 class CVirtualDirectoryManager
  {
   public:
    // Data member of pool information
    int m_MaxFileInfoNumber;
    int m_UsedFileInfoNumber;
    // Data member of item number
    int m_DirectoryNumber;
    int m_FileNumber;
    // Data member of file information pool
    CVirtualFileInfoNode *m_pFileInfoPool;
    int m_FreeNodeIndex;
    // Data member of max. number of file compress block
    int m_MaxCompressBlockNumber;
    // Data member of file infomation root node
    CVirtualFileInfoNode *m_pFileInfoRootNode;
    // Data member of sort item table pointer
    _vdmSortItem *m_pSortItemTable;
    // Function member of allocate file info node ( = NULL, error )
    CVirtualFileInfoNode *AllocFileInfoNode( void );
    // Function member of free file info node
    void FreeFileInfoNode( CVirtualFileInfoNode *pNode );
    #ifdef __BORLANDC__
     // Function member of append file
     CVirtualFileInfoNode *AppendFile(
                                      CFileInfoNode        *pFileNode,
                                      CVirtualFileInfoNode *pStoreNode,
                                      bool                 ChildFlag
                                     );
     // Function member of append directory
     CVirtualFileInfoNode *AppendDirectory(
                                           CFileInfoNode        *pDirectoryNode,
                                           CVirtualFileInfoNode *pStoreNode,
                                           bool                  ChildFlag
                                          );
     // Function member of generate directory manager
     bool GenerateDirectoryManager(
                                   CFileInfoNode        *pSourceNode,
                                   CVirtualFileInfoNode *pStoreNode,
                                   bool                 UpdateModeFlag
                                  );
    #endif
    // Function member of sort directory function ( qsort, static function )
    static int SortDirectoryFunction( const void *pItem1, const void *pItem2 );
    // Function member of sort and relink sub directory tree
    bool RelinkSubDirectoryTree( CVirtualFileInfoNode *pParentNode );
    // Function member of sort sub directory tree
    bool SortSubDirectoryTree( CVirtualFileInfoNode *pParentNode );
    // Function member of add base address ( convert all node )
    void AddBaseAddress_FileInfoPool( int NodeNumber, void *Address );
    // Function member of sub base address ( convert all node )
    void SubBaseAddress_FileInfoPool( int NodeNumber, void *Address );
    // Function member of get node number of compact mode
    int GetNodeNumberOfCompactMode( void );
    // Function member of get max number of compress block
    int GetMaxNumberOfCompressBlock( void );
   public:
    // Constructor
    CVirtualDirectoryManager( void );
	// Destructor
	~CVirtualDirectoryManager( void );
    // Function member of import virtual directory manager file
	bool ImportVdmFile( FILE* pFile, int ExpandFileInfoNumber );
    // Function member of export virtual directory manager file
	bool ExportVdmFile( FILE* pFile, bool CompactModeFlag );
    // Function member of startup directory manager
    bool StartupVirtualDirectoryManager( int FileInfoNumber );
    // Function member of shutdown directory manager
    void ShutdownVirtualDirectoryManager( void );
    // Function member of clear directory manager
    void ClearVirtualDirectoryManager( void );
    #ifdef __BORLANDC__
     // Function member of assign virtual directory manager
     bool AssignVirtualDirectoryManager(
                                        CDirectoryManager *pDirectoryManager,
                                        bool              UpdateModeFlag
                                       );
    #endif                                       
    // Function member of sort directory tree
    bool SortDirectoryTree( void );
    // Function member of search file name ( -1 = error )
    int SearchFilename( int ParentPathHandle, const char *pFilename );
    // Function member of pool information
    int  GetFileInfoNumber( void )              { return m_UsedFileInfoNumber; }
    void SetFileInfoNumber( int Number )        { m_UsedFileInfoNumber = Number; }
    // Function member of item number
    int  GetDirectoryNumber( void )             { return m_DirectoryNumber; }
    void SetDirectoryNumber( int Number )       { m_DirectoryNumber = Number; }
    int  GetFileNumber( void )                  { return m_FileNumber; }
    void SetFileNumber( int Number )            { m_FileNumber = Number; }
    // Function member of free node index
    int  GetFreeNodeIndex( void )               { return m_FreeNodeIndex; }
    void SetFreeNodeIndex( int Index )          { m_FreeNodeIndex = Index; }
    // Function member of get max. number of file compress block
    int GetMaxCompressBlockNumber( void )       { return m_MaxCompressBlockNumber; }
    // Function member of get file info pool
    CVirtualFileInfoNode *GetFileInfoPool( void )      { return m_pFileInfoPool; }
    // Function member of get file info node
    CVirtualFileInfoNode *GetFileInfoNode( int NodeIndex );
    // Function member of get node index
    int GetNodeIndex( CVirtualFileInfoNode *pNode );
    // Function member of manage root node
    CVirtualFileInfoNode *CreateFileInfoRootNode( void );
    CVirtualFileInfoNode *GetFileInfoRootNode( void )  { return m_pFileInfoRootNode; }
    void SetFileInfoRootNode( void *pNode )
     { m_pFileInfoRootNode = (CVirtualFileInfoNode *)pNode; }
    // Fucntion member of create root directory
    CVirtualFileInfoNode *CreateRootDirectoryNode( char *pFilename );
    // Function memebr of create directory node ( NULL = error )
    CVirtualFileInfoNode *CreateDirectoryNode( int ParentPathHandle, char *pFilename );
    // Function member of create file node ( NULL = error )
    CVirtualFileInfoNode *CreateFileNode( int ParentPathHandle, char *pFilename );
  };

// End condition compile
#endif

