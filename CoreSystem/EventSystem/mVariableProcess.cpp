#include "stdafx.h"
#pragma hdrstop
//---------------------------------------------------------------------------
#include "mVariableProcess.h"
#include "JLSendMessage.h"
//---------------------------------------------------------------------------

//////////////////////								////////////////////////////

//#define RESMGR

#ifdef RESMGR
#pragma message("�޲z�Ҧ��Ұ�") 

#else
#pragma message("�@��Ҧ��Ұ�") 
#endif

////////////////////////////////////////////////////////////////////////////////////

//�غc
namespace FDO
{

mVariableProcess::mVariableProcess(String GvarFilename,String LvarFilename,String MsgFilename)
{        
  //  GvarList=new TList();
   // LvarList=new TList();

    SendEventMessage2( "Start to run", __FUNCTION__ );//�}�l����

    SendEventMessage2( "Global Variable File", GvarFilename.c_str() );//�����ܼ���
	//�������ܼ�
	{
		memPack *tp=LoadFromPacket(GvarFilename.c_str());

#ifdef RESMGR

		//cosiann
		if(tp == 0 || tp->Data == NULL)
		{
			Assert(0 , "Global Variable File Error");
		}

#else

#endif


		char *data= (char*)(tp->Data);
		//���o�ܼƫŧi�`��
		short *Count=(short*)data;
		data+=sizeof(short);
        SendEventMessage2( "Total of global variable", *Count );//�����ܼ��`��

		//-----------------------------------------------------------------------
		for(int i=0;i<*Count;i++)
		{
		    ARE_Unit_File *evtmp=(ARE_Unit_File*)data;
		    data+=sizeof(ARE_Unit_File);
            SendEventMessage4( "Name of variable", evtmp->varName, "Type of variable", evtmp->varKind );//�ܼƦW�� ; �ܼƫ��A
            GvarList.push_back(new moVariable(evtmp->varKind,evtmp->varName));
		}
	}

    SendEventMessage2( "Local variable file", LvarFilename.c_str() );//�ϰ��ܼ���
	//���ϰ��ܼ�
	{
		memPack *tp=LoadFromPacket(LvarFilename.c_str());

#ifdef RESMGR

		//cosiann
		if(tp == 0 || tp->Data == NULL)
		{
			Assert(0 , "Local variable file Error");
		}

#else

#endif

		
		char *data= (char*)(tp->Data);
		//���o�ܼƫŧi�`��
		short *Count=(short*)data;
		data+=sizeof(short);
        SendEventMessage2( "Total of local variable", *Count ); //�ϰ��ܼ��`��

		//-----------------------------------------------------------------------
		for(int i=0;i<*Count;i++)
		{
		    ARE_Unit_File *evtmp=(ARE_Unit_File*)data;
		    data+=sizeof(ARE_Unit_File);
            SendEventMessage4( "Name of variable", evtmp->varName, "Type of variable", evtmp->varKind );//�ܼƦW�� ; �ܼƫ��A
            LvarList.push_back(new moVariable(evtmp->varKind,evtmp->varName));
		}
	}

    SendEventMessage2( "Msg File" , MsgFilename.c_str() );//Msg��
	//��MSG
	memPack *tp=LoadFromPacket(MsgFilename.c_str());

#ifdef RESMGR

	//cosiann
	if(tp->Data == NULL)
	{
		Assert(0 , "Msg File Error");
	}

#else

#endif



	MsgData=new iniReader();
	if( !LoadFromNPCMsgBin(tp->Data,tp->DataSize) )	// Darkness Edit
		MsgData->LoadFromFileInMemory(tp->Data,tp->DataSize);

    SendEventMessage2( "======", "======" );
}
//---------------------------------------------------------------------------
void mVariableProcess::ReSet(String LvarFilename,String MsgFilename)
{         
    for(int i=0;i<(int)LvarList.size();i++)
        delete LvarList[i];
    LvarList.clear();

    delete MsgData;

    SendEventMessage2( "Start to run", __FUNCTION__ );//�}�l����

    SendEventMessage2( "Local variable file", LvarFilename.c_str() );//�ϰ��ܼ���
	//���ϰ��ܼ�
	{
		memPack *tp=LoadFromPacket(LvarFilename.c_str());


#ifdef RESMGR

		//cosiann
		if(tp->Data == NULL)
		{
			Assert(0 , "Local variable file Error");
		}
#else

#endif


		char *data= (char*)(tp->Data);
		//���o�ܼƫŧi�`��
		short *Count=(short*)data;
		data+=sizeof(short);
        SendEventMessage2( "Total of local variable", *Count );//�ϰ��ܼ��`��

		//-----------------------------------------------------------------------
		for(int i=0;i<*Count;i++)
		{
		    ARE_Unit_File *evtmp=(ARE_Unit_File*)data;
		    data+=sizeof(ARE_Unit_File);
            SendEventMessage4( "Name of variable", evtmp->varName, "Type of variable", evtmp->varKind );//�ܼƦW�� ; �ܼƫ��A
            LvarList.push_back(new moVariable(evtmp->varKind,evtmp->varName));
		}
	}

    SendEventMessage2( "Msg File", MsgFilename.c_str() );//Msg��
	//��MSG
	memPack *tp=LoadFromPacket(MsgFilename.c_str());

#ifdef RESMGR
	//cosiann
	if(tp->Data == NULL)
	{
		Assert(0 , "Msg File Error");
	}

#else

#endif


	
	MsgData=new iniReader();
	if( !LoadFromNPCMsgBin(tp->Data,tp->DataSize) )	// Darkness Edit			
		MsgData->LoadFromFileInMemory(tp->Data,tp->DataSize);
                                     
    SendEventMessage2( "======", "======" );
}
//---------------------------------------------------------------------------
//�Ѻc
//---------------------------------------------------------------------------
mVariableProcess::~mVariableProcess(void)
{
    for(int i=0;i<(int)GvarList.size();i++)
        delete GvarList[i];
	GvarList.clear();

	for(int i=0;i<(int)LvarList.size();i++)
        delete LvarList[i];
    delete MsgData;

	m_NPCMsgVector.clear();
}
//---------------------------------------------------------------------------
//���o�����ܼ�
moVariable* mVariableProcess::GetVariable(EVS_Unit_Class *type,EVS_Unit_Class *value)
{
    moVariable *tmp=NULL;
	int itype = ToInt(type->Var);
	int ivalue = ToInt(value->Var);
	if(itype==1)//����
        tmp= GvarList[ivalue];
	else if(itype==2)//�ϰ�
        tmp= LvarList[ivalue];

    return tmp;
}
//---------------------------------------------------------------------------
//���oMSG�T��
StringList* mVariableProcess::GetMsg(int id)
{
    int count=_F(MsgData->GetString("Msg_Count[%d]", id));
    StringList *tmp=new StringList();//�n�ѧO�H�ӧR��
    for(int i=0;i<count;i++)
        tmp->push_back(MsgData->GetString("Msg_Count[%d][%d]",id,i));

    return tmp;
}
//---------------------------------------------------------------------------
//���o�e�r���T��
WCHAR* mVariableProcess::GetMsgUN(int id)
{
	
    for( size_t i=0; i<m_NPCMsgVector.size(); i++ )
	{
		if( m_NPCMsgVector[i].MSG_ID == id )
			return m_NPCMsgVector[i].MSG_sentence; 
	}

    return L"";        
}
//---------------------------------------------------------------------------
//�qBINŪ��
bool mVariableProcess::LoadFromNPCMsgBin( unsigned char* pMemData, unsigned int iDataSize )
{
	m_NPCMsgVector.clear();

	// ���o��O
	//memcpy( &m_iCountry, pMemData, sizeof(short) );
	//if( !m_iCountry == LID_VIETNAMESE )// �V�n
	//{
	//	return false;
	//}
	
	unsigned miDataSize = iDataSize;// - sizeof( short );

    // �ˬd��ƪ��׬O�_���T
    const int iSize = sizeof( sNPCMSG );
    if( miDataSize % iSize )
    {
        //Assert( 0, "Load failed, DataSize error !!" );
        return false;
    }


    // �p���Ƶ���
    unsigned int iDataCount = miDataSize / iSize;

    m_NPCMsgVector.resize( iDataCount );

    BYTE* pData = pMemData;

	// ���o��O
	//pData += sizeof(short);


    // �x�s���
    for( unsigned int i=0; i<iDataCount; ++i )
    {
        memcpy( &m_NPCMsgVector[i], pData, iSize );
        pData += iSize;
    }
     
	return true;
}
//---------------------------------------------------------------------------
}

