#include "stdafx.h"
#pragma hdrstop
//---------------------------------------------------------------------------
#include "mEventMacroProcess.h" 
#include "PacketReader.h"
#include "JLSendMessage.h"
#include <cassert>


//////////////////////								////////////////////////////

#ifdef RESMGR
#pragma message("�޲z�Ҧ��Ұ�") 

#else
#pragma message("�@��Ҧ��Ұ�") 
#endif

////////////////////////////////////////////////////////////////////////////////////


namespace FDO
{
//---------------------------------------------------------------------------
//�غc
//---------------------------------------------------------------------------
mEventMacroProcess::mEventMacroProcess(void)
{      
    Index=0;
	Einfo.IsTalkToNPC = false ;
    Einfo.IsEnd=false;
    Einfo.Index=-1;    
}
//---------------------------------------------------------------------------
//�Ѻc
//---------------------------------------------------------------------------
mEventMacroProcess::~mEventMacroProcess(void)
{      
    for(int i=0;i<(int)macrolist.size();i++)
        delete macrolist[i];
}
//---------------------------------------------------------------------------
//�_�l
//---------------------------------------------------------------------------
bool mEventMacroProcess::Create()
{
    SendEventMessage2( "Build Macro", __FUNCTION__ );//�إߥ���

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
    //���ƥ�
    EVS_Header *Header=(EVS_Header*)data;
    data+=sizeof(EVS_Header);

    //������
    EVS_Event *Evn  =(EVS_Event*)data;   
    data+=sizeof(EVS_Event);
    SendEventMessage2( "Event Name", Evn->Name ); //�ƥ�W��

    //�[�J����
	assert(macrolist.empty());
   // macrolist.clear();

    SendEventMessage2( "Total of Macro", Evn->EntryCount );//�����`��

    for(int i=0;i<Evn->EntryCount;i++)
        macrolist.push_back(new mEventMacro(&Einfo,VarS,&data));

	return true;
}
//---------------------------------------------------------------------------
//����
//---------------------------------------------------------------------------
bool mEventMacroProcess::Run()
{
    //�j������
    if(Einfo.IsEnd)
        return false;
    //���D
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
    //return false ���,���ƥ�w�]��̫�@�ӥ���,�ӥB�ӥ���return false(����槹��)
    if(Index >= (int)macrolist.size())
        return false;

    if(!(macrolist[Index])->Run())
        Index++;

	return true;
}
//---------------------------------------------------------------------------
}
