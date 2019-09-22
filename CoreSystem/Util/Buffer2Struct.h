#ifndef _Buffer2Struct_20090813pm0356_
#define _Buffer2Struct_20090813pm0356_

#include "LokiLib.h"

namespace Buffer2Struct
{
	typedef uint ValueType;


	
	template<typename TType,sint nCount = 1>
	struct Value 
	{
		typedef TType Type;
		enum
		{
			COUNT = nCount,VALUESIZE = sizeof(TType)
		};

		template<typename TType>
		static sint AssignOne(const char8* buf,char8* pstruct) 
		{
			(*(TType*)pstruct) = (*(TType*)buf);
			return sizeof(TType);
		}

		template<>
		static sint AssignOne<TL::NullClass>(const char8* buf,char8* pstruct) 
		{
			return 0;
		}

		template<>
		static sint AssignOne<std::wstring>(const char8* buf,char8* pstruct) ; 

	};
	
	//////////////////////////////////////////////////////////////////////////	

	
	template<typename TArgList,uint iIdx >
	sint ToStructLoop	(const char8* buff , char8* pStruct)
	{
		typedef TL::TypeAt< TArgList , iIdx >::Result ThisType;
		const uint nCount = ThisType::COUNT;
		const char8* beginBuff = buff;
		for (sint i = 0 ; i < nCount ; ++i)
		{
			sint nReadSize = ThisType::AssignOne<ThisType::Type>(buff, pStruct + (i * ThisType::VALUESIZE));
			if(nReadSize)
				buff += nReadSize;
		}		
		return 0;
	}

	template<typename TArgList>
	sint ToStruct	(const char8* buff , char8* pStruct)
	{
		ToStructLoop<TArgList,0>(buff , pStruct);
		return 0;
	}
	

	

	//////////////////////////////////////////////////////////////////////////
// 	void Test()
// 	{
// 		struct AAA
// 		{
// 			int i;
// 			int a[9];
// 			std::wstring str;
// 		};
// 
// 		AAA obj;
// 		const char* buf;
// 		
// 		//ToStruct<TYPELIST_2( Value<int,10> , Value<std::wstring> )>(buf,(char8*)&obj);
// 	}
};
#endif