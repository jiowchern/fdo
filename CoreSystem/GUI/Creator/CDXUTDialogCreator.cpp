#include "stdafx.h"
#include "CDXUTDialogCreator.h"
#include "LooknFeelReader.h"
#include "ImageSetReader.h"
#include "CDXUTUIBuilder.h"
#include "Element.h"
#include "CDXUTControl.h"
namespace FDO
{	
	const char16* fnCreateStringBuffer(void* buf ,uint nSize)
	{
		char16* str = (char16*)new char8[nSize + sizeof(char16)];
		memcpy(str, buf , nSize);
		str[nSize / sizeof(char16)] = 0;
		return str;
	}
	//////////////////////////////////////////////////////////////////////////
	void fnDestoryStringBuffer(const char16*  str)
	{
		delete str;
	}
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	struct EmptyEvent : public UICreateListener
	{
		virtual void	OnEvent	(int nType,const wchar_t* text) {};
	};
	EmptyEvent gee;
	UICreator* UICreator::gpUICreator = 0;
	//////////////////////////////////////////////////////////////////////////
	UICreator::UICreator(list<CDXUTDialog*>* pDialogList,list<CDXUTDialog*>* pAlwaysOnTop,list<CDXUTDialog*>* mpAlwaysOnBottom,CDXUTDialogResourceManager* pResMgr
			,const UICreatorData& data)
		:	mpDialogList(pDialogList),
			mpAlwaysOnTop(pAlwaysOnTop)		,
			mpAlwaysOnBottom(mpAlwaysOnBottom)	,
			mpResMgr(pResMgr),
			mlfreader(&mLooknFeelSet,&mImageSet),
			mpInitLayoutEvent(data.pInitLayoutEvent),
			mpBuilderCreator(data.pBuilderCreator),
			mpBuilderDestory(data.pBuilderDestory),
			mpControlCreator(data.pControlCreator),
			mbReleaseResource(false)

	{
		gpUICreator = this;
	}
	//////////////////////////////////////////////////////////////////////////
	UICreator::~UICreator()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void UICreator::Initial(const char* looknFeelPath,const char* ImageSetPath)
	{
		//初始化 PropertyFactory
		GUI::PropertyFactory* pPropertyFactory = GUI::PropertyFactory::GetSingletonPtr();
		if(pPropertyFactory == 0)
		{
			pPropertyFactory = new GUI::PropertyFactory;
		}

		memPack *pMem = LoadFromPacket(ImageSetPath);
		if(pMem && pMem->DataSize)
		{			
			FDO::GUIImageSetReader isreader;
			const char16* str = fnCreateStringBuffer(&pMem->Data[2],pMem->DataSize - 2);
			isreader.LoadXML(str,&mImageSet);
			fnDestoryStringBuffer(str);
		}
		else
		{
			FILE *FP;
			if( (FP=fopen(ImageSetPath,"rb")) == 0 )
				return ;//false;

			// 計算檔案大小
			fseek( FP, 0, SEEK_END);
			vector<unsigned char> in( ftell(FP) );
			fseek( FP, 0, SEEK_SET);

			// 讀取資料流中的資料
			fread( &in[0], 1, in.size(), FP );

			// 關閉一個已經開啟的檔案資料流
			fclose( FP );


			FDO::GUIImageSetReader isreader;
			isreader.LoadXML((const wchar_t*)&in[2],&mImageSet);
		}

		pMem = LoadFromPacket(looknFeelPath);
		if(pMem && pMem->DataSize)
		{
			GUILooknFeelReader lfreader(&mImageSet);
			const char16* str = fnCreateStringBuffer(&pMem->Data[2],pMem->DataSize - 2);
			lfreader.LoadXML(str,&mLooknFeelSet);
			fnDestoryStringBuffer(str);			
		}
		else
		{
			FILE *FP;
			if( (FP=fopen(looknFeelPath,"rb")) == 0 )
				return ;//false;

			// 計算檔案大小
			fseek( FP, 0, SEEK_END);
			vector<unsigned char> in( ftell(FP) );
			fseek( FP, 0, SEEK_SET);

			// 讀取資料流中的資料
			fread( &in[0], 1, in.size(), FP );

			// 關閉一個已經開啟的檔案資料流
			fclose( FP );

			GUILooknFeelReader lfreader(&mImageSet);

			lfreader.LoadXML((const wchar_t*)&in[2],&mLooknFeelSet);			
		}
	}
	//////////////////////////////////////////////////////////////////////////

	std::set<std::string> gPaths;
	CDXUTDialog* UICreator::Create(const char* path,CDXUTDialog* pSrc /*= 0*/,UICreateListener* pListener /*= 0*/)
	{
		
		if(pListener == 0)
		{
			pListener = &gee;
		}

		pListener->Send(UICL::DataMessage,L"%s",L"開啟檔案...");

		memPack *pMem = LoadFromPacket(path);
		if(pMem && pMem->DataSize)
		{
			const char16* str = fnCreateStringBuffer(&pMem->Data[2],pMem->DataSize - 2);
			CDXUTDialog* pDia = _Parse(str,pSrc,pListener);
			fnDestoryStringBuffer(str);						
			// 
			{
// 				string str;
// 				StringFormat8(str,"Dialog產生 位置:%x 檔名:%s \n",pDia,path);
// 				OutputDebugStringA(str.c_str());
			}

			if(mbReleaseResource)
				ReleaseResource(pMem);			

			std::set<std::string>::_Pairib it = gPaths.insert(path);
			if(it.second && pDia == 0)
			{
				_LOGA(0,"DIALOG載入失敗 : %s " , path)	;
			}
			if(pDia)
				pDia->SetDebugLayoutPath(ANSItoUNICODE(path));
			return pDia;
		}
		else
		{
			FILE *FP;
			if( (FP=fopen(path,"rb")) == 0 )
			{
				pListener->Send(UICL::DataError,L"%s",L"開啟檔案失敗請檢查路徑是否正確.");
				return 0;//false;
			}

			// 計算檔案大小
			fseek( FP, 0, SEEK_END);
			vector<unsigned char> in( ftell(FP) );
			fseek( FP, 0, SEEK_SET);

			// 讀取資料流中的資料
			pListener->Send(UICL::DataMessage,L"%s",L"讀取串流資訊...");
			fread( &in[0], 1, in.size(), FP );

			// 關閉一個已經開啟的檔案資料流
			fclose( FP );
			CDXUTDialog* pDia = _Parse((const wchar_t*)&in[2],pSrc,pListener);
			{
// 				string str;
// 				StringFormat8(str,"Dialog產生 位置:%x 檔名:%s \n",pDia,path);
// 				OutputDebugStringA(str.c_str());
			}

			std::set<std::string>::_Pairib it = gPaths.insert(path);
			if(it.second && pDia == 0)
			{
				_LOGA(0,"DIALOG載入失敗 : %s " , path)	;
			}

			if(pDia)
				pDia->SetDebugLayoutPath(ANSItoUNICODE(path));
			return pDia;
		}
		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	CDXUTDialog* UICreator::_Parse(const wchar_t* buf,CDXUTDialog* pSrc,UICreateListener* pListener)
	{
		
//		GUILayoutReader& lfreader = mlfreader;
		assert(mpInitLayoutEvent);
		
		GUILayoutReader lfreader(&mLooknFeelSet,&mImageSet);
		mpInitLayoutEvent(&lfreader);
		lfreader.ReadyParse(lfreader.GetDegree(0));

		GUI::Layout::Control* set = 0;
		pListener->Send(UICL::DataMessage,L"%s",L"解析檔案XML...");
		lfreader.LoadXML(buf,set,pListener);

		if(set)
		{			
			CDXUTDialog* pDia = Create(set,pSrc,pListener);
			GUI::Element::fnDestoryElementTree(set);
			return pDia;
		}
		else
		{
			pListener->Send(UICL::DataError,L"%s",L"解析檔案失敗,請檢查xml格式是否正確.");
		}
		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	CDXUTDialog* UICreator::Create(const GUI::Layout::Control* pSrc,CDXUTDialog* pDlgSrc /*= 0*/,UICreateListener* pListener /*= 0*/)
	{
		if(pListener == 0)
		{
			pListener = &gee;
		}

		CDXUTDialog* pDialog = pDlgSrc;
		if(pDialog == 0)
			pDialog = new CDXUTDialog();
		// create dialog		
		
		void* ptr;
		if(pSrc->Get(GUI::Layout::Control::TYPE,ptr))
		{
			const GUI::LooknFeel::Control* pCtrl = (const GUI::LooknFeel::Control*)ptr;
			if(pCtrl == 0)
				return 0;
			int nBase;
			pCtrl->Get(GUI::LooknFeel::Control::BASE,nBase);
			if(nBase != GUI::LooknFeel::Control::Dialog)
			{
				
				//assert(0 && "nBase != GUI::LooknFeel::Control::Dialog");
				pListener->Send(UICL::DataError,L"第一個CONTROL標籤必須是要Dialog Base.\n請查找LooknFeel設定檔.");
				return 0;
			}

			//////////////////////////////////////////////////////////////////////////
			// 預先處理...不能放到CoreSystem裡的元件處理
			int level = 0;
			pSrc->Get(GUI::Layout::Control::LEVEL,level);
			if( level == GUI::Layout::Control::Normal)
			{				
				pDialog->JoinDialogList(mpDialogList);
			}
			else if(level == GUI::Layout::Control::AlwaysOnTop )
			{				
				pDialog->JoinDialogList(mpAlwaysOnTop);
			}
			else if(level == GUI::Layout::Control::AlwaysOnBottom )
			{
				pDialog->JoinDialogList(mpAlwaysOnBottom);				
			}
			else
			{
				pListener->Send(UICL::DataError,L"錯誤的LEVEL標籤.");
				return 0;
			}
			pDialog->Init(mpResMgr);
			pDialog->EnableUseSideFrame(true);
			pListener->Send(UICL::DataMessage,L"Dialog產生成功");
			CDXUTDialog::fnSetResourceManager(mpResMgr);
			//////////////////////////////////////////////////////////////////////////

			CDXUTDialogBuilder db(pDialog,pListener);
			//db.ReceiveStuff(UIBuilder::CLASS_LOOKNFEEL,pCtrl->GetType(),pCtrl);
			fnBroadcastStuff(UIBuilder::CLASS_LOOKNFEEL,&db,pCtrl);
			db.ReceiveStuff(UIBuilder::CLASS_LAYOUT,pSrc->GetType(),pSrc);			
			//_Build(&db,pSrc,pCtrl);

			// create control
			pListener->Send(UICL::DataMessage,L"Begin Create Control...");
			std::list<const GUI::Element*> eles;
			if(pSrc->GetChild(eles))
			{
				std::list<const GUI::Element*>::const_iterator it = eles.begin();
				for( ; it != eles.end();++it)
				{
					const GUI::Layout::Control* pEle = (const GUI::Layout::Control*)*it;				
					if(pEle->GetType() == GUI::Layout::CONTROL)
					{
						_CreateControl(pDialog,pEle,pListener);
					}
					else if(pEle->GetType() == GUI::Layout::CONTROL_DIALOG)						 
					{
						fnBroadcastStuff(UIBuilder::CLASS_LAYOUT,&db,pEle);						
					}
					else
					{
						pListener->Send(UICL::DataWarning,L"無法解析的標籤 TYPE	= %d.",pEle->GetType());
					}
				}
			}
			
		}
		assert(pDialog);
		return pDialog;
	}
	//////////////////////////////////////////////////////////////////////////
	void UICreator::_Build(UIBuilder* pBuilder,const GUI::Layout::Control* pLayoutSrc,const GUI::LooknFeel::Control* pLooknFeelSrc)
	{
		// 設定樣式
		//pBuilder->ReceiveStuff(UIBuilder::CLASS_LOOKNFEEL,pLooknFeelSrc->GetType(),pLooknFeelSrc);
		fnBroadcastStuff(UIBuilder::CLASS_LOOKNFEEL,pBuilder,pLooknFeelSrc);
		
		// 版面配置
		//pBuilder->ReceiveStuff(UIBuilder::CLASS_LAYOUT,pLayoutSrc->GetType(),pLayoutSrc);
		fnBroadcastStuff(UIBuilder::CLASS_LAYOUT,pBuilder,pLayoutSrc);		
		
	}
	//////////////////////////////////////////////////////////////////////////
	void UICreator::_CreateControl(CDXUTDialog* pBuilder,const GUI::Layout::Control* pSrc,UIListener* l)
	{
		void* type;
		if(pSrc->Get(GUI::Layout::Control::TYPE,type) == false)
			return ;

		const GUI::LooknFeel::Control* pCtrl = (const GUI::LooknFeel::Control*)type;
		if(pCtrl)
		{
			int base = 0;
			if(pCtrl->Get(GUI::LooknFeel::Control::BASE,base))
			{				
				assert(mpBuilderCreator && mpControlCreator);
				ControlBuilder* pcb = mpBuilderCreator(base,mpControlCreator(base,pBuilder));				
				assert(pcb);
				if(pcb)
				{					
					_Build(pcb,pSrc,pCtrl);
					pBuilder->AddControl(pcb->GetControl(),pcb->GetControl()->GetID(),false);
					if(l)
						pcb->Dump(l);
					assert(mpBuilderDestory);
					mpBuilderDestory(pcb);
				}
				return ;
			}
		}
		return ;
	}
	//////////////////////////////////////////////////////////////////////////
	
}