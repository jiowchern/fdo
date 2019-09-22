#include "stdafx.h"
#include "CDXUTEditBoxInputPlugin.h"
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost\range\iterator_range.hpp>
#include <boost\algorithm\string\trim.hpp>

namespace EditBoxPlugin
{
	using namespace boost;
	
	//static char gnNums[10] = { '0','1','2','3','4','5','6','7','8','9' };

// 		NON_LIMIT,
// 		ONLY_NUMBER ,
// 		ONLY_ENGLISH,
// 		ONLY_ENGLISH_NUMBER ,				
	
	const wstring gnNeedStr[MODE_COUMT] = 
	{
		L"",
		L"0123456789",
		L"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ",
		L"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ",
	};
	//static wstring gnStr(L"0123456789");
	//static wstring gnOnlyNumberStr(L"0123456789");

	//////////////////////////////////////////////////////////////////////////
	//
	//////////////////////////////////////////////////////////////////////////
	CDXUTEditBoxInputPlugin::CDXUTEditBoxInputPlugin()
		: m_InputMode(NON_LIMIT)
		, m_MaxNumber(-1)
		, m_MinNumber(-1)
		, m_bRangeLimit(false)
	{
	}

	//////////////////////////////////////////////////////////////////////////
	//
	//////////////////////////////////////////////////////////////////////////
	CDXUTEditBoxInputPlugin::CDXUTEditBoxInputPlugin(InputMode nInputMode, const sint nMaxNum, const sint nMinNum)
		: m_InputMode(nInputMode)
		, m_MaxNumber(nMaxNum)
		, m_MinNumber(nMinNum)
	{
		SetInputMode( nInputMode, nMaxNum, nMinNum );
	}

	//////////////////////////////////////////////////////////////////////////
	//
	//////////////////////////////////////////////////////////////////////////
	CDXUTEditBoxInputPlugin::~CDXUTEditBoxInputPlugin()
	{
	}

	//////////////////////////////////////////////////////////////////////////
	//
	//////////////////////////////////////////////////////////////////////////
	void CDXUTEditBoxInputPlugin::SetInputMode(InputMode mode, sint nMaxNum, sint nMinNum)
	{ 
		m_InputMode = mode; 
		_SetInputNumberRange( nMaxNum, nMinNum );
	}

	//////////////////////////////////////////////////////////////////////////
	//
	//////////////////////////////////////////////////////////////////////////
	void CDXUTEditBoxInputPlugin::_SetInputNumberRange(sint nMaxNum, sint nMinNum)
	{
		if( m_InputMode != ONLY_NUMBER )
		{
			Assert( 0, L"you will set number range for editbox，"
				L" please set input mode to [only_number]" );
			return ;
		}
		_CheckRangeRule( nMaxNum, nMinNum );
	}

	//////////////////////////////////////////////////////////////////////////
	//
	//////////////////////////////////////////////////////////////////////////
	void CDXUTEditBoxInputPlugin::RunInputPlugin(wstring& inputStr)
	{
		switch(m_InputMode)
		{
		case NON_LIMIT: break;

		case ONLY_NUMBER:
			_OnlyNumBerRule(inputStr);
			break;
		case ONLY_ENGLISH:break;

		case ONLY_ENGLISH_NUMBER:break;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	//
	//////////////////////////////////////////////////////////////////////////
	void CDXUTEditBoxInputPlugin::_CheckRangeRule(sint nMaxNum, sint nMinNum)
	{
		if( nMaxNum < nMinNum )
		{
			Assert( 0, L"最大值比最小的小!!" );
			return ;
		}

		if( nMaxNum == NO_MILIT_NUMBER_RANGE && 
			nMinNum == NO_MILIT_NUMBER_RANGE )
		{
			m_bRangeLimit = false;
			return ;
		}
		m_MaxNumber = nMaxNum;
		m_MinNumber = nMinNum;
		m_bRangeLimit = true;
	}

	//////////////////////////////////////////////////////////////////////////
	//
	//////////////////////////////////////////////////////////////////////////
	void CDXUTEditBoxInputPlugin::_OnlyNumBerRule(wstring& inputStr)
	{
		_FilterOnlyNumber(FilterCharFunctor(gnNeedStr[m_InputMode]), inputStr);
		_TrimLeft_Str(inputStr, L"0");

		if( inputStr.empty() )
		{
			wstring tmp(L"0");
			inputStr = tmp;
		}
		_CheckInputNumberRange(inputStr);
	}
	//////////////////////////////////////////////////////////////////////////
	//
	//////////////////////////////////////////////////////////////////////////
	void CDXUTEditBoxInputPlugin::_CheckInputNumberRange(wstring& inputStr)
	{
		if( inputStr.empty() || !m_bRangeLimit )
		{
			return ;
		}
		 
		sint64 nInputNumber = 0;
		try 
     	{  
			nInputNumber = boost::lexical_cast<sint64>(inputStr);
     	}  
     	catch(bad_lexical_cast & e)  
     	{  
			Assert( 0, e.what() );
	 	}  

		if( nInputNumber > m_MaxNumber )
		{
			nInputNumber = m_MaxNumber;
		}
		else if( nInputNumber < m_MinNumber )
		{
			nInputNumber = m_MinNumber;
		}

		inputStr = boost::lexical_cast<wstring>(nInputNumber);
	}

	//////////////////////////////////////////////////////////////////////////
	//
	//////////////////////////////////////////////////////////////////////////
	void CDXUTEditBoxInputPlugin::_FilterOnlyNumber(FilterCharFunctor& funtor, wstring& inputStr)
	{
		inputStr.erase( 
			remove_if( inputStr.begin(),inputStr.end(), 
			funtor ), 
			inputStr.end() );	
	}

	//////////////////////////////////////////////////////////////////////////
	//
	//////////////////////////////////////////////////////////////////////////
	void CDXUTEditBoxInputPlugin::_TrimLeft_Str(wstring& inputStr, const wstring& trimStr)
	{
		trim_left_if( inputStr, is_any_of(trimStr) );
	}

	//////////////////////////////////////////////////////////////////////////
	//
	//////////////////////////////////////////////////////////////////////////
	void CDXUTEditBoxInputPlugin::_EraseAll(wstring& inputStr, const wstring& eraseStr)
	{
		erase_all(inputStr, eraseStr);
	}
}



