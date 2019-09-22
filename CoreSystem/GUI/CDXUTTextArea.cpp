#include "stdafx.h"
#include "CDXUTTextArea.h"
#include <algorithm>
#include "CDXUTDialogCreator.h"
#include "RenderComponent.h"
struct TextareaHelper
{

};
//////////////////////////////////////////////////////////////////////////

CDXUTTextarea::CDXUTTextarea(CDXUTDialog* pDlg) : CDXUTSubDialog(pDlg) , mScrollBarListener(this,&CDXUTTextarea::_ScrollChangePosition)
{
	mptView.x = 0;
	mptView.y = 0;
	m_Type = DXUT_CONTROL_TEXTAREA;
	if(m_pDialog)
	{
		CDXUTDialog* pMain = new CDXUTDialog();
		if(UICreator::gpUICreator)
		{
			//UICreator::gpUICreator->Create("xml\\TextareaBase.xml",pMain,0);
		}		
		pMain->SetListener(-1 , EVENT_DIALOG_MOUSEWHEEL ,this , &CDXUTTextarea::_OnDialogMouseWhell,0 );				
		pMain->JoinDialogList(CDXUTControl::m_pDialog->GetDialogList());		
		CDXUTSubDialog::SetDialog(pMain , false);
		mhMain = pMain->GetHandle();				
		m_pDialog->Raise();
		CDXUTSubDialog::Raise();		
	}
}
//////////////////////////////////////////////////////////////////////////
CDXUTTextarea::~CDXUTTextarea()
{
	_FOREACH(EnvironmentSet , mEnvs , it)
	{
		delete *it;
	}
	mEnvs.clear();
	CDXUTDialog* pMain = CDXUTDialog::Query(mhMain);
	if(pMain)
	{
		pMain->SeparateDialogList();
		delete pMain;
	}
}
//////////////////////////////////////////////////////////////////////////
HRESULT	CDXUTTextarea::OnInit() 
{
	_BuildControls(mEnvs,true,true);

	
	
	return S_OK;
}
//////////////////////////////////////////////////////////////////////////
void CDXUTTextarea::SetStartView(const POINT& pt) 
{	
	if(mptView.x != pt.x || mptView.y != pt.y)
	{
		//mScrollBarVisitor.AcceptScroll(mptView.y - pt.y);
		mptView = pt;
		_BuildControls(mEnvs,true,true);
		
	}
}
//////////////////////////////////////////////////////////////////////////
void CDXUTTextarea::SetScrollBar(CDXUTScrollBar* pBar)
{
	mScrollBarVisitor.Register(pBar);
	pBar->AddVisitor(&mScrollBarListener);
}
//////////////////////////////////////////////////////////////////////////
int CDXUTTextarea::_OnDialogMouseWhell(const UIEventData& data)
{
	mScrollBarVisitor.AcceptScroll(-data.nEventData);
	return 0;
}
//////////////////////////////////////////////////////////////////////////
void CDXUTTextarea::_ScrollChangePosition(sint nPos)
{
	POINT pt(mptView);
	pt.y = nPos;
	SetStartView(pt);
}
//////////////////////////////////////////////////////////////////////////
void CDXUTTextarea::UpdateRects()
{
	SIZE szPre = {m_width , m_height};
	CDXUTControl::UpdateRects();

	POINT ptParent;
	m_pDialog->GetLocation(ptParent);

	RECT rcdlg(m_rcBoundingBox);
	OffsetRect(&rcdlg ,ptParent.x , ptParent.y);
	CDXUTDialog* pMain = CDXUTDialog::Query(mhMain);
	if(pMain)
	{
		pMain->SetLocation(rcdlg.left,rcdlg.top);
		pMain->SetSize(RectWidth(rcdlg) , RectHeight(rcdlg));

		if(szPre.cx != RectWidth(rcdlg)  || szPre.cy != RectHeight(rcdlg))
			_BuildControls(mEnvs,true,true);		
	}
	
}
//////////////////////////////////////////////////////////////////////////
void CDXUTTextarea::SetText(const char16* text)
{	
	mEnvCache.clear();
	_FOREACH(EnvironmentSet , mEnvs , it)
	{
		delete *it;
	}
	mEnvs.clear();
	Keyword::Run(text , this);

	_BuildControls(mEnvs,true,true);
}
//////////////////////////////////////////////////////////////////////////
void CDXUTTextarea::_BuildControl(CDXUTTextItemEnvironment& envs,const BuiuldInfo& bi,const RECT& rcCurElement)
{
	CDXUTDialog* pMain = CDXUTDialog::Query(mhMain);
	if(pMain)
	{
		RECT rcView = {mptView.x,mptView.y,mptView.x + pMain->GetWidth() , mptView.y + pMain->GetHeight()};		

		RECT rc(rcCurElement);
		bool bInScreen = IntersectRect(&rc,&rcCurElement,&rcView) != FALSE;
		if(bInScreen)
		{
			CDXUTTextItem* pTextItem = new CDXUTTextItem(&envs,pMain);

			std::wstring str;
			str.assign(&bi.pString[bi.iStringHead] , &bi.pString[bi.iStringTail+1]);
			pTextItem->SetText(str.c_str());

			
			OffsetRect(&rc,-mptView.x,-mptView.y);
			pMain->AddControl(pTextItem , -1 , rc)	;	

			if(rcCurElement.top < rcView.top)
				pTextItem->SetAlign(DT_LEFT | DT_BOTTOM);
			else
				pTextItem->SetAlign(DT_LEFT | DT_TOP);
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void CDXUTTextarea::_BuildControl(CDXUTTextItemEnvironment& envs,uint nIcon,const RECT& rcCurElement)
{

	CDXUTDialog* pMain = CDXUTDialog::Query(mhMain);
	if(pMain)
	{
		
		RECT rc(rcCurElement);
		RECT rcView = {mptView.x,mptView.y,mptView.x + pMain->GetWidth() , mptView.y + pMain->GetHeight()};		
		bool bInScreen = IntersectRect(&rc,&rcCurElement,&rcView) != FALSE;
		if(bInScreen)
		{
			CDXUTSystemIcon* pCtrl = new CDXUTSystemIcon(&envs,pMain);

			
			OffsetRect(&rc,-mptView.x,-mptView.y);

			pMain->AddControl(pCtrl , -1 , rc)	;	

			pCtrl->SetIcon(nIcon);

			
		}
		
	}
}
//////////////////////////////////////////////////////////////////////////
void CDXUTTextarea::_BuildControls(EnvironmentSet& envs,bool bLimitWidth ,bool bLimitHeight)
{
	
	CDXUTDialog* pMain = CDXUTDialog::Query(mhMain);
	if(pMain)
	{

		pMain->RemoveAllControls(); // 移除全部元件
		//pMain->ReleaseAllListener();
		SIZE szMain = {pMain->GetWidth() , pMain->GetHeight()};
		POINT ptCur = {0,0}; // 目前的位置
		sint nLastLineHeight = 16;
		_FOREACH(EnvironmentSet , envs , it)
		{
			CDXUTTextItemEnvironment& env = *(*it);
			if(env.bReturn)
			{
				ptCur.x = 0;
				ptCur.y = nLastLineHeight;
				nLastLineHeight += 16;
			}
// 
// 			if(ptCur.y > szMain.cy)
// 				return  ; // 結束建立

			

			// 建立sys
			if(env.nSysIcon != -1)
			{
				RECT rcIcon;
				CalcIconSrcRect(env.nSysIcon,rcIcon);

				if( RectWidth(rcIcon) > szMain.cx - ptCur.x && bLimitWidth)
				{
					// 過寬到下一行
					ptCur.x = 0;								
					ptCur.y += RectHeight(rcIcon); // 更新下一行位置	
					
// 					if(ptCur.y > szMain.cy)
// 						return  ; // 結束建立				
				}

				RECT rcClient = {ptCur.x,ptCur.y,ptCur.x + RectWidth(rcIcon),ptCur.y + RectHeight(rcIcon)};
				
				_BuildControl(env,env.nSysIcon,rcClient);
				ptCur.x += RectWidth(rcIcon);
				
			}
			
			
			// 字串建立部分 ...
			if(env.baseString.size())
			{
				SIZE szCurElement = {0,0};
				
				BuiuldInfo bi(env.baseString.c_str());
				// 計算字的寬高
				for (sint i = 0 ; i < (sint)env.baseString.length() ; ++i)
				{
					CDXUTDialogResourceManager* pMgr = pMain->GetManager();
					SIZE szChar = pMgr->GetChar16Size( env.iFont , env.baseString[i]);
					
					if(szCurElement.cx + szChar.cx > szMain.cx - ptCur.x && bLimitWidth)
					{
						// 過寬到下一行

						
						if(bi.Has())
						{
							// 先建立之前的
							RECT rcCurElement = { ptCur.x 
								, ptCur.y 
								, szCurElement.cx + ptCur.x //+ szChar.cx
								, bi.nMaxHeight + ptCur.y};
							_BuildControl(env,bi,rcCurElement);

							bi.Reset(bi.iStringTail + 1);

							szCurElement.cx =  szChar.cx;
						}
						else
						{
							szCurElement.cx += szChar.cx;
						}
						
						ptCur.x = 0;						
						ptCur.y = nLastLineHeight; // 更新下一行位置										
						nLastLineHeight += 16;		// 預設下一航大小
					}
					else
					{
						szCurElement.cx += szChar.cx;
						if(szCurElement.cy < szChar.cy)
						{
							szCurElement.cy = szChar.cy;
							if(ptCur.y + szCurElement.cy > nLastLineHeight)
								nLastLineHeight = ptCur.y + szCurElement.cy;
						}
					}	
					if(szChar.cy > bi.nMaxHeight)
					{
						bi.nMaxHeight = szChar.cy;
					}
					
					bi.iStringTail ++ ;
				}
				if(szCurElement.cx != 0)
				{
					
					bi.iStringTail ++ ;
					RECT rcCurElement = { ptCur.x 
						, ptCur.y 
						, szCurElement.cx + ptCur.x 
						, bi.nMaxHeight + ptCur.y};

					_BuildControl(env,bi,rcCurElement);

					bi.Reset(0);

					ptCur.x += szCurElement.cx;

				}
			}
			
		}

		// 更新scrollbar 
		CDXUTDialog* pDlg = CDXUTDialog::Query(mhMain);
		if(pDlg)
		{
			mScrollBarVisitor.AcceptMaxSize(nLastLineHeight ,pDlg->GetHeight());			
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void CDXUTTextarea::OnEnvironment(const Keyword::IEnvironmentListener::EventData& eventdata) 
{
	CDXUTDialogResourceManager* pMgr = CDXUTDialog::GetManager();
	const Keyword::EnvironmentNode* pEnvNode = eventdata.env.pEnv;
	
	mEnvs.push_back(new CDXUTTextItemEnvironment());
	CDXUTTextItemEnvironment& tmpenv = *mEnvs.back();		

	std::list<const Keyword::EnvironmentNode*> nodes;
	nodes.push_front(pEnvNode);
	while(pEnvNode = pEnvNode->GetParent())
	{
		nodes.push_front(pEnvNode);
	}
	_FOREACH(std::list<const Keyword::EnvironmentNode*> , nodes , itNode)
	{
		const Keyword::Environment* pEnv = (*itNode)->GetObjectPoint();
		EnvironmentCache::const_iterator it = mEnvCache.find(pEnv);
		if(it != mEnvCache.end())
		{
			const CDXUTTextItemEnvironment* pEnvironment = it->second;
			tmpenv = *pEnvironment;
			continue;
		}
		
		char16* strname;
		if(pEnv->Get(Keyword::Environment::NAME ,strname))
		{
			std::wstring name(strname);
			std::transform(name.begin() , name.end() , name.begin() ,towupper);

			if (name == L"MAPFONT")
			{
				tmpenv.nFontColor = 0xFF538ed5;				
			}
			else if (name == L"MONFONT")
			{
				tmpenv.nFontColor = 0xFFd33535;				
			}
			else if (name == L"ITEMFONT")
			{
				tmpenv.nFontColor = 0xFF73d351;				
			}
			else if (name == L"NPCFONT")
			{
				tmpenv.nFontColor = 0xFFffff66;				
			}
			else if (name == L"HINT")
			{
				GameCommand::ParamSet params;
				params.resize(2);

				int tmp = 0;
				pEnv->Get(Keyword::Environment::HINTID,tmp);
				tmpenv.nHintId = tmp;
				params[0] = tmp;

				params[1] = true; //開
				tmpenv.SetCommandMouseIn	(name.c_str() , params);	
				params[1] = false;//關
				tmpenv.SetCommandMouseOut	(name.c_str() , params);	
			}
			else if (name == L"MONHINT")
			{
				GameCommand::ParamSet params;
				params.resize(2);

				int tmp = 0;
				pEnv->Get(Keyword::Environment::MONID,tmp);
				tmpenv.nHintId = tmp;
				params[0] = tmp;

				params[1] = true; //開
				tmpenv.SetCommandMouseIn	(name.c_str() , params);	
				params[1] = false;//關
				tmpenv.SetCommandMouseOut	(name.c_str() , params);
			}
			else if (name == L"ITEMHINT")
			{
				GameCommand::ParamSet params;
				params.resize(2);

				int tmp = 0;
				pEnv->Get(Keyword::Environment::ITEMID,tmp);
				tmpenv.nHintId = tmp;
				params[0] = tmp;

				params[1] = true; //開
				tmpenv.SetCommandMouseIn	(name.c_str() , params);	
				params[1] = false;//關
				tmpenv.SetCommandMouseOut	(name.c_str() , params);	
			}
			else if (name == L"MAP")
			{	
				GameCommand::ParamSet params;
				params.resize(3);

				int tmp = 0;
				pEnv->Get(Keyword::Environment::MAPID,tmp);
				params[0] = tmp;

				tmp = 0;
				pEnv->Get(Keyword::Environment::X,tmp);
				params[1] = tmp;

				tmp = 0;
				pEnv->Get(Keyword::Environment::Y,tmp);
				params[2] = tmp;

				tmpenv.SetCommandMouseLDown(name.c_str() , params);	

				
			}
			else if (name == L"NPC")
			{	
				GameCommand::ParamSet params;
				params.resize(2);

				int tmp = 0;
				pEnv->Get(Keyword::Environment::NPCID,tmp);
				params[0] = tmp;				

				tmp = 0;
				pEnv->Get(Keyword::Environment::MAPID,tmp);
				params[1] = tmp;				

				tmpenv.SetCommandMouseLDown(name.c_str() , params);			

				
			}
			else if (name == L"S")
			{
				tmpenv.baseString += L' ';
			}
			else if (name == L"R")
			{
				tmpenv.bReturn = true;
			}
			if (name == L"SYSICON")
			{
				int nId = 0;
				if(pEnv->Get(Keyword::Environment::ID,nId))
				{
					tmpenv.nSysIcon = nId;
				}
			}
			else if (name == L"C")
			{			
				if(pEnv->Get(Keyword::Environment::FONT_COLOR,tmpenv.nFontColor))
				{

				}
			}
			else if (name == L"FONT")
			{
				// 處理字型
				{
					struct FontStyle
					{
						char16* fontname;
						sint	nHeight;
						sint	nWeight;
					};
					FontStyle fontstyle;

					pEnv->Get(Keyword::Environment::FONT_HEIGHT,fontstyle.nHeight);
					pEnv->Get(Keyword::Environment::FONT_WEIGHT,fontstyle.nWeight);		
					if(pEnv->Get(Keyword::Environment::FONT_NAME,fontstyle.fontname))
					{
						tmpenv.iFont = pMgr->AddFont(fontstyle.fontname,fontstyle.nHeight,fontstyle.nWeight,0);
					}		
				}
			}
	
			
		}
	}
	//////////////////////////////////////////////////////////////////////////
	// 建立 元件
	if(nodes.size())
	{
		struct FilterCharFunctor
		{
			bool operator()(char16 c)
			{
				return !(c != L'\r' && c != L'\n'&& c != L'\t'/*&& c != L' '*/);
			}
		};
		tmpenv.baseString += eventdata.env.text;
		tmpenv.baseString.erase(
			remove_if(tmpenv.baseString.begin(),tmpenv.baseString.end(), FilterCharFunctor()), 
			tmpenv.baseString.end());		
		mEnvCache.insert( make_pair(nodes.back()->GetObjectPoint() , mEnvs.back()) );		
	}
	
	
	
	
}

