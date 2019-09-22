#ifndef _ImageSetReader_20080819pm0151_
#define _ImageSetReader_20080819pm0151_
#include "ImageSet.h"
#include "XmlLoader.h"
namespace FDO
{
	class GUIImageSetReader : public XmlLoader 
	{

		//////////////////////////////////////////////////////////////////////////
		// temp data
		struct Temp
		{
			wstring					imagename;
			GUI::ImageSet::Image	img;	
			int						w;
			int						h;
			int						srcid;
			GUI::ImageSet::Source	src;			
			GUI::ImageSet* 			pSet;
		};
		
		Temp	mTemp;
		void	_ClearTempData	();
	public:
				GUIImageSetReader	();
				~GUIImageSetReader	();
		
		void	LoadXML				(const wchar_t* xmlText,GUI::ImageSet* pSet);

		//////////////////////////////////////////////////////////////////////////
		//
		void	XmlStartImage		();
		void	XmlEndImage			();
		void	XmlAttribImage		(const wchar_t* attr,int nAttrLen,const wchar_t* value,int nValLen);		

		void	XmlStartSource		();
		void	XmlEndSource		();
		void	XmlAttribSource		(const wchar_t* attr,int nAttrLen,const wchar_t* value,int nValLen);
	};
}
#endif