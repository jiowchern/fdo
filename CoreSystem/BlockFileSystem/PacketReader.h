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
// �O����^�е��c
//-----------------------------------------------------------------------
typedef struct memPack        //���� ***   //Read text from outside file
{
    unsigned char* Data;
    unsigned int   DataSize;
    bool IsValid() const;
} MemPack, *MemPackPtr;

//-----------------------------------------------------------------------
// �ʸ���Ū�����O
//-----------------------------------------------------------------------
class PacketReader
{
private:

    // Ū���ʸ��ɩδ���
    bool ReadPacket;

    // ���|
    std::string path;

    //std::auto_ptr<CBlockFileManager> MFileManager; // �D�ʸ��ɮ׺޲z
    //std::auto_ptr<CBlockFileManager> PFileManager; // PATCH�ʸ��ɮ׺޲z

    CBlockFileManager* pMFileManager;   // �D�ʸ��ɮ׺޲z
    CBlockFileManager* pPFileManager;   // PATCH�ʸ��ɮ׺޲z

    CBlockFileManager* pMFileThreadManager;   // �D�ʸ��ɮ׺޲z(Thread��)
    CBlockFileManager* pPFileThreadManager;   // PATCH�ʸ��ɮ׺޲z(Thread��)
    CBlockFileManager* pMFileThreadManager2;   // �D�ʸ��ɮ׺޲z(Thread��)
    CBlockFileManager* pPFileThreadManager2;   // PATCH�ʸ��ɮ׺޲z(Thread��)
    CBlockFileManager* pMFileThreadManager3;   // �D�ʸ��ɮ׺޲z(Thread��)
    CBlockFileManager* pPFileThreadManager3;   // PATCH�ʸ��ɮ׺޲z(Thread��)


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
// �q�ʸ���Ū�X�ɮת��禡
//-----------------------------------------------------------------------
memPack *LoadFromPacket(const std::string &FileName);
memPack *LoadFromBinFile(const std::string &FileName);

}

//-----------------------------------------------------------------------
#endif // PACKETREADER_H