#ifndef _RoleProperty_20091230am1055_
#define _RoleProperty_20091230am1055_

namespace FDO
{
	namespace GameProperty
	{
		typedef sint Value;
		typedef uint Type;
		typedef std::map<Type,Value> Container;
		//////////////////////////////////////////////////////////////////////////

		struct ISet 
		{
			typedef  Container::const_iterator Iterator;

			virtual void	Set		(Type type,Value newValue,bool bForceUpdate= false)		= 0;
			virtual void	Diff	(Type type,Value diffVal,bool bForceUpdate= false)		= 0;
			virtual bool	Get		(Type type,Value &val) const	= 0;
			virtual Value	Get		(Type type)				const	= 0;


			inline Container::const_iterator GetBegin() const 
			{
				return mDatas.begin();
			}

			inline Container::const_iterator GetEnd() const 
			{
				return mDatas.end();
			}

		protected:						


			Container	mDatas;
		};
		typedef ISet*		ISetPtr;
		typedef const ISet*	ISetCPtr;
	}
	typedef GameProperty::ISetPtr PropertyPtr;
}
#endif