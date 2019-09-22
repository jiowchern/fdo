#include "stdafx.h"
#pragma hdrstop
//---------------------------------------------------------------------------
#include "mEventMacro.h"   

//---------------------------------------------------------------------------
namespace FDO
{
//---------------------------------------------------------------------------
isMap *mEventMacro::sm_pEventMap = NULL;

//---------------------------------------------------------------------------
mEventMacro::mEventMacro(Event_Info_mo *Einfo1,mVariableProcess *VarS1,char** data)
{
   // datalist=new TList();
    //datalist->Clear();

    VarS=VarS1;
    Einfo=Einfo1;
    bFlag=false;

    //������
    EVS_Entry *Ent = NULL;
    EVS_Unit  *UT  = NULL;
    Ent = (EVS_Entry*)*data;
    ID=ToString(Ent->EntryID);

    SendEventMessage2( "<<Add macro>>", ID.c_str() );//�[�J����

    *data += sizeof(EVS_Entry);
    for(int k=0;k<Ent->UnitCount;k++)
    {
        //-----------------------------------------------------------------------
        //�����
        UT = (EVS_Unit*)*data;
        *data += sizeof(EVS_Unit);
        //-----------------------------------------------------------------------
        EVS_Unit_Class *ctp = new EVS_Unit_Class;
        ctp->Type = UT->Type;       //��쫬�O
        ctp->Var  = ToString(UT->TextLenght); //����
        //-----------------------------------------------------------------------
        if( UT->Type == DATATYPE_TEXT )//�� Text
        {
            char* tevData = new char[UT->TextLenght+1];
            memcpy( tevData, *data, UT->TextLenght );
	        ctp->Var = tevData;//����
	        delete []tevData; tevData = 0;
            *data += sizeof(char)*UT->TextLenght;
        }
        //-----------------------------------------------------------------------
        if( (Ent->EntryID==101) && (k==0) )//�аO�n�ƥ��O�U��
        {
            Mark=ctp->Var;
            SendEventMessage2( "<<Add label>>", Mark.c_str() );//�[�J�аO
        }
        //-----------------------------------------------------------------------
        datalist.push_back(ctp);
        //-----------------------------------------------------------------------
    }
}
//---------------------------------------------------------------------------
//�Ѻc
//---------------------------------------------------------------------------
mEventMacro::~mEventMacro(void)
{
    for(size_t i=0;i<datalist.size();i++)
        delete datalist[i];
                                                   
    datalist.clear();
}
//---------------------------------------------------------------------------
bool mEventMacro::Run()
{
	Assert(sm_pEventMap, "No EventMap!");
	int ID_ToInt = atoi(ID.c_str());
    isItor i = sm_pEventMap->find( ID_ToInt );
    if( i == sm_pEventMap->end())
        return false;
    return i->second(this);
}

}