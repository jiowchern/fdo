#ifndef _COMMAND_H_
#define _COMMAND_H_
#include "stdafx.h"

//�ݩ�std::map<T>��find
template<typename T , typename K >
typename T::iterator MapFind(T &date , const K &key)
{
	//�`�N��ѼƨS���ǤJconst�C�ҥH��iterator�Ӥ��Oconst_iterator
	typename T::iterator iter ;

	iter = date.find(key) ;

	return iter ;

}
//�ݩ�std::vector<T> �� std::list<T>�� findif(start , end , boost::apply_visitor(K)) 
template<typename T , typename K >
typename T::iterator ArrayFindifWithVisitor( T &date , K &key)
{
	typename T::iterator iter ;

	iter = std::find_if( date.begin() , date.end() , boost::apply_visitor(key) ) ;

	return iter ;
}

/////		DX��Com����覡
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