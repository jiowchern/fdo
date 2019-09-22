#ifndef _GameCommand_20091212pm0316_
#define _GameCommand_20091212pm0316_
#include "boost/any.hpp"
#include "LokiLib.h"
namespace GameCommand
{
	typedef boost::any Param ;
	typedef std::vector<boost::any> ParamSet;
	//////////////////////////////////////////////////////////////////////////
	template<typename T>
	struct TAssignParam
	{
		static T CnvOut(const Param& val)
		{			
			return boost::any_cast<T>(val);
		}

		static T CnvIn(T val)
		{
			return val;
		}
	};

	template<typename T>
	struct TAssignParam<T&>
	{
		static T& CnvOut(const Param& val)
		{			
			return *(boost::any_cast<T*>(val));
		}

		static T* CnvIn(T& val)
		{
			return &val;
		}
	};
	//////////////////////////////////////////////////////////////////////////
	struct ICommandFuncSlot
	{
		virtual ~ICommandFuncSlot	(){};
		virtual sint GetParamSize	() const	= 0;
		virtual void Run			(const ParamSet& params)			= 0;
		virtual bool String2Params	(const ParamSet& strings,ParamSet& outparams)	= 0;
	};
	
	typedef boost::shared_ptr<ICommandFuncSlot> ICommandFuncSlotPtr;
	//////////////////////////////////////////////////////////////////////////
	typedef TL::NullClass TNull;
	struct Empty{	};

	template<typename TP1 ,typename TP2 ,typename TP3 ,typename TP4 ,typename TP5 ,typename TPEnd = TNull>
	struct TParamListCount
	{
		enum
		{
			COUNT = typename TParamListCount<TP2,TP3,TP4,TP5,TPEnd>::COUNT + 1
		};
	};

	template<>
	struct TParamListCount<TNull,TNull,TNull,TNull,TNull,TNull>
	{
		enum
		{
			COUNT = 0
		};
	};

	template<>
	struct TParamListCount<Empty,Empty,Empty,Empty,Empty,Empty>
	{
		enum
		{
			COUNT = 0
		};
	};

	template<sint nParamSize,typename TReturn,typename TP1 ,typename TP2 ,typename TP3 ,typename TP4 ,typename TP5 >
	struct TFunctionType;

	template<typename TReturn,typename TP1 ,typename TP2 ,typename TP3 ,typename TP4 ,typename TP5 >
	struct TFunctionType<0,TReturn,TP1,TP2,TP3,TP4,TP5>
	{
		typedef TReturn (*FunctionType)();
	};

	template<typename TReturn,typename TP1 ,typename TP2 ,typename TP3 ,typename TP4 ,typename TP5 >
	struct TFunctionType<1,TReturn,TP1,TP2,TP3,TP4,TP5>
	{
		typedef TReturn (*FunctionType)(TP1);
	};

	template<typename TReturn,typename TP1 ,typename TP2 ,typename TP3 ,typename TP4 ,typename TP5 >
	struct TFunctionType<2,TReturn,TP1,TP2,TP3,TP4,TP5>
	{
		typedef TReturn (*FunctionType)(TP1,TP2);
	};

	template<typename TReturn,typename TP1 ,typename TP2 ,typename TP3 ,typename TP4 ,typename TP5 >
	struct TFunctionType<3,TReturn,TP1,TP2,TP3,TP4,TP5>
	{
		typedef TReturn (*FunctionType)(TP1,TP2,TP3);
	};

	template<typename TReturn,typename TP1 ,typename TP2 ,typename TP3 ,typename TP4 ,typename TP5 >
	struct TFunctionType<4,TReturn,TP1,TP2,TP3,TP4,TP5>
	{
		typedef TReturn (*FunctionType)(TP1,TP2,TP3,TP4);
	};

	template<typename TReturn,typename TP1 ,typename TP2 ,typename TP3 ,typename TP4 ,typename TP5 >
	struct TFunctionType<5,TReturn,TP1,TP2,TP3,TP4,TP5>
	{
		typedef TReturn (*FunctionType)(TP1,TP2,TP3,TP4,TP5);
	};
	template<typename TReturn,typename TP1 = TNull,typename TP2 = TNull,typename TP3 = TNull,typename TP4 = TNull,typename TP5 = TNull>
	struct TCommandFunctionSlot : public ICommandFuncSlot
	{
		typedef std::wstring	String;
		enum	{	PARAMSIZE = typename TParamListCount<TP1,TP2,TP3,TP4,TP5>::COUNT	};
		typedef TFunctionType<PARAMSIZE,TReturn,TP1,TP2,TP3,TP4,TP5> StaticFunctionType;
		typedef typename StaticFunctionType::FunctionType TFnc;
		TCommandFunctionSlot	(TFnc pfnc) 
			: mFn(pfnc)		{		}
		virtual ~TCommandFunctionSlot	(){}

		virtual sint GetParamSize	() const	{return PARAMSIZE;};
		virtual void Run			(const ParamSet& params)			{_Run<PARAMSIZE>(params);	}
		
		virtual bool String2Params	(const ParamSet& strings,ParamSet& outparams)	
		{

			if(strings.size() >= 1)
				_String2Value<TP1>(outparams[0],strings[0]);
			if(strings.size() >= 2)
				_String2Value<TP2>(outparams[1],strings[1]);
			if(strings.size() >= 3)
				_String2Value<TP3>(outparams[2],strings[2]);
			if(strings.size() >= 4)
				_String2Value<TP4>(outparams[3],strings[3]);
			if(strings.size() >= 5)
				_String2Value<TP5>(outparams[4],strings[4]);
			return true;

		}
	private:
		template<sint parsize>	void _Run(const ParamSet& params)	{	}
		template<>	void _Run<0>(const ParamSet& params) {	(*mFn)();	}
		
		template<>	void _Run<1>(const ParamSet& params) {	(*mFn)(TAssignParam<TP1>::CnvOut(params[0]));	}
		template<>	void _Run<2>(const ParamSet& params) {	(*mFn)(TAssignParam<TP1>::CnvOut(params[0]),TAssignParam<TP2>::CnvOut(params[1]));	}
		template<>	void _Run<3>(const ParamSet& params) {	(*mFn)(TAssignParam<TP1>::CnvOut(params[0]),TAssignParam<TP2>::CnvOut(params[1]),TAssignParam<TP3>::CnvOut(params[2]));	}
		template<>	void _Run<4>(const ParamSet& params) {	(*mFn)(TAssignParam<TP1>::CnvOut(params[0]),TAssignParam<TP2>::CnvOut(params[1]),TAssignParam<TP3>::CnvOut(params[2]),TAssignParam<TP4>::CnvOut(params[3]));	}
		template<>	void _Run<5>(const ParamSet& params) {	(*mFn)(TAssignParam<TP1>::CnvOut(params[0]),TAssignParam<TP2>::CnvOut(params[1]),TAssignParam<TP3>::CnvOut(params[2]),TAssignParam<TP4>::CnvOut(params[3]),TAssignParam<TP5>::CnvOut(params[4]));	}

		template<typename T>	void _String2Value(Param& params1,const Param& params2)	{ params1 = boost::any_cast<String>(params2);}
		template<>				void _String2Value<TNull>(Param& params1,const Param& params2)	{	}
		template<>				void _String2Value<uint>(Param& params1,const Param& params2)	{params1 = uint(_wtoi(boost::any_cast<String>(params2).c_str()));	}
		template<>				void _String2Value<sint>(Param& params1,const Param& params2)	{params1 = _wtoi(boost::any_cast<String>(params2).c_str());	}		
		template<>				void _String2Value<float>(Param& params1,const Param& params2)	{params1 = (float)_wtof(boost::any_cast<String>(params2).c_str());	}				
		template<>				void _String2Value<double>(Param& params1,const Param& params2)	{params1 = _wtof(boost::any_cast<String>(params2).c_str());	}				
		TFnc	mFn;
	};
	
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	template<sint nParamSize,typename TObj,typename TReturn,typename TP1 ,typename TP2 ,typename TP3 ,typename TP4 ,typename TP5 >
	struct TQueryFunction;

	template<typename TObj,typename TReturn,typename TP1 ,typename TP2 ,typename TP3 ,typename TP4 ,typename TP5 >
	struct TQueryFunction<0,TObj,TReturn,TP1,TP2,TP3,TP4,TP5>
	{
		typedef TReturn (TObj::*FunctionType)();
	};

	template<typename TObj,typename TReturn,typename TP1 ,typename TP2 ,typename TP3 ,typename TP4 ,typename TP5 >
	struct TQueryFunction<1,TObj,TReturn,TP1,TP2,TP3,TP4,TP5>
	{
		typedef TReturn (TObj::*FunctionType)(TP1);
	};

	template<typename TObj,typename TReturn,typename TP1 ,typename TP2 ,typename TP3 ,typename TP4 ,typename TP5 >
	struct TQueryFunction<2,TObj,TReturn,TP1,TP2,TP3,TP4,TP5>
	{
		typedef TReturn (TObj::*FunctionType)(TP1,TP2);
	};

	template<typename TObj,typename TReturn,typename TP1 ,typename TP2 ,typename TP3 ,typename TP4 ,typename TP5 >
	struct TQueryFunction<3,TObj,TReturn,TP1,TP2,TP3,TP4,TP5>
	{
		typedef TReturn (TObj::*FunctionType)(TP1,TP2,TP3);
	};

	template<typename TObj,typename TReturn,typename TP1 ,typename TP2 ,typename TP3 ,typename TP4 ,typename TP5 >
	struct TQueryFunction<4,TObj,TReturn,TP1,TP2,TP3,TP4,TP5>
	{
		typedef TReturn (TObj::*FunctionType)(TP1,TP2,TP3,TP4);
	};

	template<typename TObj,typename TReturn,typename TP1 ,typename TP2 ,typename TP3 ,typename TP4 ,typename TP5 >
	struct TQueryFunction<5,TObj,TReturn,TP1,TP2,TP3,TP4,TP5>
	{
		typedef TReturn (TObj::*FunctionType)(TP1,TP2,TP3,TP4,TP5);
	};

	template<typename TObj,typename TReturn,typename TP1 = TNull,typename TP2 = TNull,typename TP3 = TNull,typename TP4 = TNull,typename TP5 = TNull>
	struct TCommandFuncSlot : public ICommandFuncSlot
	{
		typedef std::wstring	String;
		enum	{	PARAMSIZE = typename TParamListCount<TP1,TP2,TP3,TP4,TP5>::COUNT	};
		typedef TQueryFunction<PARAMSIZE,TObj,TReturn,TP1,TP2,TP3,TP4,TP5> QueryFunctionType;

//		
		typedef typename QueryFunctionType::FunctionType TFnc;
		
		TCommandFuncSlot	(TObj* pObj,TFnc pfnc) 
			: mpObj(pObj) ,mpFnc(pfnc)		{		}
		~TCommandFuncSlot	(){}

		virtual sint GetParamSize	() const	{return PARAMSIZE;};
		virtual void Run			(const ParamSet& params)			
		{
			_Run<PARAMSIZE>(params);
		}
		virtual bool String2Params	(const ParamSet& strings,ParamSet& outparams)	
		{
			
			if(strings.size() >= 1)
				_String2Value<TP1>(outparams[0],strings[0]);
			if(strings.size() >= 2)
				_String2Value<TP2>(outparams[1],strings[1]);
			if(strings.size() >= 3)
				_String2Value<TP3>(outparams[2],strings[2]);
			if(strings.size() >= 4)
				_String2Value<TP4>(outparams[3],strings[3]);
			if(strings.size() >= 5)
				_String2Value<TP5>(outparams[4],strings[4]);
			return true;
	
		}

		
	private:

		TObj*	mpObj;
		TFnc	mpFnc;

		template<sint parsize>	void _Run(const ParamSet& params)	{	}
		template<>	void _Run<0>(const ParamSet& params) {	(mpObj->*mpFnc)();	}
		template<>	void _Run<1>(const ParamSet& params) {	(mpObj->*mpFnc)(TAssignParam<TP1>::CnvOut(params[0]));	}
		template<>	void _Run<2>(const ParamSet& params) {	(mpObj->*mpFnc)(TAssignParam<TP1>::CnvOut(params[0]),TAssignParam<TP2>::CnvOut(params[1]));	}
		template<>	void _Run<3>(const ParamSet& params) {	(mpObj->*mpFnc)(TAssignParam<TP1>::CnvOut(params[0]),TAssignParam<TP2>::CnvOut(params[1]),TAssignParam<TP3>::CnvOut(params[2]));	}
		template<>	void _Run<4>(const ParamSet& params) {	(mpObj->*mpFnc)(TAssignParam<TP1>::CnvOut(params[0]),TAssignParam<TP2>::CnvOut(params[1]),TAssignParam<TP3>::CnvOut(params[2]),TAssignParam<TP4>::CnvOut(params[3]));	}
		template<>	void _Run<5>(const ParamSet& params) {	(mpObj->*mpFnc)(TAssignParam<TP1>::CnvOut(params[0]),TAssignParam<TP2>::CnvOut(params[1]),TAssignParam<TP3>::CnvOut(params[2]),TAssignParam<TP4>::CnvOut(params[3]),TAssignParam<TP5>::CnvOut(params[4]));	}
			
		template<typename T>	void _String2Value(Param& params1,const Param& params2)	{ params1 = boost::any_cast<String>(params2);}
		template<>				void _String2Value<TNull>(Param& params1,const Param& params2)	{	}
		template<>				void _String2Value<uint>(Param& params1,const Param& params2)	{params1 = uint(_wtoi(boost::any_cast<String>(params2).c_str()));	}
		template<>				void _String2Value<sint>(Param& params1,const Param& params2)	{params1 = _wtoi(boost::any_cast<String>(params2).c_str());	}		
		template<>				void _String2Value<float>(Param& params1,const Param& params2)	{params1 = (float)_wtof(boost::any_cast<String>(params2).c_str());	}				
		template<>				void _String2Value<double>(Param& params1,const Param& params2)	{params1 = _wtof(boost::any_cast<String>(params2).c_str());	}				
	};
	
	//////////////////////////////////////////////////////////////////////////
	struct CommandShell 
	{		
						CommandShell	(ICommandFuncSlotPtr pSlot);
						~CommandShell	();
		void			Run				(const ParamSet& params) ;		
		void			RunString		(const ParamSet& params) ;		
	private:
		ICommandFuncSlotPtr mpCommandFunc;
		
	};
	//////////////////////////////////////////////////////////////////////////
	typedef ResoueceSystem::HANDLE CommandHandle;
	struct Command
	{
		
			Command		(CommandHandle hnd,ICommandFuncSlotPtr pSlot,const ParamSet& params);
			~Command	();
		
		void	Run		();
	private:
		ParamSet		mParams;
		CommandShell	mCommandShell;
	};

	//////////////////////////////////////////////////////////////////////////
	// 輕量化版
	struct CommandLight
	{

		CommandLight			(CommandHandle hnd,ICommandFuncSlotPtr pSlot);
		~CommandLight			();

		void	Run				(const ParamSet& params);
		void	RunString		(const ParamSet& params);
	private:		
		CommandShell	mCommandShell;
	};

	
	//////////////////////////////////////////////////////////////////////////
	

	struct Factory 
	{
		

		friend struct Executer;
						Factory		();
						~Factory	();

		template<typename TReturn >
		CommandHandle	MakeLight(typename TFunctionType<0,TReturn,TNull,TNull,TNull,TNull,TNull>::FunctionType fnc)
		{
			return MakeLight<TReturn,TNull,TNull,TNull,TNull,TNull>(fnc);
		}

		template<typename TReturn ,typename TP1 >
		CommandHandle	MakeLight(typename TFunctionType<1,TReturn,TP1,TNull,TNull,TNull,TNull>::FunctionType fnc)
		{
			return MakeLight<TReturn,TP1,TNull,TNull,TNull,TNull>(fnc);
		}

		template<typename TReturn ,typename TP1 ,typename TP2 >
		CommandHandle	MakeLight(typename TFunctionType<2,TReturn,TP1,TP2,TNull,TNull,TNull>::FunctionType fnc)
		{
			return MakeLight<TReturn,TP1,TP2,TNull,TNull,TNull>(fnc);
		}

		template<typename TReturn ,typename TP1 ,typename TP2 ,typename TP3 >
		CommandHandle	MakeLight(typename TFunctionType<3,TReturn,TP1,TP2,TP3,TNull,TNull>::FunctionType fnc)
		{
			return MakeLight<TReturn,TP1,TP2,TP3,TNull,TNull>(fnc);
		}

		template<typename TReturn ,typename TP1 ,typename TP2 ,typename TP3 ,typename TP4 >
		CommandHandle	MakeLight(typename TFunctionType<4,TReturn,TP1,TP2,TP3,TP4,TNull>::FunctionType fnc)
		{
			return MakeLight<TReturn,TP1,TP2,TP3,TP4,TNull>(fnc);
		}

		template<typename typename TReturn ,typename TP1 ,typename TP2 ,typename TP3 ,typename TP4 ,typename TP5 >
		CommandHandle	MakeLight(typename TFunctionType<TParamListCount<TP1,TP2,TP3,TP4,TP5>::COUNT,TReturn,TP1,TP2,TP3,TP4,TP5>::FunctionType fnc)
		{
			ICommandFuncSlotPtr pSlot(new TCommandFunctionSlot<TReturn,TP1,TP2,TP3,TP4,TP5>(fnc));
			return GetResoueceLight().Create(pSlot);
		}

		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		template<typename TObj,typename TReturn >
		CommandHandle	MakeLight(TObj* pObj,typename TQueryFunction<0,TObj,TReturn,TNull,TNull,TNull,TNull,TNull>::FunctionType fnc)
		{
			return MakeLight<TObj,TReturn,TNull,TNull,TNull,TNull,TNull>(pObj,fnc);
		}

		template<typename TObj,typename TReturn ,typename TP1 >
		CommandHandle	MakeLight(TObj* pObj,typename TQueryFunction<1,TObj,TReturn,TP1,TNull,TNull,TNull,TNull>::FunctionType fnc)
		{
			return MakeLight<TObj,TReturn,TP1,TNull,TNull,TNull,TNull>(pObj,fnc);
		}

		template<typename TObj,typename TReturn ,typename TP1 ,typename TP2 >
		CommandHandle	MakeLight(TObj* pObj,typename TQueryFunction<2,TObj,TReturn,TP1,TP2,TNull,TNull,TNull>::FunctionType fnc)
		{
			return MakeLight<TObj,TReturn,TP1,TP2,TNull,TNull,TNull>(pObj,fnc);
		}

		template<typename TObj,typename TReturn ,typename TP1 ,typename TP2 ,typename TP3 >
		CommandHandle	MakeLight(TObj* pObj,typename TQueryFunction<3,TObj,TReturn,TP1,TP2,TP3,TNull,TNull>::FunctionType fnc)
		{
			return MakeLight<TObj,TReturn,TP1,TP2,TP3,TNull,TNull>(pObj,fnc);
		}

		template<typename TObj,typename TReturn ,typename TP1 ,typename TP2 ,typename TP3 ,typename TP4 >
		CommandHandle	MakeLight(TObj* pObj,typename TQueryFunction<4,TObj,TReturn,TP1,TP2,TP3,TP4,TNull>::FunctionType fnc)
		{
			return MakeLight<TObj,TReturn,TP1,TP2,TP3,TP4,TNull>(pObj,fnc);
		}
		
		template<typename TObj,typename TReturn ,typename TP1 ,typename TP2 ,typename TP3 ,typename TP4 ,typename TP5 >
		CommandHandle	MakeLight(TObj* pObj,typename TQueryFunction<TParamListCount<TP1,TP2,TP3,TP4,TP5>::COUNT,TObj,TReturn,TP1,TP2,TP3,TP4,TP5>::FunctionType fnc)
		{
			ICommandFuncSlotPtr pSlot(new TCommandFuncSlot<TObj,TReturn,TP1,TP2,TP3,TP4,TP5>(pObj,fnc));
			return GetResoueceLight().Create(pSlot);
		}
		
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////

		CommandHandle	Make		(const char16* cmdname,ParamSet& params);
		CommandHandle	MakeString	(const char16* cmdname,const ParamSet& params);
		void			Recycle		(CommandHandle hnd);
		
		template<typename TObj,typename TReturn>
		void			Register	(const char16* cmdname,TObj* pObj,typename TQueryFunction<TParamListCount<TNull,TNull,TNull,TNull,TNull>::COUNT,TObj,TReturn,TNull,TNull,TNull,TNull,TNull>::FunctionType fnc)
		{
			Register<TObj,TReturn,TNull,TNull,TNull,TNull,TNull>(cmdname,pObj,fnc);
		}

		template<typename TObj,typename TReturn,typename TP1>
		void			Register	(const char16* cmdname,TObj* pObj,typename TQueryFunction<TParamListCount<TP1,TNull,TNull,TNull,TNull>::COUNT,TObj,TReturn,TP1,TNull,TNull,TNull,TNull>::FunctionType fnc)
		{
			Register<TObj,TReturn,TP1,TNull,TNull,TNull,TNull>(cmdname,pObj,fnc);
		}

		template<typename TObj,typename TReturn,typename TP1,typename TP2>
		void			Register	(const char16* cmdname,TObj* pObj,typename TQueryFunction<TParamListCount<TP1,TP2,TNull,TNull,TNull>::COUNT,TObj,TReturn,TP1,TP2,TNull,TNull,TNull>::FunctionType fnc)
		{
			Register<TObj,TReturn,TP1,TP2,TNull,TNull,TNull>(cmdname,pObj,fnc);
		}

		template<typename TObj,typename TReturn,typename TP1,typename TP2,typename TP3>
		void			Register	(const char16* cmdname,TObj* pObj,typename TQueryFunction<TParamListCount<TP1,TP2,TP3,TNull,TNull>::COUNT,TObj,TReturn,TP1,TP2,TP3,TNull,TNull>::FunctionType fnc)
		{
			Register<TObj,TReturn,TP1,TP2,TP3,TNull,TNull>(cmdname,pObj,fnc);
		}

		template<typename TObj,typename TReturn,typename TP1,typename TP2,typename TP3,typename TP4>
		void			Register	(const char16* cmdname,TObj* pObj,typename TQueryFunction<TParamListCount<TP1,TP2,TP3,TP4,TNull>::COUNT,TObj,TReturn,TP1,TP2,TP3,TP4,TNull>::FunctionType fnc)
		{
			Register<TObj,TReturn,TP1,TP2,TP3,TP4,TNull>(cmdname,pObj,fnc);
		}

		template<typename TObj,typename TReturn,typename TP1 ,typename TP2 ,typename TP3 ,typename TP4 ,typename TP5 >
		void			Register	(const char16* cmdname,TObj* pObj,typename TQueryFunction<TParamListCount<TP1,TP2,TP3,TP4,TP5>::COUNT,TObj,TReturn,TP1,TP2,TP3,TP4,TP5>::FunctionType fnc)
		{
			if(_GetCommandFunction(cmdname) == 0)
			{
				ICommandFuncSlotPtr pSlot(new TCommandFuncSlot<TObj,TReturn,TP1,TP2,TP3,TP4,TP5>(pObj,fnc));
				_AddCommand(cmdname , pSlot) ;
			}	
			else
			{
				assert(0 && "重複的註冊");
			}
		}

		void			Unregister	(const char16* cmdname);

		typedef ResoueceSystem::TResource<CommandLight,ObjFactory> CommandLightResouece;		
		inline	CommandLightResouece& GetResoueceLight() {return mLightRes;}
	private:
		typedef ResoueceSystem::TResource<Command,ObjFactory> CommandResouece;		
		
		CommandResouece			mRes;
		CommandLightResouece	mLightRes;

		inline	CommandResouece& _GetResouece() {return mRes;}
		inline	CommandLightResouece& _GetResoueceLight() {return mLightRes;}
		
		typedef std::map<std::wstring,ICommandFuncSlotPtr> CommandSlotSet;
		CommandSlotSet	mSlots;
		ICommandFuncSlotPtr		_GetCommandFunction	(const char16* cmdname) const ;
		void					_AddCommand			(const char16* cmdname , ICommandFuncSlotPtr pCmd);
	};
	//////////////////////////////////////////////////////////////////////////
	template<typename TObj,typename TReturn = void ,typename TP1 = TNull,typename TP2 = TNull,typename TP3 = TNull,typename TP4 = TNull,typename TP5 = TNull>
	struct TLightCommandMaker
	{
		static CommandHandle	Make(Factory& fac,TObj* pObj,typename TQueryFunction<TParamListCount<TP1,TP2,TP3,TP4,TP5>::COUNT,TObj,TReturn,TP1,TP2,TP3,TP4,TP5>::FunctionType fnc)
		{
			ICommandFuncSlotPtr pSlot (new TCommandFuncSlot<TObj,TReturn,TP1,TP2,TP3,TP4,TP5>(pObj,fnc));
			return fac.GetResoueceLight().Create(pSlot);
		}
	};
	

	
	//////////////////////////////////////////////////////////////////////////
	struct Executer
	{
		
				Executer	(Factory& fac);
				~Executer	();
		void	Run			(CommandHandle hnd);
		
		
		
		void	RunLight	(CommandHandle hnd)
		{
			RunLight<Empty,Empty,Empty,Empty,Empty>(hnd,Empty(),Empty(),Empty(),Empty(),Empty());
		}

		template<typename TP1 >
		void	RunLight	(CommandHandle hnd,TP1 p1)
		{
			RunLight<TP1,Empty,Empty,Empty,Empty>(hnd,p1,Empty(),Empty(),Empty(),Empty());
		}

		template<typename TP1 ,typename TP2 >
		void	RunLight	(CommandHandle hnd,TP1 p1,TP2 p2)
		{
			RunLight<TP1,TP2,Empty,Empty,Empty>(hnd,p1,p2,Empty(),Empty(),Empty());
		}

		template<typename TP1 ,typename TP2 ,typename TP3 >
		void	RunLight	(CommandHandle hnd,TP1 p1,TP2 p2,TP3 p3)
		{
			RunLight<TP1,TP2,TP3,Empty,Empty>(hnd,p1,p2,p3,Empty(),Empty());
		}

		template<typename TP1 ,typename TP2 ,typename TP3 ,typename TP4 >
		void	RunLight	(CommandHandle hnd,TP1 p1,TP2 p2,TP3 p3,TP4 p4)
		{
			RunLight<TP1,TP2,TP3,TP4,Empty>(hnd,p1,p2,p3,p4,Empty());
		}
		
		template<typename TP1 ,typename TP2 ,typename TP3 ,typename TP4 ,typename TP5 >
		void	RunLight	(CommandHandle hnd,TP1 p1,TP2 p2,TP3 p3,TP4 p4,TP5 p5)
		{
			ParamSet params;
			const uint nParamCount = TParamListCount<TP1,TP2,TP3,TP4,TP5,Empty>::COUNT;
			params.resize(nParamCount);

			if(nParamCount >= 1)
				params[0] = TAssignParam<TP1>::CnvIn(p1);				
			if(nParamCount >= 2)
				params[1] = TAssignParam<TP2>::CnvIn(p2);				
			if(nParamCount >= 3)
				params[2] = TAssignParam<TP3>::CnvIn(p3);				
			if(nParamCount >= 4)
				params[3] = TAssignParam<TP4>::CnvIn(p4);				
			if(nParamCount >= 5)
				params[4] = TAssignParam<TP5>::CnvIn(p5);				

			_RunLight(hnd , params);
		}

		void	RunLight		(CommandHandle hnd,const ParamSet& params);

		
		void	RunLightString	(CommandHandle hnd	,const char16* param1 = 0
													,const char16* param2 = 0
													,const char16* param3 = 0
													,const char16* param4 = 0
													,const char16* param5 = 0);

	private:
		void	_RunLight	(CommandHandle hnd,const ParamSet& params);
		
		Factory& mFac;
	};
	//////////////////////////////////////////////////////////////////////////

	


};

namespace FDO
{
	namespace CommandSystem
	{
		typedef ::GameCommand::CommandHandle	Handle;
		typedef ::GameCommand::Factory		Factory;
		typedef ::GameCommand::Executer		Executer;
		typedef ::GameCommand::ParamSet		ParamSet;
		//////////////////////////////////////////////////////////////////////////
		// 
		GameCommand::Executer&	fnGetExecuter	();
		GameCommand::Factory&	fnGetFactory	();
		//////////////////////////////////////////////////////////////////////////
		void					_RunCommand		(uint nCmd , CommandSystem::ParamSet& ps);
		void					RunCommand		(uint nCmd);

		template<typename T1>
		void					RunCommand		(uint nCmd ,T1 p1)
		{
			CommandSystem::ParamSet ps;
			ps.resize(1);
			ps[0] = p1;
			_RunCommand(nCmd , ps);
		}

		template<typename T1 , typename T2>
		void					RunCommand		(uint nCmd ,T1 p1 ,T2 p2)
		{
			CommandSystem::ParamSet ps;
			ps.resize(2);
			ps[0] = p1;	ps[1] = p2;
			_RunCommand(nCmd , ps);
		}

		//////////////////////////////////////////////////////////////////////////
		struct Manager : Singleton<Manager>
		{
			friend	GameCommand::Executer&	fnGetExecuter	();
			friend	GameCommand::Factory&	fnGetFactory	();

			Manager		();
			~Manager	();
			void	Initial		();
			void	Release		();
		private:
			GameCommand::Factory	mFac;		
			GameCommand::Executer	mExr;		
		};

		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////

	};
}

#endif