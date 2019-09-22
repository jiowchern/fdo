#include "stdafx.h"
#include "GameCommand.h"

namespace GameCommand
{
	//////////////////////////////////////////////////////////////////////////
	Command::Command(CommandHandle hnd,ICommandFuncSlotPtr pSlot,const ParamSet& params) 
		: mCommandShell(pSlot) , mParams(params)
	{

	}
	//////////////////////////////////////////////////////////////////////////
	Command::~Command()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void Command::Run()
	{
		mCommandShell.Run(mParams);
	}
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	CommandLight::CommandLight(CommandHandle hnd,ICommandFuncSlotPtr pSlot)
		: mCommandShell(pSlot)
	{

	}
	//////////////////////////////////////////////////////////////////////////
	CommandLight::~CommandLight()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void CommandLight::Run(const ParamSet& params)
	{
		mCommandShell.Run(params);
	}
	//////////////////////////////////////////////////////////////////////////
	void CommandLight::RunString(const ParamSet& params)
	{
		mCommandShell.RunString(params);
	}
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	CommandShell::CommandShell(ICommandFuncSlotPtr pSlot) : mpCommandFunc(pSlot)
	{

	}
	//////////////////////////////////////////////////////////////////////////
	CommandShell::~CommandShell()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void CommandShell::Run(const ParamSet& params) 
	{	
		try
		{
			if(mpCommandFunc)
				mpCommandFunc->Run(params);

			
		}
		catch(...)
		{

		}
	}
	//////////////////////////////////////////////////////////////////////////
	void CommandShell::RunString(const ParamSet& strparams) 
	{
		try
		{
			ParamSet params;			
			params.resize(strparams.size());
			if(mpCommandFunc)
			{
				if(mpCommandFunc->String2Params(strparams , params))
				{
					mpCommandFunc->Run(params);
				}
				
			}

		}
		catch(...)
		{

		}
	}
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	Factory::Factory()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	Factory::~Factory()
	{
// 		_FOREACH(CommandSlotSet ,mSlots,it)
// 		{
// 			delete it->second;
// 		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Factory::Recycle(CommandHandle hnd)
	{
		mRes.Destory(hnd);		
		mLightRes.Destory(hnd);
	}
	//////////////////////////////////////////////////////////////////////////
	CommandHandle Factory::MakeString(const char16* cmdname,const ParamSet& strings)
	{
		

		ICommandFuncSlotPtr pSlot = _GetCommandFunction(cmdname);
		if(pSlot && pSlot->GetParamSize() == strings.size())
		{
			ParamSet params;
			params.resize(pSlot->GetParamSize());
			if(pSlot->String2Params(strings,params))
				return mRes.Create(pSlot,params);
		}
		if( pSlot )
		{
			assert(pSlot->GetParamSize() == strings.size() && "Command把计计秖ぃ");
		}
		return CommandHandle();
	}

	//////////////////////////////////////////////////////////////////////////
	CommandHandle Factory::Make(const char16* cmdname,ParamSet& params)
	{
		ICommandFuncSlotPtr pSlot = _GetCommandFunction(cmdname);
		if(pSlot && pSlot->GetParamSize() <= (sint)params.size())
		{
			return mRes.Create(pSlot,params);
		}
		if( pSlot )
		{
			assert(pSlot->GetParamSize() <= params.size() && "Command把计计秖ぃì");
		}
		return CommandHandle();
	}
	//////////////////////////////////////////////////////////////////////////
	void Factory::Unregister(const char16* cmdname)
	{
		mSlots.erase(cmdname);
	}
	//////////////////////////////////////////////////////////////////////////
	void Factory::_AddCommand(const char16* cmdname , ICommandFuncSlotPtr pCmd)
	{
		mSlots.insert( make_pair(cmdname , pCmd) );
	}
	//////////////////////////////////////////////////////////////////////////
	ICommandFuncSlotPtr Factory::_GetCommandFunction(const char16* cmdname) const 
	{
		if(cmdname)
		{
			CommandSlotSet::const_iterator it = mSlots.find(cmdname);
			if(it != mSlots.end())
			{
				return it->second;
			}
		}
		
		return ICommandFuncSlotPtr();
	}
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	Executer::Executer(Factory& fac) : mFac(fac)
	{

	}
	//////////////////////////////////////////////////////////////////////////
	Executer::~Executer()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void Executer::RunLight	(CommandHandle hnd,const ParamSet& params)
	{
		_RunLight(hnd , params);
	}
	//////////////////////////////////////////////////////////////////////////
	void Executer::RunLightString	(CommandHandle hnd	,const char16* param1
														,const char16* param2
														,const char16* param3
														,const char16* param4
														,const char16* param5)
	{
		ParamSet params;
		if(param1)
			params.push_back(std::wstring(param1));
		if(param2)
			params.push_back(std::wstring(param2));
		if(param3)
			params.push_back(std::wstring(param3));
		if(param4)
			params.push_back(std::wstring(param4));
		if(param5)
			params.push_back(std::wstring(param5));

		try
		{
			Factory::CommandLightResouece& res = mFac._GetResoueceLight();
			res.Execute<void,const ParamSet&>(hnd,&CommandLight::RunString,params);
		}
		catch (std::exception const &)
		{
		}
		catch (...)
		{

		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Executer::_RunLight(CommandHandle hnd,const ParamSet& params)
	{
		try
		{
			Factory::CommandLightResouece& res = mFac._GetResoueceLight();
			res.Execute<void,const ParamSet&>(hnd,&CommandLight::Run,params);
		}
		catch (std::exception const &)
		{
		}
		catch (...)
		{

		}

	}
	//////////////////////////////////////////////////////////////////////////
	void Executer::Run(CommandHandle hnd)
	{
		try
		{
			Factory::CommandResouece& res = mFac._GetResouece();
			res.Execute<void>(hnd,&Command::Run);
		}
		catch (std::exception const &)
		{
		}
		catch (...)
		{
			
		}
		
		
	}
		
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	struct Test
	{
		void Test0(){};
		void Test1(uint i){};
		void Test2(uint i,uint i1,uint i2,uint i3,uint i4){};
		void Test3(const Test& i,uint i1){};
	};
	void fnTest()
	{
 		Test t;
 		Factory		factory;
 		Executer	executer(factory);
		factory.Register<Test,void>(L"test0",&t,&Test::Test0);
		factory.Register<Test,void,uint>(L"test1",&t,&Test::Test1);
 		
 		ParamSet params;
 		params.resize(10);

 		params[0] = 1;
 		params[1] = 0.4f;
		params[2] = std::wstring(L"gergerg");

 		CommandHandle hnd = factory.Make(L"test0",params);
		CommandHandle hnd1 = factory.Make(L"test1",params);

 		executer.Run(hnd);
		executer.Run(hnd1);

		CommandHandle ltghthnd3 = factory.MakeLight<Test,void,uint,uint,uint,uint,uint>(&t,&Test::Test2);

		executer.RunLight(ltghthnd3 , 1 , 2 ,3 ,4, 5);		

		factory.Recycle(ltghthnd3 );

		CommandHandle ltghthnd4 = factory.MakeLight<Test,void,const Test&,uint>(&t,&Test::Test3);

		executer.RunLight(ltghthnd4 , 1 , 2 ,3 ,4, 5);		

		factory.Recycle(ltghthnd4);

		CommandHandle ltghthnd5 = factory.MakeLight<void>(&fnTest);

		executer.RunLight(ltghthnd5 , 1 , 2 ,3 ,4, 5);		

		factory.Recycle(ltghthnd5);

	}
	
}
namespace FDO
{
	namespace CommandSystem
	{
		//////////////////////////////////////////////////////////////////////////
		GameCommand::Executer&	fnGetExecuter()
		{
			Manager& cmdMgr = Manager::GetSingleton();
			return cmdMgr.mExr;
		}
		//////////////////////////////////////////////////////////////////////////
		GameCommand::Factory&	fnGetFactory()
		{
			Manager& cmdMgr = Manager::GetSingleton();
			return cmdMgr.mFac;
		}
	}
}