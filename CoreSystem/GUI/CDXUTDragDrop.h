#ifndef _CDXUTDragDrop_20090211pm0251_
#define _CDXUTDragDrop_20090211pm0251_

namespace DragDrop
{
	class Info;
	//////////////////////////////////////////////////////////////////////////
	struct Target // 
	{			
		virtual void	OnDrop			(Info* pInfo) = 0;
		virtual void	OnMouseOver		(Info* pInfo) = 0;
		virtual void	OnDropNotTarget	(Info* pInfo) = 0;
	};
	//////////////////////////////////////////////////////////////////////////	
	
		class Manager 
		{			
			Info*	mpInfo;
		public:
					Manager			();
					~Manager		();

			void	Drag			(Info* pInfo); // <--
			void	Drop			(Target* pTarget); 
			void	DropNotTarget	(); 
			void	DropEnd			(); 
			void	OnRender		(int x,int y);
			Info*	GetCurDDInfo	();
		};
	
	
	//////////////////////////////////////////////////////////////////////////
	class Info
	{		
		static Manager*	mpMgr;
	public:
		enum
		{
			INVALID_TYPE = 0
		};
		static void	fnSetManager(Manager* m);
		static Manager*	fnGetManager();
	protected:		
		
				~Info	();
		void	_Drag	();
		int		mnType;
		int		mnSourceType;
		Target*	mpSelf;	
	public:
				Info	(int nType);
				

		inline int		GetType			()	const {	return mnType; }
		inline int		GetSourceType	()	const {	return mnSourceType; }
		inline void		SetSourceType	(int nSourceType,Target* pTar)	{	mnSourceType = nSourceType;mpSelf = pTar; }
		virtual void	OnRender		(int x,int y) = 0;
		void			OnNotTarget		();
	};
	//////////////////////////////////////////////////////////////////////////

}
typedef DragDrop::Manager		DragDropManager;
typedef DragDrop::Info			DragDropInfo;
typedef DragDrop::Target		DragDropTarget;




#define _BEGIN_DRAGDROP namespace DragDrop {
#define _END_DRAGDROP	}
#endif