//-----------------------------------------------------------------------
#ifndef mo_EventMacro_H
#define mo_EventMacro_H
//-----------------------------------------------------------------------
//#include <Classes.hpp>
//-----------------------------------------------------------------------
#include "mVariableProcess.h"
#include <map>
//-----------------------------------------------------------------------
#pragma pack(push) //確保資料的定位一定正確
#pragma pack(1)
//-----------------------------------------------------------------------
//巨集資訊
struct EVS_Entry
{
	int 	EntryID;                    //巨集編號
	int 	UnitCount;                  //欄位總數
};
//欄位資訊
struct EVS_Unit
{
	int 	Type;                       //欄位型別
	int 	TextLenght;                 //型別為TEXT則為TEXT長度後直接讀取TEXT，否則為欄位資料
};
//-----------------------------------------------------------------------
//巨集與事件溝通訊息
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
	mEventMacro(Event_Info_mo *Einfo,mVariableProcess *VarS,char** data);      //建構
	~mEventMacro(void);     //解構

    bool bFlag;

    String ID;
    String Mark;

    String sMark;
    String sMark1;

   // TList *datalist;
	std::vector<EVS_Unit_Class*> datalist;

    mVariableProcess *VarS;
    Event_Info_mo *Einfo;

    bool Run();//執行
};
}
//-----------------------------------------------------------------------
#endif
