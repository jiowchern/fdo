#ifndef _ImageSet_20080818pm0433_
#define _ImageSet_20080818pm0433_
#include <string>
#include <map>
#include "Element.h"
namespace GUI
{
	
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	class ImageSet  
	{
	public:
		struct Image
		{
			int		nSourceId;
			Rect32	rect;
		};
		struct Source
		{
			string	path;
		};
		struct Data
		{
			Rect32		rect;
			const char* path;
		};
	protected:
		typedef std::map<wstring,Image> ImgSet;
		typedef std::map<int,Source>	SrcSet;
		typedef std::map<wstring,Data>	DataSet;

		ImgSet	mImages;
		SrcSet	mSources;
		DataSet mDatas;

		template<class Key,class Val,class MapObj>
		const Val*	_Find	(Key k,const MapObj& obj) const
		{
			if(obj.size())
			{
				MapObj::const_iterator it = obj.find(k);
				if(it != obj.end())
					return &it->second;
			}
			return 0;
		}
		//////////////////////////////////////////////////////////////////////////
		template<class Key,class Val,class MapObj>
		bool	_Has	(Key k,const MapObj& obj) const
		{
			return _Find<Key,Val,MapObj>(k,obj) != 0;
		}
	public:
						ImageSet	();
						~ImageSet	();

		bool			AddSource	(int nId,const Source& src);
		bool			AddImage	(const wchar_t* name,const Image& img);

		
		void			Finish		();
		const Data*		Find		(const wchar_t* name) const;

	};		
	
};
#endif