#ifndef _CDXUTTreeList_20081023pm0522_
#define _CDXUTTreeList_20081023pm0522_
#include "CDXUTTree.h"
#include "CDXUTListBox.h"
#include "ListBoxItem.h"
namespace FDO
{
	

	

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	typedef int TextureId;	
	
	
	class CDXUTTreeNode : public Tree::TreeNode<CDXUTTreeNode>
	{
	public:
		enum{	Close,Open,Count	};
	protected:	
		enum{	TEXT_LEN = 256};
		CDXUTElement	mTxr;
		char16			mnText[TEXT_LEN];		
		int				mnCurState;
		void*			mpData;
	public:
					CDXUTTreeNode	(CDXUTTreeNode* pParent);
					~CDXUTTreeNode	();
		void		Initial			(void* pData,const char16* wstext,const CDXUTElement* pTxr);
		void		SetOpen			(bool bOpen);

		void		SetText			(const wchar_t* text);		
		void		SetData			(void* pData);		

		inline bool	IsOpen			()const {	return mnCurState == Open;};

		void*					GetData			()	const ;
		inline const char16*	GetText			()	const {	return mnText; };
		CDXUTElement*			GetCurElement	()	;
	};

	typedef Tree::TreeIterator<CDXUTTreeNode>	TreeIterator;
	#define CreateTreeNode Tree::TreeFactory::fnCreate<CDXUTTreeNode>	
	#define DestoryTreeNode Tree::TreeFactory::fnDestoryTree<CDXUTTreeNode>	
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	class DXUTTreeItem : public DXUTListBoxItem 
	{		
		RECT					mrcTrigger;		
		
	public:
		CDXUTTreeNode*			pNode;
		int						nLevel;
		DXUTTreeItem	(const TreeIterator::NodeInfo& inf);
		
		~DXUTTreeItem	();
				
		void			Blend			(float fElapsedTime);
		void			SetTriggerRect	(const RECT& rc);
		bool			InTriggerRect	(const POINT& pt);
	};
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	class CDXUTTreeList : public CDXUTListBox 
	{	
	public:
		// element 使用列舉
		enum
		{

			Open = 2, // CDXUTListBox Element的使用數量
			Close,
			Node ,
			Count 
		};
	protected:
		CDXUTTreeNode*						mpRoot;			
		CDXUTTreeNode*						mpHideSelectNode;			
		int									mnBeginIndex;
		
		// 排版設定
		
		
		int				mnIndent; //縮排 單位像素		
		SIZE			mszIcon;	// icon size
		void	_AddItem	(TreeIterator::NodeInfo& inf);
		
	public:
		typedef struct 
		{
			int					nIndent;
			const CDXUTElement*	closeIcon;
			const CDXUTElement*	openIcon;
			const CDXUTElement*	nodeIcon;
			SIZE				szIcon;	// icon size
			DWORD				dwStyle;
		}	Style;

				CDXUTTreeList	( CDXUTDialog *pDialog );
				~CDXUTTreeList	();

		void	Initital		(int ID, int x, int y, int width, int height, const Style* pStyle = 0);					
		void	SetBegin		(int nIdx,bool bUpdate = false);
		void	SetIconSize		(int cx,int cy);
		void	SetIndent		(int nIndent);
		void	UpdateList		();
		
		CDXUTTreeNode*			CreateRoot		();
		void					DestoryRoot		();
		inline CDXUTTreeNode*	GetRoot			() {return mpRoot;};

		CDXUTTreeNode*		InsertChild		(CDXUTTreeNode*	pNode);
		CDXUTTreeNode*		InsertSib		(CDXUTTreeNode*	pNode);

		CDXUTTreeNode*		InsertChild		(CDXUTTreeNode*	pParent,void* pData,const char16* wstext,const CDXUTElement* pTxr = 0);
		CDXUTTreeNode*		InsertSib		(CDXUTTreeNode*	pSib,void* pData,const char16* wstext,const CDXUTElement* pTxr = 0);
		
		void				RemoveNode		(CDXUTTreeNode* pNode);
		virtual void		DrawItemText	(DXUTListBoxItem* pItem, CDXUTElement* pElement, RECT* pRect);
		
		//--------------------------------------------------------------------------------
		// CDXUTControl interface overridings
		//--------------------------------------------------------------------------------
		virtual bool    HandleMouse	( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );
		virtual void	Render		( IDirect3DDevice9* pd3dDevice, float fElapsedTime );
		virtual	void	UpdateRects	();


		virtual void	Visit		(ScrollBarEvent::CurPos& e) ;	
	};
}
#endif