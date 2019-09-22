#include "stdafx.h"
#include "ImageSet.h"

namespace GUI
{
	ImageSet::ImageSet()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	ImageSet::~ImageSet()
	{

	}
	
	//////////////////////////////////////////////////////////////////////////
	bool ImageSet::AddSource(int nId,const Source& src)
	{
		if(_Has<int,Source,SrcSet>(nId,mSources) == false)
		{
			mSources.insert( make_pair(nId,src) );
			return true;
		}
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ImageSet::AddImage(const wchar_t* name,const Image& img)
	{
		if(_Has<wstring,Image,ImgSet>(name,mImages) == false)
		{
			mImages.insert( make_pair(name,img) );
			return true;
		}
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
// 	bool ImageSet::Find(const wchar_t* name,Data& result) const
// 	{
//  		const Image* pImg = 0;
//  		if(pImg = _Find<wstring,Image,ImgSet>(name,mImages))
//  		{
//  			const Source* pSrc = _Find<int,Source,SrcSet>(pImg->nSourceId,mSources);
// 			if(pSrc)
// 			{
// 				result.path = pSrc->path.c_str();
// 				result.rect = pImg->rect;
// 				return true;
// 			} 			
//  		}
// 		return false;
// 	}
	//////////////////////////////////////////////////////////////////////////
	void ImageSet::Finish()
	{
		ImgSet::iterator imgIt = mImages.begin();
		for ( ;imgIt !=  mImages.end() ; ++imgIt )
		{
			SrcSet::iterator srcIt(mSources.find(imgIt->second.nSourceId));			
			if(srcIt != mSources.end())
			{
				Data d;
				d.path = srcIt->second.path.c_str();
				d.rect = imgIt->second.rect;
				mDatas.insert( make_pair(imgIt->first,d) );			
			}										
		}
	}
	//////////////////////////////////////////////////////////////////////////
	const ImageSet::Data* ImageSet::Find(const wchar_t* name) const
	{
		DataSet::const_iterator pos(mDatas.find(name));
		if( pos == mDatas.end() )
		{
			return 0;
		}
		return &pos->second;
	}
}