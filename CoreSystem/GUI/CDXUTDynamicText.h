#ifndef _CDXUTDynamicText_20080924pm0135_
#define _CDXUTDynamicText_20080924pm0135_
#include "CDXUTgui.h"
#include "TextObject.h"
namespace FDO
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	class CElementHanlder
	{				
		CElement*		mpElement;
		POINT			mptDraw;

	public:
		CElementHanlder		(CElement* pElement ,const POINT& ptBegin );
		~CElementHanlder	();

		void	Draw				(int nBegin,int nEnd,const POINT& ptScreen,CDXUTDialogResourceManager* pManager);

		void	Move				(int nStep);
		void	SetPos				(int x);
		int		GetTailPos			();
		bool	HasDrawTail			(int nBegin);
		bool	HasDraw				(int nEnd);

	};
	typedef std::vector<CElementHanlder*>	CElementHanlderSet;
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	class CDXUTDynamicText :public CDXUTStatic
	{

		typedef std::list<CLineObject>::iterator	CLineObjectIter;
		typedef std::list<CElement>::iterator		CElementIter;
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		CElementHanlderSet	mEleHandlers;
		float				mfPrevStepTime;
		float				mfSecondPerPixle;
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		void				_InitPlay			(CTextObject* pObj);

		CElementHanlder*	_QueryHeadEle		();

		int					_GetBeginPos		();
		int					_GetEndPos			();
		int					_GetStep			();
		void				_CreateEles			(CElementHanlderSet& set,CElementIter& it,int nEleCount);
		void				_DestoryEles		(CElementHanlderSet& set);
	public:
		CDXUTDynamicText	( CDXUTDialog *pDialog );
		~CDXUTDynamicText	();

		virtual bool	HandleMouse			( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );
		virtual void	Render				( IDirect3DDevice9* pd3dDevice, float fElapsedTime );
		void			Play				(const wchar_t* str);

		//設定移動每像素需要多少秒
		void			SetSecondPerPixle	(float fSecond);

	};
}
#endif