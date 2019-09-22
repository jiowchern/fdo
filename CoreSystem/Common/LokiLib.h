#ifndef _LokiLib_20090220pm0525_
#define _LokiLib_20090220pm0525_


template<int T>
struct Int2Type
{
	enum
	{
		V = T
	};
	
};
namespace TL
{
	struct NullClass;
	template<typename T,typename U >
	struct TypeList
	{
		typedef T Head;
		typedef U Tail;
	};

	template <typename , unsigned int index>
	struct TypeAt;

	template<class H,class T>
	struct TypeAt<TypeList<H,T>,0>
	{
		typedef H Result;
	};

	template<class H,class T,int index>
	struct TypeAt<TypeList<H,T>,index>
	{
		typedef typename TypeAt<T,index-1>::Result Result;
	};

	template<class TTTypeList,template <class> class Unit>
	struct GenClass ;

	template<class T1,class T2,template <class> class Unit>
	struct GenClass	<TL::TypeList<T1,T2>,Unit> 
		: public GenClass<T1 ,Unit>
		, public GenClass<T2 ,Unit>
	{

	};

	template<class AtomicType,template <class> class Unit>
	struct GenClass : public Unit<AtomicType>
	{

	};

	template<template <class> class Unit>
	struct GenClass<NullClass,Unit>
	{

	};
	// 	template<class TTypeList,template<class> class Unit>
	// 	Unit<TTypeList::Head>& FieldHelper(GenClass<TTypeList,Unit>& obj,Int2Type<0>)

	template<class TTypeList,template<class> class Unit>
	Unit<typename TTypeList::Head>& FieldHelper(GenClass<TTypeList,Unit>& obj,Int2Type<0>)
	{
		GenClass<TTypeList::Head,Unit>& lobj = obj;
		return lobj;
	}

	template<int i ,class TTypeList,template<class> class Unit>
	Unit<typename TypeAt<TTypeList,i>::Result>& FieldHelper(GenClass<TTypeList,Unit>& obj,Int2Type<i>)
	{
		GenClass<TTypeList::Tail,Unit>& robj = obj;
		return FieldHelper(robj,Int2Type<i-1>());
	}

	template<int i,class TTypeList,template<class> class Unit>
	Unit<typename TypeAt<TTypeList,i>::Result>&	Field(GenClass<TTypeList,Unit>& obj)
	{
		return FieldHelper(obj,Int2Type<i>());
	}
}

#define TYPELIST_1(T1) TL::TypeList<T1, TL::NullClass>

#define TYPELIST_2(T1, T2) TL::TypeList<T1, TYPELIST_1(T2) >

#define TYPELIST_3(T1, T2, T3) TL::TypeList<T1, TYPELIST_2(T2, T3) >

#define TYPELIST_4(T1, T2, T3, T4) \
	TL::TypeList<T1, TYPELIST_3(T2, T3, T4) >

#define TYPELIST_5(T1, T2, T3, T4, T5) \
	TL::TypeList<T1, TYPELIST_4(T2, T3, T4, T5) >

#define TYPELIST_6(T1, T2, T3, T4, T5, T6) \
	TL::TypeList<T1, TYPELIST_5(T2, T3, T4, T5, T6) >

#define TYPELIST_7(T1, T2, T3, T4, T5, T6, T7) \
	TL::TypeList<T1, TYPELIST_6(T2, T3, T4, T5, T6, T7) >

#define TYPELIST_8(T1, T2, T3, T4, T5, T6, T7, T8) \
	TL::TypeList<T1, TYPELIST_7(T2, T3, T4, T5, T6, T7, T8) >

#define TYPELIST_9(T1, T2, T3, T4, T5, T6, T7, T8, T9) \
	TL::TypeList<T1, TYPELIST_8(T2, T3, T4, T5, T6, T7, T8, T9) >

#define TYPELIST_10(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10) \
	TL::TypeList<T1, TYPELIST_9(T2, T3, T4, T5, T6, T7, T8, T9, T10) >

#define TYPELIST_11(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11) \
	TL::TypeList<T1, TYPELIST_10(T2, T3, T4, T5, T6, T7, T8, T9, T10, T11) >

#define TYPELIST_12(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12) \
	TL::TypeList<T1, TYPELIST_11(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
	T11, T12) >

#define TYPELIST_13(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13) \
	TL::TypeList<T1, TYPELIST_12(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
	T11, T12, T13) >

#define TYPELIST_14(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
	T11, T12, T13, T14) \
	TL::TypeList<T1, TYPELIST_13(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
	T11, T12, T13, T14) >

#define TYPELIST_15(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
	T11, T12, T13, T14, T15) \
	TL::TypeList<T1, TYPELIST_14(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
	T11, T12, T13, T14, T15) >

#define TYPELIST_16(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
	T11, T12, T13, T14, T15, T16) \
	TL::TypeList<T1, TYPELIST_15(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
	T11, T12, T13, T14, T15, T16) >

#define TYPELIST_17(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
	T11, T12, T13, T14, T15, T16, T17) \
	TL::TypeList<T1, TYPELIST_16(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
	T11, T12, T13, T14, T15, T16, T17) >

#define TYPELIST_18(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
	T11, T12, T13, T14, T15, T16, T17, T18) \
	TL::TypeList<T1, TYPELIST_17(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
	T11, T12, T13, T14, T15, T16, T17, T18) >

#define TYPELIST_19(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
	T11, T12, T13, T14, T15, T16, T17, T18, T19) \
	TL::TypeList<T1, TYPELIST_18(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
	T11, T12, T13, T14, T15, T16, T17, T18, T19) >

#define TYPELIST_20(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
	T11, T12, T13, T14, T15, T16, T17, T18, T19, T20) \
	TL::TypeList<T1, TYPELIST_19(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
	T11, T12, T13, T14, T15, T16, T17, T18, T19, T20) >

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
namespace ClassPolicy
{
	struct NullClass;
	template<template <class> class T1,typename T2>
	struct TTemplateList
	{

	};


	template<template<class> class TUnit,typename TBase>
	struct TClAssassembledUnit : public TUnit<TBase>
	{

	};

	template<typename TBase ,	 
		template <class> class TTypeList,
		template <template<class> class,class> class TUnit
	>
	struct TClAssassembledPolicy : public TUnit<TTypeList,TBase>
	{

	};

	// 宣告
	template<typename TBase ,	 
		typename TTypeList,
		template <
		template<class> class
		,class
		> class TUnit = ClassPolicy::TClAssassembledUnit
	>
	struct TClAssassembled ;

	// 特化
	template<typename TBase,template <class> class T1,typename T2,template <template<class> class,class> class Unit>
	struct TClAssassembled	<TBase,TTemplateList<T1,T2>,Unit> 	
		:public TClAssassembled<TBase,T2,Unit>
		,public TClAssassembledPolicy<TBase,T1,Unit>
	{

	};

	// 特化
	template<typename TBase,template <template<class> class,class> class Unit >
	struct TClAssassembled<TBase,NullClass,Unit>
	{

	};

}

#define TEMPLIST_1(T1)		ClassPolicy::TTemplateList<T1,ClassPolicy::NullClass>
#define TEMPLIST_2(T1, T2)	ClassPolicy::TTemplateList<T1, TEMPLIST_1(T2) >

// template<typename TReturn,typename TParamList>
// struct TDelegate
// {
// 	struct IFunctor
// 	{
// 		virtual TReturn operator()()		;
// 		virtual TReturn operator()() const  ;
// 	};
// 
// 	template<typename T,typename TPfn>
// 	struct TMemberFunctor 
// 	{
// 
// 		typedef TPfn	FunctionPointType;
// 		FunctionPointType	mpfn;
// 		TMemberFunctor(T* pObj,TPfn pfn) : mpfn(pfn)
// 		{
// 
// 		}
// 	};
// 
// 	template<typename T,typename TPfn>
// 	TDelegate(T* pObj,TPfn pfn)
// 	{
// 		mpFunctor = new TMemberFunctor<T,TPfn>(pObj,pfn);
// 	}
// 
// 	TReturn operator()()		{	return mpFunctor();	}
// 	TReturn operator()() const  {	return mpFunctor();	}
// 
// 	IFunctor*	mpFunctor;
// };

#endif