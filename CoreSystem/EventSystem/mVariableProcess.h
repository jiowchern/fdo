//-----------------------------------------------------------------------
#ifndef mo_VariableProcess_H
#define mo_VariableProcess_H
//-----------------------------------------------------------------------
//#include <Classes.hpp>
//-----------------------------------------------------------------------
#include "mVariable.h"
//-----------------------------------------------------------------------
#include "iniReader.h"
#include "binReader.h"
#include "PacketReader.h"
//---------------------------------------------------------------------------
#pragma pack(push) //�T�O��ƪ��w��@�w���T
#pragma pack(1)
//-----------------------------------------------------------------------
//�ܼƩw�q�ɵ��c
struct ARE_Unit_File
{
	//short 	Count;			        //�ܼƫŧi�`��
	char 	varName[50];		        //�ܼƦW��
	BYTE 	varKind;			        //�ܼƫ��A
};

//�ܼƵ��c(���ܼƸs�ϥ�)
struct ARE_Unit
{
	String Name;					    //�ܼƦW��
	String Var;					    	//�ܼƭ�
	mEventMacroProcess_enum varKind;    //�ܼƫ��A
};

//NPC�T�����c	Darkness add
struct sNPCMSG
{
	short	MSG_ID;						//�T��ID
	WCHAR	MSG_sentence[697];			//�T�����e
};
//-----------------------------------------------------------------------
#pragma pack( pop)
//-----------------------------------------------------------------------
//�����ܼư϶�
//-----------------------------------------------------------------------
namespace FDO
{

class mVariableProcess : public TPersistent
{
private:
	vector<sNPCMSG> m_NPCMsgVector;	//MSGBin	Darkness add	
    iniReader *MsgData;//MSG��C
	typedef std::vector<moVariable *> VariableList;
	VariableList GvarList;//�����ܼƦ�C
    VariableList LvarList;//�ϰ��ܼƦ�C
public:
	unsigned short m_iCountry;//��O	Darkness add
	mVariableProcess(String GvarFilename,String LvarFilename,String MsgFilename);//�غc
	~mVariableProcess(void);     //�Ѻc

    void ReSet(String LvarFilename,String MsgFilename);
    moVariable* GetVariable(EVS_Unit_Class *type,EVS_Unit_Class *value);//���o�����ܼ�
    StringList* GetMsg(int id);//���oMSG�T��
	WCHAR* GetMsgUN(int id);//���o�V�n��T��	Darkness add
	bool LoadFromNPCMsgBin( unsigned char* pMemData, unsigned int iDataSize );//�qBINŪ�� Darkness add
};
}
//-----------------------------------------------------------------------
#endif
