#ifndef _KeywordSystem_20091201pm0446_
#define _KeywordSystem_20091201pm0446_
#include "basetype.h"
#include "XmlLoader.h"
#include "Element.h"
#include "tree.h"


namespace Keyword
{
	typedef			 wchar_t	char16;
	//////////////////////////////////////////////////////////////////////////
	// define
	struct IElement;
	typedef const IElement* IElementPtr;
	typedef std::list<IElementPtr> ElementSet;

	struct IEnvironmentListener ;

	//////////////////////////////////////////////////////////////////////////
	// API
	// �ѪR�y�k ���͹������󶰦X
	void	EnableException		();
	void	DisableException	();
	void	Run						(const char16* keyword,IEnvironmentListener* pListener );
	//////////////////////////////////////////////////////////////////////////	
	//////////////////////////////////////////////////////////////////////////	
	// ���ҰѼ�

	struct Environment : public GUI::XmlElement
	{
		enum
		{
			// �D�n�欰
			NAME,			// element name
			// �H�U�O�ݩ�
			// �@�ΰϰ�
			FONT_COLOR,			 
			FONT_NAME,
			FONT_HEIGHT,
			FONT_WEIGHT,
			FONT_BLOD,
			FONT_ITALIC,
			ID,
			X,
			Y,
			MAPID,
			NPCID,
			HINTID,
			ITEMID,
			MONID,
			COUNT
		};		
				Environment	();
				~Environment();		

		void	Clone(Environment& ele);
	};
	typedef Tree::TNode<Environment> EnvironmentNode;
	struct Element 
	{
		enum
		{
			
		};
		Element (const char16* s,const EnvironmentNode* e) :text(s) , pEnv(e) {}		
		std::wstring		text;
		const EnvironmentNode*	pEnv;
	};

	struct IEnvironmentListener
	{
		struct EventData
		{
			EventData(const Element& e) : env(e){}
			const Element& env;
		};
		//virtual ~IEnvironmentListener() {}
		virtual	void OnEnvironment(const EventData& env) = 0;
	};
	//////////////////////////////////////////////////////////////////////////	
	// ���Һ޲z
	struct EnvironmentManager
	{
						EnvironmentManager		();
						~EnvironmentManager		();
		void			Start					();
		void			Done					();
		
		void			Begin					(const char16* name);
		void			End						();

		Environment*		GetCurrentEnvironment	();
		EnvironmentNode*	GetCurrentEnvironmentNode	();
	private:
		
		EnvironmentNode*	mpEnvCur;
		EnvironmentNode*	mpEnvRoot;
		

	};

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	struct ParseException
	{
		FDOParseException	fdoParseException;
		std::wstring		parseText;
	};

	//////////////////////////////////////////////////////////////////////////
	// �ѪRxml�y�k���͹�������

	struct XmlParser 
	{
				XmlParser	();
				~XmlParser	();

		void	Initial		();
		void	Release		();

		void	XmlStartRoot	(const char16* name,int nLen);
		void	XmlEndRoot		(const char16* name,int nLen);		
		void	XmlAttribRoot	(const char16* attr,int nAttrLen,const char16* value,int nValLen);		
		void	XmlTextRoot		(const char16* text,int nLen);

		void	Run				(const char16* text,IEnvironmentListener* pListener);
	private:
		EnvironmentManager	mEnvMgr;
		typedef std::list<Element*>	ElementSet;
		ElementSet			mElements;
		FDO::XmlLoader		mLoader;
	};

	
}
#endif