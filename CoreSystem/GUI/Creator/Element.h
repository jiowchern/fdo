#ifndef _Element_20080829pm0320_
#define _Element_20080829pm0320_
#include "ElementDef.h"
#include "basetype.h"
namespace GUI
{

	//////////////////////////////////////////////////////////////////////////
	struct Property
	{			
		enum
		{
			BUFFER_SIZE = 32
		};		
		union
		{
			struct  
			{
				char				buf[BUFFER_SIZE];				
			};			
			unsigned int		valn32;
			int					valsn32;
			unsigned __int64	valn64;
			float				valf32;
			char*				vals8;
			wchar_t*			vals16;
			//double			valf64;
			struct
			{
				union
				{
					const char*		valc8;
					const wchar_t*	valc16;
					void*			valvp;
				};
				uint	nMemSize;				
			};

		};
		//////////////////////////////////////////////////////////////////////////
// 		template<typename T>
// 		bool Get(T& val) const 
// 		{
// 			if(bValid)
// 				val = (T)valvp;
// 			return bValid;
// 		}
		//////////////////////////////////////////////////////////////////////////
		Property() ;
		//////////////////////////////////////////////////////////////////////////
		virtual ~Property();
		//////////////////////////////////////////////////////////////////////////
		void Release();
		//////////////////////////////////////////////////////////////////////////
		void _FreePointer(void * p);
		//////////////////////////////////////////////////////////////////////////
		// operators
		//////////////////////////////////////////////////////////////////////////
		void operator = (unsigned int val);
		//////////////////////////////////////////////////////////////////////////
		void operator = (int val);
		//////////////////////////////////////////////////////////////////////////
		void operator = (float val);
		//////////////////////////////////////////////////////////////////////////
		void operator = (const char* val);
		//////////////////////////////////////////////////////////////////////////
		void operator = (const wchar_t* val);
		//////////////////////////////////////////////////////////////////////////
		void operator = (void* val);
		//////////////////////////////////////////////////////////////////////////		
		void* operator new(size_t size);
		//////////////////////////////////////////////////////////////////////////
		void operator delete(void* p);

		//////////////////////////////////////////////////////////////////////////
		void operator = (const Property& p);
	};
	//////////////////////////////////////////////////////////////////////////
	
	
	struct PropertyFactory : public ObjFactory<Property>,public FDO::Singleton< PropertyFactory > 
	{

	};
	
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	class Element 
	{
	protected:
		struct Slot
		{
			Property*	pProperty;
						Slot	();
						~Slot	();
			void		Init	();
			void		Release	();
			bool		IsValid	() const ;
			Property*	Get		();
		};
		int					mType;
		Slot*				mPropertys;
		int					mnPropertySize;
		Element*			mpParent;
		std::list<Element*> mChilds;

		void			_CheckValid	(int iIndex);
	public:
						Element		(int type,Element* pParent,int nPropertySize);
		virtual			~Element	();
		//////////////////////////////////////////////////////////////////////////
		void			Set			(int iIndex,int val);
		void			Set			(int iIndex,unsigned int val);
		void			Set			(int iIndex,float val);
		void			Set			(int iIndex,const char* val);
		void			Set			(int iIndex,const wchar_t* val);
		void			Set			(int iIndex,const char* buf,int size);
		void			Set			(int iIndex,void* p);

		void			Set			(int iIndex,const Property*)						;

		void			Add			(Element* pChild);		

		//////////////////////////////////////////////////////////////////////////

		bool			Get			(int iIndex,int& val)				const;
		bool			Get			(int iIndex,unsigned int& val)		const;
		bool			Get			(int iIndex,float& val)				const;
		bool			Get			(int iIndex,char*& val)				const;
		bool			Get			(int iIndex,wchar_t*& val)			const;
		bool			Get			(int iIndex,char*& val,int size)	const;
		bool			Get			(int iIndex,void*&)					const;
		const Property*	Get			(int iIndex)						const;

		Element*		GetParent	();
		const Element*	GetParent	() const ;
		bool			GetChild	(std::list<const Element*>& set)	const;
		int				GetType		()const;
		inline int		GetPropertySize	() const 
		{
			return mnPropertySize;
		}

		static void		fnDestoryElementTree(Element* pRoot);
	protected:
		Property*		_Get			(int iIndex)		const				;
	};
	//////////////////////////////////////////////////////////////////////////
	
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	class XmlElement : public Element
	{
	public:
		typedef void (*PFN_SET_GUIELEMENT)(Element* pEle,int iProperty,const wchar_t* value);
	private:
		static std::map<wstring,PFN_SET_GUIELEMENT> KeyWordMap;
		struct SetInfo
		{
			int					iProperty;
			PFN_SET_GUIELEMENT	pfn;
		};
		std::map<wstring,SetInfo>	mKeyWordIndexMap;
		
	protected:
		static PFN_SET_GUIELEMENT fnGetPropertySets	(const wchar_t* attrib);
		bool	_SetPropertySet	(int index,const wchar_t* attrib,PFN_SET_GUIELEMENT pfnCustom = 0);
		bool	_SetPropertySet	(int index,const wchar_t* attrib,int nCNV);
		
	public:
		enum
		{
			CNV_INT,CNV_FLOAT,CNV_WCHAR,CNV_ARGB,CNV_ARGBR,CNV_ARGBG,CNV_ARGBB,CNV_ARGBA,CNV_HEX,CNVS
		};
					XmlElement	(int type,Element* pParent,int nPropertySize);
					~XmlElement	();
		bool SetAttrib	(const wchar_t* attr,int nAttrLen,const wchar_t* value,int nValLen) ;
	};
	

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	class ElementSet
	{
	protected:
		
		std::list<Element*> mElements;
	public:
				ElementSet	();
		virtual ~ElementSet	();
		//////////////////////////////////////////////////////////////////////////

		void			Add			(Element* p);				
	};	

#define _SetGUIProperty(offset,label) _SetPropertySet(offset + label,L#label)
#define _SetGUIPropertyCNV(offset,label,cnv_type) _SetPropertySet(offset + label,L#label,cnv_type)

};
#endif