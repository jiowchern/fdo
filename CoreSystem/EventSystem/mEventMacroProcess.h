//-----------------------------------------------------------------------
#ifndef mo_EventMacroProcess_Base_H
#define mo_EventMacroProcess_Base_H
//-----------------------------------------------------------------------
//#include <Classes.hpp>
#include "mEventMacro.h"
//-----------------------------------------------------------------------
#pragma pack(push) //確保資料的定位一定正確
#pragma pack(1)
//-----------------------------------------------------------------------
//EVS檔頭資訊
struct EVS_Header
{
	char 	EVSID[4];                   //檔案視別
	int 	Version;                    //檔案版本
	int 	EventCount;                 //事件總數
};

//事件資訊
struct EVS_Event
{
	int 	Version;
	char 	Name[32];
	int 	EntryCount;                 //巨集總數
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
	mEventMacroProcess(void);      //建構
	~mEventMacroProcess(void);     //解構

    bool Create();

    int ID;     //ID
    int Class;  //Class

    String FileName;
    String VarFileName;
    String MsgFileName;
    mVariableProcess *VarS;

    bool Run();//執行
};
}
//-----------------------------------------------------------------------
#endif
