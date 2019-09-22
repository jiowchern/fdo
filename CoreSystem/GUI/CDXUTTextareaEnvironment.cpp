#include "stdafx.h"
#include "CDXUTTextareaEnvironment.h"
//////////////////////////////////////////////////////////////////////////
static GameCommand::Executer*	gpCE = 0;
static GameCommand::Factory* gpFac = 0;

typedef ObjFactory<CDXUTTextItemEnvironment> CDXUTTextItemEnvironmentFactory;
static CDXUTTextItemEnvironmentFactory* gpCDXUTTextItemEnvironmentFactory = 0;
void	fnDestroyCommand(GameCommand::CommandHandle hnd)
{
	if(gpFac)
		gpFac->Recycle(hnd);
}
//////////////////////////////////////////////////////////////////////////
void CDXUTTextItemEnvironment::fnInitial(GameCommand::Executer* pCE,GameCommand::Factory* p)
{
	fnSetCommandExecuter(pCE);
	fnSetCommandFactory(p);
	gpCDXUTTextItemEnvironmentFactory = new CDXUTTextItemEnvironmentFactory();
}
//////////////////////////////////////////////////////////////////////////
void CDXUTTextItemEnvironment::fnRelease()
{
	fnSetCommandExecuter(0);
	fnSetCommandFactory(0);
	delete gpCDXUTTextItemEnvironmentFactory;
}
//////////////////////////////////////////////////////////////////////////
void CDXUTTextItemEnvironment::fnSetCommandExecuter(GameCommand::Executer* pCE)
{
	gpCE = pCE;
}
//////////////////////////////////////////////////////////////////////////
void CDXUTTextItemEnvironment::fnSetCommandFactory(GameCommand::Factory* p)
{
	gpFac = p;
}
//////////////////////////////////////////////////////////////////////////
void CDXUTTextItemEnvironment::fnRunCommand(GameCommand::CommandHandle hnd)
{
	if(gpCE)
		gpCE->Run(hnd);
}
//////////////////////////////////////////////////////////////////////////
void* CDXUTTextItemEnvironment::operator new(size_t size)
{
	if(gpCDXUTTextItemEnvironmentFactory)
	{
		return gpCDXUTTextItemEnvironmentFactory->Create();
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////////
void CDXUTTextItemEnvironment::operator delete(void* p)
{
	if(gpCDXUTTextItemEnvironmentFactory)
	{
		gpCDXUTTextItemEnvironmentFactory->Destroy(p);
	}
}
//////////////////////////////////////////////////////////////////////////
CDXUTTextItemEnvironment::CDXUTTextItemEnvironment() : 
iFont(0),nFontColor(0xffffffff),bReturn(false),nSysIcon(-1) , bHypelink(false),bHasHypelink(false)
,nHintId(0)
{	
	
}
//////////////////////////////////////////////////////////////////////////
CDXUTTextItemEnvironment::~CDXUTTextItemEnvironment()
{
	fnRunCommand(hMouseOutEvent);

	fnDestroyCommand(hClickEvent);
	fnDestroyCommand(hMouseInEvent);
	fnDestroyCommand(hMouseOutEvent);	
}
//////////////////////////////////////////////////////////////////////////
void CDXUTTextItemEnvironment::DoEvent_MouseLDown()
{
	fnRunCommand(hClickEvent);
}
//////////////////////////////////////////////////////////////////////////
void CDXUTTextItemEnvironment::DoEvent_MouseIn()
{
	fnRunCommand(hMouseInEvent);
}
//////////////////////////////////////////////////////////////////////////
void CDXUTTextItemEnvironment::DoEvent_MouseOut()
{
	fnRunCommand(hMouseOutEvent);
}
//////////////////////////////////////////////////////////////////////////
void CDXUTTextItemEnvironment::SetCommandMouseLDown(const char16* commandName,GameCommand::ParamSet& params)
{
	if(gpFac)
	{
		hClickEvent = gpFac->Make(commandName , params);		
	}
	bHasHypelink = true;
	CDXUTDialogResourceManager* pMgr = CDXUTDialog::GetManager();
	DXUTFontNode* pNode = pMgr->GetFontNode(iFont);
	if(pNode)
	{
		iFont = pMgr->AddFont(pNode->strFace,pNode->nHeight,pNode->nWeight+ 200,false , true);
	}
}
//////////////////////////////////////////////////////////////////////////
void CDXUTTextItemEnvironment::SetCommandMouseIn(const char16* commandName,GameCommand::ParamSet& params)
{
	if(gpFac)
	{
		hMouseInEvent = gpFac->Make(commandName , params);
	}	
	bHasHypelink = true;
	CDXUTDialogResourceManager* pMgr = CDXUTDialog::GetManager();
	DXUTFontNode* pNode = pMgr->GetFontNode(iFont);
	if(pNode)
	{
		iFont = pMgr->AddFont(pNode->strFace,pNode->nHeight,pNode->nWeight + 200,false , true);
	}
}
//////////////////////////////////////////////////////////////////////////
void CDXUTTextItemEnvironment::SetCommandMouseOut(const char16* commandName,GameCommand::ParamSet& params)
{
	if(gpFac)
	{
		hMouseOutEvent = gpFac->Make(commandName , params);
	}	
	bHasHypelink = true;
	CDXUTDialogResourceManager* pMgr = CDXUTDialog::GetManager();
	DXUTFontNode* pNode = pMgr->GetFontNode(iFont);
	if(pNode)
	{
		iFont = pMgr->AddFont(pNode->strFace,pNode->nHeight,pNode->nWeight+ 200,false , true);
	}
}