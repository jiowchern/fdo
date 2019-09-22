#ifndef _GameMessageEventBase_20090612pm0206_
#define _GameMessageEventBase_20090612pm0206_
#include "LokiLib.h"
#include "GameMessageEvent.h"
#include "ElementDef.h"
#include "Log.h"
#define def_gameevent_param(obj,name) \
	typedef FDO::TDGGET::TParam<FDO::TSmallObj<obj>> name;

#define GAMEEVENT_DEFPARAM(obj) \
	typedef FDO::TDGGET::TParam<FDO::TSmallObj<obj>> 

#define def_gameevent_param_eventid(obj,eventid,name) \
	typedef FDO::TDGGET::TPairEventParam<FDO::TSmallObj<obj>,eventid> name;

namespace FDO
{


	namespace TDGGET
	{		

		//////////////////////////////////////////////////////////////////////////
		template<typename TData>
		struct TParam  
		{				
			typedef TData Data;
			typedef typename TData::Base Object;
			typedef const TData* ObjPtr;

			TParam(GEParam param) : mparam(param)
			{

			}

			TParam() : mparam( new TData)
			{

			}

			template<typename TP1>
			TParam(TP1 tp1) : mparam( new TData(tp1))
			{

			}

			template<typename TP1,typename TP2>
			TParam(TP1 tp1,TP2 tp2) : mparam( new TData(tp1,tp2))
			{

			}

			template<typename TP1,typename TP2,typename TP3>
			TParam(TP1 tp1,TP2 tp2,TP3 tp3) : mparam( new TData(tp1,tp2,tp3))
			{

			}

			template<typename TP1,typename TP2,typename TP3,typename TP4>
			TParam(TP1 tp1,TP2 tp2,TP3 tp3,TP4 tp4) : mparam( new TData(tp1,tp2,tp3,tp4))
			{

			}


			TData* operator-> () const 
			{
				return static_cast<TData*>(mparam.get());
			}

			GEParam GetParam	()
			{
				return mparam;
			}


		private:
			GEParam mparam;
		};
		//////////////////////////////////////////////////////////////////////////
		template<typename TData , uint nEventId>
		struct TPairEventParam : public TParam<TData>
		{
			enum
			{
				EVENTID = nEventId
			};
		};


		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////

	};
	//////////////////////////////////////////////////////////////////////////

	template<typename T>
	struct TEventObjectFactory : public ObjFactory<T,T::BLOCK_SIZE>
	{		

		enum
		{
			BlockSize = T::BLOCK_SIZE
		};

		uint mnMaxUsage ;
		inline T* Create()
		{	
			uint nCurCount = ObjFactory<T,BlockSize>::GetObjCount();

			if(mnMaxUsage < nCurCount + 1)
				mnMaxUsage = nCurCount + 1;
			
			if( (nCurCount / BlockSize) < ((nCurCount + 1) / BlockSize))
			{
				_LOG(0 , L"TDGGameEventObjectSize:%d ,BlockSize = %d ,MaxUsage:%d\n",sizeof(T),BlockSize,mnMaxUsage );	
			}

			return ObjFactory<T,BlockSize>::Create();
		}

		inline void Destroy(void* p)
		{ 
			memset(p,0x12345678,sizeof(T));
			ObjFactory<T,BlockSize>::Destroy(p);			
		}

		TEventObjectFactory() : mnMaxUsage(0)
		{

		}

		~TEventObjectFactory() 
		{								
			_LOG(0 , L"TDGGameEventObjectSize:%d ,BlockSize = %d ,MaxUsage:%d\n",sizeof(T),BlockSize,mnMaxUsage );						
		}


	};

	template<uint nObjSize>
	struct TBlockSize
	{
		enum
		{
			BLOCK_SIZE = 50
		};
	};

	//©w¸q°Ï¶ô
	template<> struct TBlockSize<16>	{		enum{	BLOCK_SIZE = 1000	};	};	
	template<> struct TBlockSize<32>	{		enum{	BLOCK_SIZE = 1000	};	};	

	template<uint nSize>
	struct TBuffer
	{
		enum
		{
			BLOCK_SIZE = TBlockSize<nSize>::BLOCK_SIZE
		};
		char	mem[nSize];		
	};

	typedef TYPELIST_8(TBuffer<16> , TBuffer<32> ,TBuffer<64> , TBuffer<128> , TBuffer<256>,TBuffer<512>,TBuffer<1024>,TBuffer<2048>) BufferList;
	struct TEventObjectFactorySet : public TL::GenClass< BufferList , TEventObjectFactory> , public FDO::Singleton<TEventObjectFactorySet>
	{

	};

	template<uint nNum,uint curPow = 1 >
	struct TNumberIn2thpower
	{
		enum
		{	
			TEMP1		= curPow * 2 < nNum ? nNum : 0	,
			TEMP2		= curPow * 2 < nNum ? curPow * 2: 0	,
			POWER2TH	= TNumberIn2thpower<TEMP1,TEMP2>::POWER2TH + 1,		
			NUMBER		= curPow + TNumberIn2thpower<TEMP1,TEMP2>::NUMBER
		};
	};

	template<>
	struct TNumberIn2thpower<0,0>
	{
		enum
		{
			POWER2TH = 0,
			NUMBER = 1
		};
	};

	//////////////////////////////////////////////////////////////////////////
	template<typename T>
	struct TSmallObj : public T
	{
		enum {	FACTORY_OBJSIZE = TNumberIn2thpower<sizeof(T)>::NUMBER < 16 ? 16 : TNumberIn2thpower<sizeof(T)>::NUMBER };
		typedef T				Base;
		typedef TEventObjectFactory< TBuffer<FACTORY_OBJSIZE> >	Factory;
		typedef TSmallObj<T>	ThisType;

		TSmallObj()
		{

		}

		template<typename TP1>
		TSmallObj(TP1 tp1) : T(tp1)
		{

		}

		template<typename TP1,typename TP2>
		TSmallObj(TP1 tp1,TP2 tp2) : T(tp1,tp2)
		{

		}

		template<typename TP1,typename TP2,typename TP3>
		TSmallObj(TP1 tp1,TP2 tp2,TP3 tp3) : T(tp1,tp2,tp3)
		{

		}

		template<typename TP1,typename TP2,typename TP3,typename TP4>
		TSmallObj(TP1 tp1,TP2 tp2 ,TP3 tp3 ,TP4 tp4) : T(tp1,tp2,tp3,tp4)
		{

		}

		virtual ~TSmallObj()
		{

		}

		//static TSmallObj<T>* fnCreate(size_t size)
		static void* fnCreate(size_t size)
		{			
			return static_cast<Factory&>(TEventObjectFactorySet::GetSingleton()).Create();
			//return new(static_cast<Factory&>(TEventObjectFactorySet::GetSingleton()).Create()) TSmallObj();
		}

		//////////////////////////////////////////////////////////////////////////
		static void fnDestroy(void* p)
		{
			static_cast<Factory&>(TEventObjectFactorySet::GetSingleton()).Destroy(p);
		}
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		// 		void* operator new(size_t, void *_Where) 
		// 		{
		// 			return (_Where);
		// 		}
		void* operator new(size_t size)
		{					
			return fnCreate(size);
		}
		//////////////////////////////////////////////////////////////////////////
		// 		void operator delete(void* p,void* pFactory)
		// 		{
		// 			((TSmallObj*)p)->~TSmallObj();
		// 			fnDestroy(p);
		// 		}
		void operator delete(void* p)
		{
			((ThisType*)p)->~ThisType();
			fnDestroy(p);

		}
	};



}

#endif