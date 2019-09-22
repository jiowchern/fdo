/*
  Filename    : DirectoryManager.H
  Author      : Curtis Lo
  Date        : 2003/05/08
  Descirption : Header file of directory manager
*/
// Avoid recurrence include this file, Start condition compile
#ifndef __DirectoryManager_H
#define __DirectoryManager_H


#ifndef MAXFILE
#define MAXFILE 256
#endif

#ifndef MAXPATH
#define MAXPATH 256
#endif

typedef _finddata_t ffblk;
 // Include standard header files
 //#include <Dir.h>

 // Include private header files
 #include "Callbacks.h"

 // Define symbol of file information type
 enum _FileInfoType { _fitUnknown, _fitDirectory, _fitFile };

 // Define symbol of file info status
 enum _FileInfoStatus { _fisUnknown, _fisSame, _fisDelete, _fisUpdate, _fisNew };

 // Define global structure of sort item
 struct _dmSortItem
  {
   // Declare data member of file info type
   _FileInfoType m_FileInfoType;
   char          m_Filename[ MAXFILE + 1 ];
   void          *m_pNode;
  };

 // Declare class of file infomration node
 class CFileInfoNode
  {
   private:
    // Data member of used flag
    bool m_UsedFlag;
    // Data member of processed flag
    bool m_ProcessedFlag;
    // Data member of file information type
    _FileInfoType m_FileInfoType;
    // Data member of file info status
    _FileInfoStatus m_FileInfoStatus;
    // Data member of file infomation
    ffblk m_FileInfo;
    // Data member of child file update flag
    bool m_ChildFileUpdateFlag;
    // Data member of file mark information
    unsigned long m_SumMark64_H32, m_SumMark64_L32;
    unsigned long m_XorMark;
    unsigned long m_Adler32, m_Crc32;
    // Data member of hierarchy list-link
    CFileInfoNode *m_pParentNode;
    CFileInfoNode *m_pFirstChildNode;
    CFileInfoNode *m_pLastChildNode;
    int m_ChildNodeNumber;
    // Data member of borther list-link
    CFileInfoNode *m_pPreviousNode;
    CFileInfoNode *m_pNextNode;
    // Function member of reverse string and append to buffer 
    void ReverseStringAdd( char *pBuffer, char *pString );
    // Function member of reverse string
    void ReverseString( char *pString );
   public:
    // Constructor
    CFileInfoNode( void ); 
    // Destructor
    ~CFileInfoNode( void );
    // Function member of setup node
    void SetupNode( void );
    // Function member of assign node
    void AssignNode( CFileInfoNode *pNode );
    // Function member of adjust pointer address ( special function for save node )
    void SubBaseAddress( void *pAddress );
    void AddBaseAddress( void *pAddress );
    // Function member of get/set used flag
    bool GetUsedFlag( void )                    { return m_UsedFlag; }
    void SetUsedFlag( bool Flag )               { m_UsedFlag = Flag; }
    // Function member of get/set processed flag
    bool GetProcessedFlag( void )               { return m_ProcessedFlag; }
    void SetProcessedFlag( bool Flag )          { m_ProcessedFlag = Flag; }
    // Function member of file info type
    _FileInfoType GetFileInfoType( void )       { return m_FileInfoType; }
    void SetFileInfoType( _FileInfoType Type )  { m_FileInfoType = Type; }
    // Function member of file info status              
    _FileInfoStatus GetFileInfoStatus( void )           { return m_FileInfoStatus; }
    void SetFileInfoStatus( _FileInfoStatus Status )    { m_FileInfoStatus = Status; }
    // Function member of file information
    ffblk *GetFileInfo( void )           { return &m_FileInfo; }
    void SetFileInfo( ffblk *pFileInfo );
    // Function member of file name
    char *GetFilename( void );
    void SetFilename( char *pFilename );
    // Function member of get file size
	long GetFileSize( void )                    { return m_FileInfo.size; }
    // Function member of get date/time
    int GetFileYear( void );
    int GetFileMonth( void );
    int GetFileDay( void );
    int GetFileHour( void );
    int GetFileMinute( void );
    int GetFileSecond( void );
    // Function member of child file update flag
    bool GetChildFileUpdateFlag( void )         { return m_ChildFileUpdateFlag; }
    void SetChildFileUpdateFlag( bool Flag )    { m_ChildFileUpdateFlag = Flag; }
    // Function member of set ancestors update flag
    void SetAncestorsUpdateFlag( void );
    // Function member of get full filename
    void GetFullFilename( char *pPath );
    // Function member of append child node
    void AppendChildNode( CFileInfoNode *pNode );
    // Function member of insert node ( borther node )
    void InsertNodeToPrevious( CFileInfoNode *pNode );
    void InsertNodeToNext( CFileInfoNode *pNode );
    // Function member of hierarchy list-link
    CFileInfoNode *GetParentNode( void )        { return m_pParentNode; }
    void SetParentNode( CFileInfoNode *pNode )  { m_pParentNode = pNode; }
    CFileInfoNode *GetFirstChildNode( void )            { return m_pFirstChildNode; }
    void SetFirstChildNode( CFileInfoNode *pNode )      { m_pFirstChildNode = pNode; }
    CFileInfoNode *GetLastChildNode( void )             { return m_pLastChildNode; }
    void SetLastChildNode( CFileInfoNode *pNode )       { m_pLastChildNode = pNode; }
    int GetChildNodeNumber( void )              { return m_ChildNodeNumber; }
    // Funciton member of borther list-link
    CFileInfoNode *GetPreviousNode( void )      { return m_pPreviousNode; }
    void SetPreviousNode( CFileInfoNode *pNode ){ m_pPreviousNode = pNode; }
    CFileInfoNode *GetNextNode( void )          { return m_pNextNode; }
    void SetNextNode( CFileInfoNode *pNode )    { m_pNextNode = pNode; }
    // Function member of get brother node count
    int GetBrotherNodeCount( void );
    // Function member of get first brother node
    CFileInfoNode *GetFirstBrotherNode( void );
    // Function member of get brother node
    CFileInfoNode *GetBrotherNode( int OrderId );
    // Function member of search brother node
    CFileInfoNode *SearchBrotherNode( char *pFilename );
    // Function member of record file mark
    bool RecordFileMark( void );
    // Function member of check same file
    bool IsSame( CFileInfoNode *pNode );
  };

 // Declare class of directory manager
 class CDirectoryManager
  {
   public:
    // Data member of pool information
    int m_MaxFileInfoNumber;
    int m_UsedFileInfoNumber;
    // Data member of item number
    int m_DirectoryNumber;
    int m_FileNumber;
    // Data member of file information pool
    CFileInfoNode *m_pFileInfoPool;
    int m_FreeNodeIndex;
    // Data member of file infomation root node
    CFileInfoNode *m_pFileInfoRootNode;
    // Data member of sort item table pointer
    _dmSortItem *m_pSortItemTable;
    // Data member of display callback function
    _DsfcbFunction m_pDisplayFunction;
    // Function member of allocate file info node ( = NULL, error )
    CFileInfoNode *AllocFileInfoNode( void );
    // Function member of free file info node
    void FreeFileInfoNode( CFileInfoNode *pNode );
    // Function member of make sub directory tree
    bool MakeSubDirectoryTree( CFileInfoNode *pParentNode, bool FileMarkFlag );
    // Function member of sort directory function ( qsort, static function )
    static int SortDirectoryFunction( const void *pItem1, const void *pItem2 );
    // Function member of sort and relink sub directory tree
    bool RelinkSubDirectoryTree( CFileInfoNode *pParentNode );
    // Function member of sort sub directory tree
    bool SortSubDirectoryTree( CFileInfoNode *pParentNode );
   public:
    // Constructor
    CDirectoryManager( void );
    // Destructor
    ~CDirectoryManager( void );
    // Function member of startup directory manager
    bool StartupDirectoryManager( int FileInfoNumber );
    // Function member of shutdown directory manager
    void ShutdownDirectoryManager( void );
    // Function member of clear directory manager
    void ClearDirectoryManager( void );
    // Function member of clear processed flag
    void ClearProcessedFlag( void );
    // Function member of make directory tree
    bool MakeDirectoryTree(
                           char           *pPath,
                           bool           FileMarkFlag, 
                           _DsfcbFunction pDisplayFunction
                          );
    // Function member of sort directory tree
    bool SortDirectoryTree( void );
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
    // Function member of get file info pool
    CFileInfoNode *GetFileInfoPool( void )      { return m_pFileInfoPool; }
    // Function member of get root node
    CFileInfoNode *CreateFileInfoRootNode( void );
    CFileInfoNode *GetFileInfoRootNode( void )  { return m_pFileInfoRootNode; }
    void SetFileInfoRootNode( void *pNode )
     { m_pFileInfoRootNode = (CFileInfoNode *)pNode; }
  };

// End condition compile
#endif

