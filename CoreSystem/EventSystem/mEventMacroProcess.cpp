#include "stdafx.h"
#pragma hdrstop
//---------------------------------------------------------------------------
#include "mEventMacroProcess.h" 
#include "PacketReader.h"
#include "JLSendMessage.h"
#include <cassert>


//////////////////////								////////////////////////////

#ifdef RESMGR
#pragma message("管理模式啟動") 

#else
#pragma message("一般模式啟動") 
#endif

////////////////////////////////////////////////////////////////////////////////////


namespace FDO
{
//---------------------------------------------------------------------------
//建構
//---------------------------------------------------------------------------
mEventMacroProcess::mEventMacroProcess(void)
{      
    Index=0;
	Einfo.IsTalkToNPC = false ;
    Einfo.IsEnd=false;
    Einfo.Index=-1;    
}
//---------------------------------------------------------------------------
//解構
//---------------------------------------------------------------------------
mEventMacroProcess::~mEventMacroProcess(void)
{      
    for(int i=0;i<(int)macrolist.size();i++)
        delete macrolist[i];
}
//---------------------------------------------------------------------------
//起始
//---------------------------------------------------------------------------
bool mEventMacroProcess::Create()
{
    SendEventMessage2( "Build Macro", __FUNCTION__ );//建立巨集

    memPack *tp=LoadFromPacket(FileName.c_str());

#ifdef RESMGR

	//cosiann
	if(tp->Data == NULL)
	{
        return false;
	}

#else
    if(!tp)
        return false;

#endif


    char *data=(char*)tp->Data;
    //取事件
    EVS_Header *Header=(EVS_Header*)data;
    data+=sizeof(EVS_Header);

    //取巨集
    EVS_Event *Evn  =(EVS_Event*)data;   
    data+=sizeof(EVS_Event);
    SendEventMessage2( "Event Name", Evn->Name ); //事件名稱

    //加入巨集
	assert(macrolist.empty());
   // macrolist.clear();

    SendEventMessage2( "Total of Macro", Evn->EntryCount );//巨集總數

    for(int i=0;i<Evn->EntryCount;i++)
        macrolist.push_back(new mEventMacro(&Einfo,VarS,&data));

	return true;
}
//---------------------------------------------------------------------------
//執行
//---------------------------------------------------------------------------
bool mEventMacroProcess::Run()
{
    //強迫結束
    if(Einfo.IsEnd)
        return false;
    //跳躍
    if(Einfo.Index == -2)
    {
        for(int i=0;i<(int)macrolist.size();i++)
        {
            if(macrolist[i]->Mark==Einfo.Mark)
            {
                Einfo.Index=i;
                Index=i;
                break;
            }
        }
        Einfo.Index = -1;
    }
    //return false 表示,本事件已跑到最後一個巨集,而且該巨集return false(表執行完畢)
    if(Index >= (int)macrolist.size())
        return false;

    if(!(macrolist[Index])->Run())
        Index++;

	return true;
}
//---------------------------------------------------------------------------
}
