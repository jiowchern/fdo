#ifndef _Listener_20081006pm0426_
#define _Listener_20081006pm0426_

class UIListener
{	
	std::vector<char>	mFilter;
protected:
public:	
					UIListener	(const char* filter,int nsize);
					~UIListener	();
	void			Send		(int nType,const wchar_t* fmt, ... );
	virtual void	OnEvent		(int nType,const wchar_t* text) ;
};

class UICreateListener : public UIListener
{
public:
	UICreateListener();
	~UICreateListener();
	enum
	{
		ProgMessage,ProgError,ProgWarning,
		XMLLog,DataMessage,DataError,DataWarning,Count
	};	
};

typedef UICreateListener UICL;
#endif