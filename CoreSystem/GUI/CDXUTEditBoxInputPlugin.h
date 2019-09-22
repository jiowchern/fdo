#ifndef CDXUTEditBoxInputPlugin_H_20100330
#define CDXUTEditBoxInputPlugin_H_20100330

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
namespace EditBoxPlugin
{
	//////////////////////////////////////////////////////////////////////////
	//
	//////////////////////////////////////////////////////////////////////////
	struct FilterCharFunctor
	{	
		FilterCharFunctor(const wstring& str)
			: m_FilterStr(str)
		{
			//static char nums[10] = { '0','1','2','3','4','5','6','7','8','9' };
		}
		bool operator()(char c)
		{
			
			for (uint i = 0 ; i < m_FilterStr.size() ; ++i)
			{
				if(m_FilterStr[i] != c)
					continue;

				return false;
			}
			return true;
		}
	private:
		wstring m_FilterStr;
	}; 

	//////////////////////////////////////////////////////////////////////////
	//
	//////////////////////////////////////////////////////////////////////////
	enum InputMode
	{
		NON_LIMIT,
		ONLY_NUMBER ,
		ONLY_ENGLISH,
		ONLY_ENGLISH_NUMBER ,				
		MODE_COUMT,
	};

	enum
	{
		NO_MILIT_NUMBER_RANGE = -1,
	};


	//////////////////////////////////////////////////////////////////////////
	//
	//////////////////////////////////////////////////////////////////////////
	struct CDXUTEditBoxInputPlugin
	{

	private:
		InputMode m_InputMode;
		uint m_MaxNumber;
		uint m_MinNumber;
		bool m_bRangeLimit;

	public:
		CDXUTEditBoxInputPlugin							();
		CDXUTEditBoxInputPlugin							(InputMode nInputMode, const sint nMaxNum = NO_MILIT_NUMBER_RANGE, const sint nMinNum = NO_MILIT_NUMBER_RANGE);
		~CDXUTEditBoxInputPlugin						();
		//////////////////////////////////////////////////////////////////////////
		//
		//////////////////////////////////////////////////////////////////////////
		void		SetInputMode						(InputMode mode, sint nMaxNum = NO_MILIT_NUMBER_RANGE, sint nMinNum = NO_MILIT_NUMBER_RANGE);
		void		RunInputPlugin						(wstring& inputStr);
		
		//////////////////////////////////////////////////////////////////////////
		//設定參數
		//////////////////////////////////////////////////////////////////////////
	private:
		void		_SetInputNumberRange				(sint nMaxNum, sint nMinNum);
		void		_CheckRangeRule						(sint nMaxNum, sint nMinNum);
		void		_OnlyNumBerRule						(wstring& inputStr);
		void		_CheckInputNumberRange				(wstring& inputStr);

		//////////////////////////////////////////////////////////////////////////
		//boot功能
		//////////////////////////////////////////////////////////////////////////
		void		_FilterOnlyNumber					(FilterCharFunctor& funtor, wstring& inputStr);
		void		_TrimLeft_Str						(wstring& inputStr, const wstring& str);
		void		_EraseAll							(wstring& inputStr, const wstring& str);
	};
}
#endif