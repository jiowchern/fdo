#ifndef _CDXUTTextArea_20091205pm0901_
#define _CDXUTTextArea_20091205pm0901_
#include "CDXUTControl.h"
#include "Keyword/KeywordSystem.h"
#include "CDXUTText.h"
#include "CDXUTScrollBar.h"

template<typename T>
struct TScrollBarListener : public ScrollBarEvent::BarVisitor 
{		
	typedef void (T::*TFn)(sint npos);
	TScrollBarListener(T* p,TFn fn ) : mpObj(p) , mpfn(fn)
	{

	}
	virtual void	Visit			(ScrollBarEvent::CurPos& e) 
	{			
		(mpObj->*mpfn)(e.nCurPos);
	}
private:
	T*		mpObj;
	TFn		mpfn;
};


struct CDXUTTextarea : public CDXUTSubDialog , public Keyword::IEnvironmentListener
{
	friend struct TextareaHelper;
					CDXUTTextarea	(CDXUTDialog* pDlg);
					~CDXUTTextarea	();

	virtual HRESULT	OnInit			() ;
	
	void			SetText			(const char16* text);
	virtual	void	OnEnvironment	(const Keyword::IEnvironmentListener::EventData& env) ;

	virtual void	UpdateRects		();

	void			SetStartView	(const POINT& pt) ;

	void			SetScrollBar	(CDXUTScrollBar* pBar);

	
private:

	int				_OnDialogMouseWhell		(const UIEventData& data);
	void			_ScrollChangePosition	(sint nPos);
	TScrollBarListener<CDXUTTextarea>	mScrollBarListener;
	DXUTScrollBarVisitor	mScrollBarVisitor	;

	POINT					mptView;// 可視範圍
	DialogHandle			mhMain;

	typedef std::list<CDXUTTextItemEnvironment*>	EnvironmentSet;	
	EnvironmentSet		mEnvs;

	// 快取
	typedef std::map<const Keyword::Environment* , const CDXUTTextItemEnvironment*> EnvironmentCache;
	EnvironmentCache	mEnvCache;
	
	void			_BuildControls(EnvironmentSet& envs,bool bLimitWidth ,bool bLimitHeight );

	struct BuiuldInfo
	{
		sint iStringHead ;
		sint iStringTail ;
		const char16* pString;
		sint nMaxHeight;
		BuiuldInfo(const char16* str ) : pString(str)
		{
			iStringHead = 0;
			iStringTail = iStringHead - 1;
			nMaxHeight	= 16;
		}

		inline void	Reset	(sint iHead)	{	iStringHead = iHead ; iStringTail = iStringHead - 1	; nMaxHeight	= 16;}
		inline bool Has		()const 	{	return iStringTail > iStringHead ;}
	};
	// build string control
	void			_BuildControl(CDXUTTextItemEnvironment& envs,const BuiuldInfo& bi,const RECT& rcCurElement);

	// build icon control
	void			_BuildControl(CDXUTTextItemEnvironment& envs,uint nIcon,const RECT& rcCurElement);
};
#endif