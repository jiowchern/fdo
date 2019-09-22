#include "stdafx.h"
#pragma hdrstop
//---------------------------------------------------------------------------
#include "mVariableProcess.h"
#include "JLSendMessage.h"
//---------------------------------------------------------------------------

//////////////////////								////////////////////////////

//#define RESMGR

#ifdef RESMGR
#pragma message("管理模式啟動") 

#else
#pragma message("一般模式啟動") 
#endif

////////////////////////////////////////////////////////////////////////////////////

//建構
namespace FDO
{

mVariableProcess::mVariableProcess(String GvarFilename,String LvarFilename,String MsgFilename)
{        
  //  GvarList=new TList();
   // LvarList=new TList();

    SendEventMessage2( "Start to run", __FUNCTION__ );//開始執行

    SendEventMessage2( "Global Variable File", GvarFilename.c_str() );//全域變數檔
	//取全域變數
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
		//取得變數宣告總數
		short *Count=(short*)data;
		data+=sizeof(short);
        SendEventMessage2( "Total of global variable", *Count );//全域變數總數

		//-----------------------------------------------------------------------
		for(int i=0;i<*Count;i++)
		{
		    ARE_Unit_File *evtmp=(ARE_Unit_File*)data;
		    data+=sizeof(ARE_Unit_File);
            SendEventMessage4( "Name of variable", evtmp->varName, "Type of variable", evtmp->varKind );//變數名稱 ; 變數型態
            GvarList.push_back(new moVariable(evtmp->varKind,evtmp->varName));
		}
	}

    SendEventMessage2( "Local variable file", LvarFilename.c_str() );//區域變數檔
	//取區域變數
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
		//取得變數宣告總數
		short *Count=(short*)data;
		data+=sizeof(short);
        SendEventMessage2( "Total of local variable", *Count ); //區域變數總數

		//-----------------------------------------------------------------------
		for(int i=0;i<*Count;i++)
		{
		    ARE_Unit_File *evtmp=(ARE_Unit_File*)data;
		    data+=sizeof(ARE_Unit_File);
            SendEventMessage4( "Name of variable", evtmp->varName, "Type of variable", evtmp->varKind );//變數名稱 ; 變數型態
            LvarList.push_back(new moVariable(evtmp->varKind,evtmp->varName));
		}
	}

    SendEventMessage2( "Msg File" , MsgFilename.c_str() );//Msg檔
	//取MSG
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

    SendEventMessage2( "Start to run", __FUNCTION__ );//開始執行

    SendEventMessage2( "Local variable file", LvarFilename.c_str() );//區域變數檔
	//取區域變數
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
		//取得變數宣告總數
		short *Count=(short*)data;
		data+=sizeof(short);
        SendEventMessage2( "Total of local variable", *Count );//區域變數總數

		//-----------------------------------------------------------------------
		for(int i=0;i<*Count;i++)
		{
		    ARE_Unit_File *evtmp=(ARE_Unit_File*)data;
		    data+=sizeof(ARE_Unit_File);
            SendEventMessage4( "Name of variable", evtmp->varName, "Type of variable", evtmp->varKind );//變數名稱 ; 變數型態
            LvarList.push_back(new moVariable(evtmp->varKind,evtmp->varName));
		}
	}

    SendEventMessage2( "Msg File", MsgFilename.c_str() );//Msg檔
	//取MSG
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
//解構
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
//取得相應變數
moVariable* mVariableProcess::GetVariable(EVS_Unit_Class *type,EVS_Unit_Class *value)
{
    moVariable *tmp=NULL;
	int itype = ToInt(type->Var);
	int ivalue = ToInt(value->Var);
	if(itype==1)//全域
        tmp= GvarList[ivalue];
	else if(itype==2)//區域
        tmp= LvarList[ivalue];

    return tmp;
}
//---------------------------------------------------------------------------
//取得MSG訊息
StringList* mVariableProcess::GetMsg(int id)
{
    int count=_F(MsgData->GetString("Msg_Count[%d]", id));
    StringList *tmp=new StringList();//要由別人來刪除
    for(int i=0;i<count;i++)
        tmp->push_back(MsgData->GetString("Msg_Count[%d][%d]",id,i));

    return tmp;
}
//---------------------------------------------------------------------------
//取得寬字元訊息
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
//從BIN讀取
bool mVariableProcess::LoadFromNPCMsgBin( unsigned char* pMemData, unsigned int iDataSize )
{
	m_NPCMsgVector.clear();

	// 取得國別
	//memcpy( &m_iCountry, pMemData, sizeof(short) );
	//if( !m_iCountry == LID_VIETNAMESE )// 越南
	//{
	//	return false;
	//}
	
	unsigned miDataSize = iDataSize;// - sizeof( short );

    // 檢查資料長度是否正確
    const int iSize = sizeof( sNPCMSG );
    if( miDataSize % iSize )
    {
        //Assert( 0, "Load failed, DataSize error !!" );
        return false;
    }


    // 計算資料筆數
    unsigned int iDataCount = miDataSize / iSize;

    m_NPCMsgVector.resize( iDataCount );

    BYTE* pData = pMemData;

	// 取得國別
	//pData += sizeof(short);


    // 儲存資料
    for( unsigned int i=0; i<iDataCount; ++i )
    {
        memcpy( &m_NPCMsgVector[i], pData, iSize );
        pData += iSize;
    }
     
	return true;
}
//---------------------------------------------------------------------------
}

