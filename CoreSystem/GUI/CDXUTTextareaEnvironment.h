#ifndef _CDXUTTextareaEnvironment20091212pm0252_
#define _CDXUTTextareaEnvironment20091212pm0252_
#include "boost/any.hpp"
#include "GameCommand.h"

struct CDXUTTextItemEnvironment
{	
	
	sint			iFont;
	uint			nFontColor;
	uint			nHintId;
	std::wstring	baseString;	
	bool			bReturn;
	sint			nSysIcon;
	GameCommand::CommandHandle	hClickEvent;	
	GameCommand::CommandHandle	hMouseInEvent;	
	GameCommand::CommandHandle	hMouseOutEvent;	
	bool			bHasHypelink;	
	bool			bHypelink;	

			CDXUTTextItemEnvironment	();
			~CDXUTTextItemEnvironment	();

	void	DoEvent_MouseLDown			();
	void	DoEvent_MouseIn				();
	void	DoEvent_MouseOut			();

	void	SetCommandMouseLDown		(const char16* commandName,GameCommand::ParamSet& params);
	void	SetCommandMouseIn			(const char16* commandName,GameCommand::ParamSet& params);
	void	SetCommandMouseOut			(const char16* commandName,GameCommand::ParamSet& params);
	//////////////////////////////////////////////////////////////////////////
	
	static void	fnSetCommandExecuter	(GameCommand::Executer* pCE);	
	static void fnSetCommandFactory		(GameCommand::Factory* p);
	static void fnRunCommand			(GameCommand::CommandHandle hnd);
	static void	fnInitial				(GameCommand::Executer* pCE,GameCommand::Factory* p);
	static void	fnRelease				();

	//////////////////////////////////////////////////////////////////////////
	void*	operator new				(size_t size);
	void	operator delete				(void* p);			
};

#endif