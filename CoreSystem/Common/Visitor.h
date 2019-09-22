#ifndef _visitor_20090220pm0533_
#define _visitor_20090220pm0533_
#include "LokiLib.h"

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
/*
	�[���
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
		����:
			ObjV1 <--- �Q�[�����1
			ObjV2 <--- �Q�[�����2
			Obj <--- �۩w�q�����ƥ��[�����

		step 1:  �w�q�[���
			// �w�q�Q�[����� p.s �����PVisitable template�Ĥ@�ӰѼ�(TVisitor)���ǰt
			typedef Visitor<TL::TypeList<ObjV1 , TL::TypeList<ObjV2>>> VisitXXX; 

		step 2: �d�I�[��ƥ�
			class Obj : public VisitXXX // <-- �~���[���
			{
				// �갵 �Q�n�B�z Visit �����禡 ...
				void Visit(ObjV1& visitable) ;
				void Visit(ObjV2& visitable) ;
			}
*/
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
/*
	�Q�[���
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
	����:
		ObjV1 <--- �Q�[�����1
		ObjV2 <--- �Q�[�����2
		Obj <--- �i�[���
	�w�q�覡.1
		struct Obj : Visitable<VisitorXXX>
		{
			ObjV1 mv1;
			ObjV2 mv2;
		}
	�w�q�覡.2
		struct Obj 
		{
			ObjV1 mv1;
			ObjV2 mv2;

			Visitable<VisitorXXX> mVisitable;
		}
	
	�Ϊk:by �w�q�覡.1
		Obj obj;
		obj.Accept(obj.mv1); // �Nmv1�q����Visitor
*/

#endif