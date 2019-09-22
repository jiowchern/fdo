#ifndef _CDXUTText20091205pm0545_
#define _CDXUTText20091205pm0545_
#include "CDXUTControl.h"
#include "CDXUTTextareaEnvironment.h"

struct	CDXUTTextItemRender;
typedef CDXUTTextItemRender* CDXUTTextItemRenderPtr;
struct CDXUTItemRender : public CDXUTControl
{
	struct ElementQueryer
	{
		ElementQueryer	(CDXUTItemRender* pCtrl);
		~ElementQueryer	();
		enum
		{
			BEGIN_SN = 1
		};
		CDXUTElement*	Query			();
	private:
		uint			mnCount;
		CDXUTItemRender*	mpCtrl;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef CDXUTDialog RednerObject ;
	//////////////////////////////////////////////////////////////////////////
	CDXUTItemRender	( CDXUTDialog *pDialog );
	virtual			~CDXUTItemRender	();

	// 繼承自 CDXUTControl
	virtual HRESULT	OnInit			() ;
	virtual void	Render			(IDirect3DDevice9* pd3dDevice, float fElapsedTime) ;

	void			AddRender		(CDXUTTextItemRenderPtr pRender);
	void			ClearRender		();

private:
	struct RenderInfo
	{
		CDXUTTextItemRenderPtr	pRender;
		RECT					rcClient;
	};
	typedef std::list<RenderInfo>	RenderSet;
	RenderSet								mRenders; // 要繪製的元件
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
struct CDXUTTextItemRender
{
	struct InitialResult
	{
		RECT	rcClient;
	};

	struct RenderInfo
	{
		RenderInfo(const RECT& rcs,const RECT& rcc,CDXUTDialog* pDialog) :
		rcScreen(rcs),rendersys(*pDialog),rcClient(rcc)
		{

		}
		const RECT&						rcScreen;
		const RECT&						rcClient;
		CDXUTItemRender::RednerObject&	rendersys;			
	};
	virtual	InitialResult	Initial	(const RECT& rcCtrl , CDXUTItemRender::ElementQueryer& q) = 0;
	virtual	void			Render	(const RenderInfo& inf) = 0;
};
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// 文字繪製
struct CDXUTTextItemRenderString : public CDXUTTextItemRender
{
							CDXUTTextItemRenderString(CDXUTTextItemEnvironment*	pEnv);
	virtual					~CDXUTTextItemRenderString();
	virtual	InitialResult	Initial	(const RECT& rcCtrl , CDXUTItemRender::ElementQueryer& q) ;
	virtual	void			Render	(const RenderInfo& inf) ;
	void					SetText	(const char16* text);
	void					SetAlign(uint nMode) ;
private:
	std::wstring				mString;
	CDXUTTextItemEnvironment*	mpEnv;
	CDXUTElement*				mpElement;
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

struct CDXUTTextItem : public CDXUTItemRender
{

	CDXUTTextItem	(CDXUTTextItemEnvironment* pEnv , CDXUTDialog *pDialog);
	virtual			~CDXUTTextItem	();

	// 繼承自 CDXUTControl	
	virtual bool	HandleMouse		( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );
	virtual void	OnMouseEnter	() ;
	virtual void	OnMouseLeave	() ;

	void			SetText			(const char16* text);
	void			SetAlign		(uint nMode) ;

private:
	CDXUTTextItemEnvironment*	mpEnv;//共用Env資訊	
	CDXUTTextItemRenderString	mRender;
	GameCommand::CommandHandle	mhClose;
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
struct CDXUTSystemIcon : public CDXUTItemRender
{
					CDXUTSystemIcon	(CDXUTTextItemEnvironment* pEnv , CDXUTDialog *pDialog);
					~CDXUTSystemIcon();
	// 繼承自 CDXUTControl	
	virtual bool	MsgProc			( UINT uMsg, WPARAM wParam, LPARAM lParam ) ;
	void			SetIcon			(uint nIcon);
private:
	CDXUTTextItemEnvironment*	mpEnv;//共用Env資訊	
	

	struct IconRender : public CDXUTTextItemRender
	{
		IconRender();
		~IconRender();
		
		virtual	InitialResult	Initial	(const RECT& rcCtrl , CDXUTItemRender::ElementQueryer& q) ;
		virtual	void			Render	(const RenderInfo& inf) ;
		inline CDXUTElement*	GetElement() {return mpElement;};
	private:
		CDXUTElement*				mpElement;
	};

	IconRender	mRender;
};

#endif