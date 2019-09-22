#include "stdafx.h"
#include "CDXUTTree.h"

namespace Tree
{

// 	TreeNode::TreeNode(TreeNode* pParent)
// 		:mpParent(pParent)
// 	{
// 		if(mpParent)
// 			mpParent->AddChild(this);
// 	}
// 	//////////////////////////////////////////////////////////////////////////
// 	TreeNode::~TreeNode()
// 	{
// 
// 	}
// 	//////////////////////////////////////////////////////////////////////////
// 	void TreeNode::AddChild(TreeNode* pChild)
// 	{
// 		mChilds.push_back(pChild);
// 	}
// 	//////////////////////////////////////////////////////////////////////////
// 	bool TreeNode::DelChild(TreeNode* pChild)
// 	{
// 		TreeNodeSet::iterator it = mChilds.begin();
// 		for( ; it != mChilds.end() ; ++it)
// 		{
// 			if(*it != pChild)
// 				continue;
// 
// 			mChilds.erase(it);
// 			return true;
// 		}
// 		return false;
// 	}	
// 	//////////////////////////////////////////////////////////////////////////
// 	bool TreeNode::GetChilds(TreeNodeSet& childs) const
// 	{
// 		bool bHaveChild = mChilds.size() > 0;
// 		if(bHaveChild)
// 			childs = mChilds;
// 		return bHaveChild;
// 	}
// 	//////////////////////////////////////////////////////////////////////////
// 	TreeNode* TreeNode::GetParent() const
// 	{
// 		return mpParent;
// 	}
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
// 	TreeIterator::TreeIterator(TreeNode* pRoot)
// 		: mpRoot(pRoot) ,mpCur(pRoot) ,mnCurLevel(0)
// 	{
// 		mpNext = _GetNext(mpCur);
// 	}
// 	//////////////////////////////////////////////////////////////////////////
// 	TreeIterator::~TreeIterator()
// 	{
// 
// 	}
// 	//////////////////////////////////////////////////////////////////////////
// 	TreeIterator::NodeInfo TreeIterator::Get() const
// 	{
// 		NodeInfo inf;
// 		inf.nLevel	=	mnCurLevel;
// 		inf.pNode	=	mpCur;
// 		return inf;
// 	}
// 	//////////////////////////////////////////////////////////////////////////
// 	bool TreeIterator::IsEnd() const
// 	{
// 		return mpNext == 0;
// 	}
// 	//////////////////////////////////////////////////////////////////////////
// 	void TreeIterator::operator++()
// 	{
// 		mpCur = mpNext;
// 		if(mpCur)
// 			mpNext = _GetNext(mpCur);
// 	}
// 	//////////////////////////////////////////////////////////////////////////
// 	TreeNode* TreeIterator::_GetNext(TreeNode* pNode)
// 	{
// 		TreeNodeSet set;
// 		if(pNode->GetChilds(set))
// 		{
// 			mnCurLevel++;
// 			return *set.begin();
// 		}
// 		else if(mpRoot != pNode)
// 		{
// 			TreeNode* pParent = pNode->GetParent();
// 			
// 			set.clear();
// 			if(pParent && pParent->GetChilds(set))
// 			{
// 				TreeNodeSet::iterator it = set.begin();
// 				for( ;it!=set.end(); ++it)
// 				{
// 					if(*it != pNode)
// 					{
// 						continue;
// 					}
// 
// 					++it;
// 					break;
// 				}
// 
// 				
// 				if(it != set.end())
// 				{
// 					// 有下一個
// 					mnCurLevel--;
// 					return *it;
// 				}
// 			}
// 			
// 		}
// 		return 0;
// 	}
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	namespace TreeFactory 
	{
		//////////////////////////////////////////////////////////////////////////
// 		TreeNode* TreeFactory::fnCreate(TreeNode* parent)
// 		{
// 			TreeNode* pNode = new TreeNode(parent);		
// 			return pNode;
// 		}
// 		//////////////////////////////////////////////////////////////////////////
// 		void TreeFactory::fnDestory(TreeNode* pNode)
// 		{
// 			// 脫離與父親的關連
// 			TreeNode* pPapa = pNode->GetParent();
// 			if(pPapa)
// 			{
// 				pPapa->DelChild(pNode);
// 			}
// 
// 			// 取出所有要刪除的節點
// 			TreeNodeSet delNodes;		
// 			TreeIterator<TreeNode> it(pNode);
// 			for( ; it.IsEnd() == false ; ++it)
// 			{
// 				TreeIterator<TreeNode>::NodeInfo inf = it.Get();
// 				delNodes.push_back(inf.pNode);
// 			}
// 
// 			// 刪除節點
// 			TreeNodeSet::iterator delit = delNodes.begin();
// 			for (; delit != delNodes.end() ;++delit)
// 			{
// 				delete *delit;
// 			}
// 		}	

	}
	
};