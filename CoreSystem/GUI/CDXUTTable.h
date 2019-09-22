#ifndef _CDXUTTable_20090519pm0438_
#define _CDXUTTable_20090519pm0438_
#include "Tree.h"
#include "CDXUTControl.h"
namespace FDO
{
	

	
	
	
	
// 	struct CDXUTSplitter	
// 	{
// 		CDXUTSplitter	(CDXUTControl* pParent,CDXUTControl* pC1,CDXUTControl* pC2);
// 		~CDXUTSplitter	();
// 	};	
	struct CDXUTPanel : public CDXUTControl
	{
						CDXUTPanel	(CDXUTDialog *pDialog) ;
		virtual			~CDXUTPanel	();		
		bool			AddControl	(CDXUTControl* pCtrl , uint nAlignMode,bool bResize);		
#ifdef _DEBUG		
		virtual void Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime );
#endif
		void			UpdatePanel	();
		bool			IsRoot		() const;
		RECT			GetStretchRect	() const ;
	protected:
		virtual void	UpdateRects	();		
		void			UpdatePanel	(const RECT& rcNow,const RECT& rcDiff);
	};

	


}
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

// struct CDXUTTable : public CDXUTControl
// {
// 			CDXUTTable	();
// 	virtual ~CDXUTTable	();
// 
// 	void	Update		();
// };

#endif