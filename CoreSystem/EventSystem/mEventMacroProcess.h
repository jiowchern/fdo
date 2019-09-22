//-----------------------------------------------------------------------
#ifndef mo_EventMacroProcess_Base_H
#define mo_EventMacroProcess_Base_H
//-----------------------------------------------------------------------
//#include <Classes.hpp>
#include "mEventMacro.h"
//-----------------------------------------------------------------------
#pragma pack(push) //�T�O��ƪ��w��@�w���T
#pragma pack(1)
//-----------------------------------------------------------------------
//EVS���Y��T
struct EVS_Header
{
	char 	EVSID[4];                   //�ɮ׵��O
	int 	Version;                    //�ɮת���
	int 	EventCount;                 //�ƥ��`��
};

//�ƥ��T
struct EVS_Event
{
	int 	Version;
	char 	Name[32];
	int 	EntryCount;                 //�����`��
};
//-----------------------------------------------------------------------
#pragma pack( pop)
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
namespace FDO
{
class mEventMacroProcess : public TPersistent
{
private:
	std::vector<mEventMacro *> macrolist;
    int Index;
    Event_Info_mo Einfo;
public:
	mEventMacroProcess(void);      //�غc
	~mEventMacroProcess(void);     //�Ѻc

    bool Create();

    int ID;     //ID
    int Class;  //Class

    String FileName;
    String VarFileName;
    String MsgFileName;
    mVariableProcess *VarS;

    bool Run();//����
};
}
//-----------------------------------------------------------------------
#endif
