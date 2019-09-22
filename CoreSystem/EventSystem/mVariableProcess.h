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
#pragma pack(push) //確保資料的定位一定正確
#pragma pack(1)
//-----------------------------------------------------------------------
//變數定義檔結構
struct ARE_Unit_File
{
	//short 	Count;			        //變數宣告總數
	char 	varName[50];		        //變數名稱
	BYTE 	varKind;			        //變數型態
};

//變數結構(給變數群使用)
struct ARE_Unit
{
	String Name;					    //變數名稱
	String Var;					    	//變數值
	mEventMacroProcess_enum varKind;    //變數型態
};

//NPC訊息結構	Darkness add
struct sNPCMSG
{
	short	MSG_ID;						//訊息ID
	WCHAR	MSG_sentence[697];			//訊息內容
};
//-----------------------------------------------------------------------
#pragma pack( pop)
//-----------------------------------------------------------------------
//紀錄變數區塊
//-----------------------------------------------------------------------
namespace FDO
{

class mVariableProcess : public TPersistent
{
private:
	vector<sNPCMSG> m_NPCMsgVector;	//MSGBin	Darkness add	
    iniReader *MsgData;//MSG串列
	typedef std::vector<moVariable *> VariableList;
	VariableList GvarList;//全域變數串列
    VariableList LvarList;//區域變數串列
public:
	unsigned short m_iCountry;//國別	Darkness add
	mVariableProcess(String GvarFilename,String LvarFilename,String MsgFilename);//建構
	~mVariableProcess(void);     //解構

    void ReSet(String LvarFilename,String MsgFilename);
    moVariable* GetVariable(EVS_Unit_Class *type,EVS_Unit_Class *value);//取得相應變數
    StringList* GetMsg(int id);//取得MSG訊息
	WCHAR* GetMsgUN(int id);//取得越南文訊息	Darkness add
	bool LoadFromNPCMsgBin( unsigned char* pMemData, unsigned int iDataSize );//從BIN讀取 Darkness add
};
}
//-----------------------------------------------------------------------
#endif
