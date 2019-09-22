//-----------------------------------------------------------------------
#ifndef PACKETREADER_H
#define PACKETREADER_H
//-----------------------------------------------------------------------
#include "BlockFileManager.h"
#include <string>
//#include <memory>

namespace FDO
{

enum enLoadingState
{
	INIT , 
	LOADDONE , 
	FILEVAILD , 
	THREAD 
};

//-----------------------------------------------------------------------
// 記憶體回覆結構
//-----------------------------------------------------------------------
typedef struct memPack        //說明 ***   //Read text from outside file
{
    unsigned char* Data;
    unsigned int   DataSize;
    bool IsValid() const;
} MemPack, *MemPackPtr;

//-----------------------------------------------------------------------
// 封裝檔讀取類別
//-----------------------------------------------------------------------
class PacketReader
{
private:

    // 讀取封裝檔或散檔
    bool ReadPacket;

    // 路徑
    std::string path;

    //std::auto_ptr<CBlockFileManager> MFileManager; // 主封裝檔案管理
    //std::auto_ptr<CBlockFileManager> PFileManager; // PATCH封裝檔案管理

    CBlockFileManager* pMFileManager;   // 主封裝檔案管理
    CBlockFileManager* pPFileManager;   // PATCH封裝檔案管理

    CBlockFileManager* pMFileThreadManager;   // 主封裝檔案管理(Thread用)
    CBlockFileManager* pPFileThreadManager;   // PATCH封裝檔案管理(Thread用)
    CBlockFileManager* pMFileThreadManager2;   // 主封裝檔案管理(Thread用)
    CBlockFileManager* pPFileThreadManager2;   // PATCH封裝檔案管理(Thread用)
    CBlockFileManager* pMFileThreadManager3;   // 主封裝檔案管理(Thread用)
    CBlockFileManager* pPFileThreadManager3;   // PATCH封裝檔案管理(Thread用)


    bool m_bDebug;
    bool Create();
    void Destroy();
    void Debug( const std::string &FileName );

public:
     PacketReader();
    ~PacketReader();

    memPack *LoadFromFile(const std::string &FileName);
    memPack *LoadFromFileEx(const std::string &FileName);
    memPack *LoadFromFileEx1(const std::string &FileName);
    memPack *LoadFromFileEx2(const std::string &FileName);

	memPack *LoadFromBinFile(const std::string &FileName);
    bool IsReadPacket() { return ReadPacket; }
};

//-----------------------------------------------------------------------
// 從封裝檔讀出檔案的函式
//-----------------------------------------------------------------------
memPack *LoadFromPacket(const std::string &FileName);
memPack *LoadFromBinFile(const std::string &FileName);

}

//-----------------------------------------------------------------------
#endif // PACKETREADER_H