//-----------------------------------------------------------------------
#ifndef mo_EventMacro_H
#define mo_EventMacro_H
//-----------------------------------------------------------------------
//#include <Classes.hpp>
//-----------------------------------------------------------------------
#include "mVariableProcess.h"
#include <map>
//-----------------------------------------------------------------------
#pragma pack(push) //�T�O��ƪ��w��@�w���T
#pragma pack(1)
//-----------------------------------------------------------------------
//������T
struct EVS_Entry
{
	int 	EntryID;                    //�����s��
	int 	UnitCount;                  //����`��
};
//����T
struct EVS_Unit
{
	int 	Type;                       //��쫬�O
	int 	TextLenght;                 //���O��TEXT�h��TEXT���׫᪽��Ū��TEXT�A�_�h�������
};
//-----------------------------------------------------------------------
//�����P�ƥ󷾳q�T��
struct Event_Info_mo
{
	bool    IsTalkToNPC ;
    bool    IsEnd;
    String  Mark;
    int     Index;
};
//-----------------------------------------------------------------------
#pragma pack( pop)
//-----------------------------------------------------------------------
namespace FDO
{
class mEventMacro;
typedef bool (*ptr_Function)(mEventMacro* tp);
typedef std::map<int,ptr_Function>     isMap;
typedef isMap::value_type              isValType;
typedef isMap::iterator                isItor;

class mEventMacro : public TPersistent
{
	//<gino>
private:
	static isMap *sm_pEventMap;
public:
	static void RegisterEventMap(isMap *map){ sm_pEventMap = map; }
	//</gino>
public:
	mEventMacro(Event_Info_mo *Einfo,mVariableProcess *VarS,char** data);      //�غc
	~mEventMacro(void);     //�Ѻc

    bool bFlag;

    String ID;
    String Mark;

    String sMark;
    String sMark1;

   // TList *datalist;
	std::vector<EVS_Unit_Class*> datalist;

    mVariableProcess *VarS;
    Event_Info_mo *Einfo;

    bool Run();//����
};
}
//-----------------------------------------------------------------------
#endif
