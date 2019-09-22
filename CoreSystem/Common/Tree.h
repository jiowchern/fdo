#ifndef _Tree_20090223pm0410_
#define _Tree_20090223pm0410_
namespace Tree
{

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////


	template<typename T>
	class TreeNode
	{
	public:
		typedef std::list<T*> TreeNodeSet;
		typedef typename TreeNodeSet::iterator	ChildIterator;
	protected:
		T*				mpParent;
		TreeNodeSet		mChilds;

	public:
		// 					TreeNode	(TreeNode* pParent);
		// 		virtual		~TreeNode	();
		// 		void		AddChild	(TreeNode* pChild);
		// 		bool		DelChild	(TreeNode* pChild);
		// 		
		// 		
		// 		bool		GetChilds	(TreeNodeSet& childs) const;
		// 		TreeNode*	GetParent	() const;
		TreeNode(T* pParent)
			:mpParent(pParent)
		{
			if(mpParent)
				mpParent->AddChild((T*)this);
		}
		//////////////////////////////////////////////////////////////////////////
		~TreeNode()
		{

		}
		//////////////////////////////////////////////////////////////////////////
		void AddChild(T* pChild)
		{
			mChilds.push_back(pChild);
		}
		//////////////////////////////////////////////////////////////////////////
		bool DelChild(T* pChild)
		{
			TreeNodeSet::iterator it = mChilds.begin();
			for( ; it != mChilds.end() ; ++it)
			{
				if(*it != pChild)
					continue;

				mChilds.erase(it);
				return true;
			}
			return false;
		}	
		//////////////////////////////////////////////////////////////////////////
		ChildIterator GetChildsIteratorBegin() 
		{
			return mChilds.begin();
		}
		ChildIterator GetChildsIteratorEnd() 
		{
			return mChilds.end();
		}
		//////////////////////////////////////////////////////////////////////////
		bool GetChilds(TreeNodeSet& childs) const
		{
			bool bHaveChild = mChilds.size() > 0;
			if(bHaveChild)
				childs = mChilds;
			return bHaveChild;
		}
		//////////////////////////////////////////////////////////////////////////
		T* GetParent() const
		{
			return mpParent;
		}
		//////////////////////////////////////////////////////////////////////////
		void	SetParent(T* p)
		{
			mpParent = p;
		}
		//////////////////////////////////////////////////////////////////////////
		bool HasChilds() const
		{
			return mChilds.size() > 0 ;
		}
	};
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////	
	template<class T>
	class TreeIterator
	{
		//typedef T::TreeNodeSet TreeNodeSet;
		T*				mpRoot;		
		T*				mpCur;

		int				mnCurLevel;
		//TreeNode* _GetNext(TreeNode* pNode);
		//////////////////////////////////////////////////////////////////////////
		bool _GetNextSib(T* pNode,T*& pNext,int& nAddLevel)
		{
			T* pParent = pNode->GetParent();
			if(pParent)
			{
				if(_GetNextChild(pParent,pNode,pNext,nAddLevel))
				{
					nAddLevel --;
					return true;
				}				
			}
			return false;
		}
		//////////////////////////////////////////////////////////////////////////
		bool _GetNextChild(T* pNode,T* pPrev,T*& pNext,int& nAddLevel)
		{
			std::list<T*> set;
			if(pNode->GetChilds(set))
			{
				std::list<T*>::iterator it = set.begin();
				for( ;pPrev != 0 && it != set.end(); ++it)
				{
					if(*it != pPrev )
					{
						continue;
					}

					++it;
					break;
				}

				if(it != set.end())
				{
					// Next是Sib
					pNext = *it;

					return true;
				}
			}
			return false;
		}
		//////////////////////////////////////////////////////////////////////////
		bool _GetNext(T* pNode,T* pPrev,T*& pNext,int& nAddLevel)
		{						
			assert(pNode);

			if(_GetNextChild(pNode,pPrev,pNext,nAddLevel) == false)
			{				
				if(pNode != mpRoot && pNode && pNode->GetParent())
				{
					--nAddLevel;
					if(_GetNext(pNode->GetParent(),pNode,pNext,nAddLevel))
					{										
						return true;
					}					
				}
				return false;
			}
			++nAddLevel;
			return true;
		}
	public:
		struct NodeInfo
		{
			T*			pNode;
			int			nLevel;
			NodeInfo() : pNode(0) ,nLevel(-1)
			{
			}
		};
		//////////////////////////////////////////////////////////////////////////
		// 					TreeIterator	(TreeNode* pRoot);
		// 					~TreeIterator	();
		// 
		// 		NodeInfo	Get				() const;
		// 		bool		IsEnd			() const;
		// 		void		operator++		();
		TreeIterator(T* pRoot)
			: mpRoot(pRoot) ,mpCur(pRoot) ,mnCurLevel(0) 
		{			
		}
		//////////////////////////////////////////////////////////////////////////
		~TreeIterator()
		{

		}
		//////////////////////////////////////////////////////////////////////////
		NodeInfo TreeIterator::Get() const
		{
			NodeInfo inf;
			inf.nLevel	=	mnCurLevel;
			inf.pNode	=	mpCur;
			return inf;
		}
		//////////////////////////////////////////////////////////////////////////
		bool IsEnd() const
		{
			return mpCur == 0;
		}
		//////////////////////////////////////////////////////////////////////////
		void operator++()
		{			
			if(mpCur)
			{
				int nLv = mnCurLevel;				
				if(_GetNext(mpCur,0,mpCur,nLv))
				{
					mnCurLevel = nLv;
				}
				else
				{
					mpCur = 0;
				}
			}
		}

	};


	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	namespace TreeFactory 
	{
		template<typename T>
		T* fnCreate(T* parent)
		{
			T* pNode = new T(parent);		
			return pNode;
		}
		//////////////////////////////////////////////////////////////////////////
		template<typename T>
		void fnDestoryNode(T* pNode)
		{
			// 脫離與父親的關連
			T* pPapa = pNode->GetParent();
			if(pPapa)
			{
				pPapa->DelChild(pNode);
			}

			// 取出所有要切斷的節點			
			T::ChildIterator itcur = pNode->GetChildsIteratorBegin() ;
			T::ChildIterator itend = pNode->GetChildsIteratorEnd() ;
			for( ; itcur != itend ; ++itcur)
			{
				(*itcur)->SetParent(0);				
			}
		
			delete pNode;
		}	
		//////////////////////////////////////////////////////////////////////////
		template<typename TreeNode>
		void fnDestoryTree(TreeNode* pNode)
		{
			// 脫離與父親的關連
			TreeNode* pPapa = pNode->GetParent();
			if(pPapa)
			{
				pPapa->DelChild(pNode);
			}

			// 取出所有要刪除的節點
			TreeNode::TreeNodeSet delNodes;		
			TreeIterator<TreeNode> it(pNode);
			for( ; it.IsEnd() == false ; ++it)
			{
				TreeIterator<TreeNode>::NodeInfo inf = it.Get();
				if(inf.pNode != pNode)
					delNodes.push_back(inf.pNode);
			}

			// 刪除節點
			TreeNode::TreeNodeSet::iterator delit = delNodes.begin();
			for (; delit != delNodes.end() ;++delit)
			{
				delete *delit;
			}
			delete pNode;
		}	
	}
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	template<typename T>
	class TNode
	{
	public:
		typedef T					ObjectType;
		typedef TNode<T>		ThisType;
		typedef std::list<ThisType*> TreeNodeSet;
		typedef typename TreeNodeSet::iterator			ChildIterator;
		typedef typename TreeNodeSet::const_iterator	const_ChildIterator;
	protected:
		ThisType*		mpParent;
		TreeNodeSet		mChilds;
		T				mObj;
	public:
		
		TNode(ThisType* pParent)
			:mpParent(pParent) , mObj(T())
		{
			if(mpParent)
				mpParent->AddChild(this);
		}
		//////////////////////////////////////////////////////////////////////////
		~TNode()
		{

		}
		//////////////////////////////////////////////////////////////////////////
		void AddChild(ThisType* pChild)
		{
			mChilds.push_back(pChild);
		}
		//////////////////////////////////////////////////////////////////////////
		bool DelChild(ThisType* pChild)
		{
			TreeNodeSet::iterator it = mChilds.begin();
			for( ; it != mChilds.end() ; ++it)
			{
				if(*it != pChild)
					continue;

				mChilds.erase(it);
				return true;
			}
			return false;
		}	
		//////////////////////////////////////////////////////////////////////////
		ChildIterator GetChildsIteratorBegin() 
		{
			return mChilds.begin();
		}
		//////////////////////////////////////////////////////////////////////////		
		ChildIterator GetChildsIteratorEnd() 
		{
			return mChilds.end();
		}
		//////////////////////////////////////////////////////////////////////////
		const_ChildIterator GetChildsIteratorBegin() const 
		{
			return mChilds.begin();
		}
		//////////////////////////////////////////////////////////////////////////		
		const_ChildIterator GetChildsIteratorEnd() const
		{
			return mChilds.end();
		}
		//////////////////////////////////////////////////////////////////////////
		bool GetChilds(TreeNodeSet& childs) const
		{
			bool bHaveChild = mChilds.size() > 0;
			if(bHaveChild)
				childs = mChilds;
			return bHaveChild;
		}

		uint GetChildSize() const
		{
			return mChilds.size();
		}

		//////////////////////////////////////////////////////////////////////////
		ThisType* GetParent() const
		{
			return mpParent;
		}
		//////////////////////////////////////////////////////////////////////////
		void	SetParent(ThisType* p)
		{
			mpParent = p;
		}
		//////////////////////////////////////////////////////////////////////////
		bool HasChilds() const
		{
			return mChilds.size() > 0 ;
		}
		//////////////////////////////////////////////////////////////////////////
		ObjectType			GetObject		()				{return mObj;}

		ObjectType&			GetObjectRef	()				{return mObj;}

		ObjectType*			GetObjectPoint	()				{return &mObj;}

		const ObjectType			GetObject		()	const		{return mObj;}

		const ObjectType&			GetObjectRef	()	const		{return mObj;}

		const ObjectType*			GetObjectPoint	()	const		{return &mObj;}
		//////////////////////////////////////////////////////////////////////////
		void				SetObject		(ObjectType o)	{mObj = o;}

		//////////////////////////////////////////////////////////////////////////
		TNode*				InsertChild		()
		{			
// 			ThisType* pNew = 0;//TFactory::CreateNode();
// 			AddChild(pNew);
			return TreeFactory::fnCreate<ThisType>(this);
		}
	};
};


#endif