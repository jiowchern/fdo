#ifndef _COMMAND_H_
#define _COMMAND_H_
#include "stdafx.h"

//屬於std::map<T>的find
template<typename T , typename K >
typename T::iterator MapFind(T &date , const K &key)
{
	//注意到參數沒有傳入const。所以用iterator而不是const_iterator
	typename T::iterator iter ;

	iter = date.find(key) ;

	return iter ;

}
//屬於std::vector<T> 或 std::list<T>的 findif(start , end , boost::apply_visitor(K)) 
template<typename T , typename K >
typename T::iterator ArrayFindifWithVisitor( T &date , K &key)
{
	typename T::iterator iter ;

	iter = std::find_if( date.begin() , date.end() , boost::apply_visitor(key) ) ;

	return iter ;
}

/////		DX的Com釋放方式
class CD3DRelease
{
public:
	template<typename T>
	void operator()(T t)
	{
		if (t != NULL)
		{
			t->Release() ;
			t = NULL ;
		}
	}
};



#endif