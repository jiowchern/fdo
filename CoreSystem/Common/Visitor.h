#ifndef _visitor_20090220pm0533_
#define _visitor_20090220pm0533_
#include "LokiLib.h"

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
/*
	觀察者
*/


template<typename TObj>
struct DoVisit
{
	virtual void Visit(TObj& obj) {}
};
//////////////////////////////////////////////////////////////////////////
template<typename TObj>
struct DoIVisit
{
	virtual void Visit(TObj& obj) = 0;
};

template<typename TList,template <class> class Unit = DoVisit>
struct Visitor : public TL::GenClass<TList,Unit>
{
	
};

/*
	ex : 
		說明:
			ObjV1 <--- 被觀察的物件1
			ObjV2 <--- 被觀察的物件2
			Obj <--- 自定義接收事件觀察的物件

		step 1:  定義觀察者
			// 定義想觀察的物件 p.s 必須與Visitable template第一個參數(TVisitor)做匹配
			typedef Visitor<TL::TypeList<ObjV1 , TL::TypeList<ObjV2>>> VisitXXX; 

		step 2: 攔截觀察事件
			class Obj : public VisitXXX // <-- 繼承觀察者
			{
				// 實做 想要處理 Visit 虛擬函式 ...
				void Visit(ObjV1& visitable) ;
				void Visit(ObjV2& visitable) ;
			}
*/
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
/*
	被觀察者
*/
template<typename TVisitor,template <class> class Unit = DoVisit>
class Visitable
{	
	typedef std::list<TVisitor*> VisitorSet;
	typedef TVisitor* TVisitorPtr;
	VisitorSet mVisitors;

	template<typename T,int index >
	struct AcceptImpl
	{
		T& mObj;
		AcceptImpl(T& o) : mObj(o)
		{

		}

		void operator() (TVisitorPtr pVisitor)
		{
			_Visit(pVisitor , Int2Type<index>());
		}
		// 
		// 		void operator() (TVisitorPtr pVisitor)
		// 		{
		// 			DoVisit<T>& pva = TL::Field<index>(*pVisitor);			
		// 			pva->Visit(mObj);
		// 		}
	private:


		void _Visit(TVisitorPtr pVisitor,Int2Type<-1>)
		{
			Unit<T>* pva = pVisitor;
			pva->Visit(mObj);
		}

		void _Visit(TVisitorPtr pVisitor,...)
		{			
			Unit<T>& va = TL::Field<index>(*pVisitor);
			va.Visit(mObj);
		}
	};

public:

	void	Register	(TVisitorPtr pbv) 
	{
		mVisitors.push_back(pbv);
	}
	void	Unregister	(TVisitorPtr pbv) 
	{
		mVisitors.remove(pbv);
	}

	template<int index,typename T>
	void AcceptIndex(T& obj)
	{
		for_each(mVisitors.begin() , mVisitors.end() , AcceptImpl<T,index>(obj));		
	}
	template<typename T>
	void Accept(T& obj)
	{
		for_each(mVisitors.begin() , mVisitors.end() , AcceptImpl<T,-1>(obj));		
	}
};
/*
	說明:
		ObjV1 <--- 被觀察的物件1
		ObjV2 <--- 被觀察的物件2
		Obj <--- 可觀察物件
	定義方式.1
		struct Obj : Visitable<VisitorXXX>
		{
			ObjV1 mv1;
			ObjV2 mv2;
		}
	定義方式.2
		struct Obj 
		{
			ObjV1 mv1;
			ObjV2 mv2;

			Visitable<VisitorXXX> mVisitable;
		}
	
	用法:by 定義方式.1
		Obj obj;
		obj.Accept(obj.mv1); // 將mv1通知給Visitor
*/

#endif