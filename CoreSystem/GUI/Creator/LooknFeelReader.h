#ifndef _LooknFeelReader_20080822pm0323_
#define _LooknFeelReader_20080822pm0323_
#include "LooknFeel.h"
#include "XmlLoader.h"
#include "ImageSet.h"
namespace FDO
{
	class GUILooknFeelReader : public XmlLoader 
	{
	public:
		
	
	private:
		GUI::LooknFeel::ControlSet*		mpSet;
		GUI::XmlElement*				mpCur;
		const GUI::ImageSet*			mpImgs;		
		void		_Clear					();

		void	_XmlStartCommon			(GUI::XmlElement* pNew);
	public:
				GUILooknFeelReader		(const GUI::ImageSet* pSet);
				~GUILooknFeelReader		();

		void	LoadXML					(const wchar_t* xmlText,GUI::LooknFeel::ControlSet* pMgr);

		void	XmlStartControl			();
		void	XmlEndControl			();
		void	XmlAttribControl		(const wchar_t* attr,int nAttrLen,const wchar_t* value,int nValLen);

		
		void	XmlEndCommon			();
		void	XmlAttribCommon			(const wchar_t* attr,int nAttrLen,const wchar_t* value,int nValLen);

		void	XmlStartControlElement	();
		

		void	XmlStartControlElementFont	();
		

		void	XmlStartControlElementFontColor	();
		
		void	XmlStartControlElementImage	();
		void	XmlAttribElementImage		(const wchar_t* attr,int nAttrLen,const wchar_t* value,int nValLen);
		

		void	XmlStartControlElementImageColor	();
		



	};
}
#endif