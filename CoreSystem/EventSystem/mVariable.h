//-----------------------------------------------------------------------
#ifndef mo_Variable_H
#define mo_Variable_H
//-----------------------------------------------------------------------
#pragma pack(push) //確保資料的定位一定正確
#pragma pack(1)
//-----------------------------------------------------------------------
//變數型態enum
enum mEventMacroProcess_enum
{
	DATATYPE_NONE=0,
	DATATYPE_BOOL,				        //TF
    DATATYPE_BYTE,				        //byte
    DATATYPE_SHORT,				        //短int
	DATATYPE_LONG,				        //長int
	DATATYPE_FLOAT,				        //單精確度浮點數
	DATATYPE_TEXT,				        //文字
	DATATYPE_SELECTION,			        //?
	DATATYPE_FLAG,				        //旗標
	DATATYPE_VARIABLE_INDEX,	        //區域變數索引
  	DATATYPE_LABEL_INDEX		        //msg文字索引
};
//-----------------------------------------------------------------------
//欄位資訊for巨集class
struct EVS_Unit_Class
{
	int 	Type;                       //欄位型別
	String  Var;                        //欄位值
};
//-----------------------------------------------------------------------
#pragma pack( pop)
//-----------------------------------------------------------------------
//紀錄變數區塊
//-----------------------------------------------------------------------
class moVariable
{
    private:
        bool canDirDel;//可否傳遞值後在另一個地方刪除
    public:
        moVariable(bool Ddel=false);
        moVariable(BYTE varKind,String Name);//變數使用
        moVariable(EVS_Unit_Class *ctp);//純值使用
        ~moVariable();

        String Name;
        String Var;
	    mEventMacroProcess_enum varKind;    //變數型態

        void operator =  (String &sou);
        void operator =  (moVariable &sou);
        void operator =  (moVariable *sou);
        void operator =  (int sou);
        void operator =  (double sou);
        void operator =  (float sou);
        void operator =  (short sou);
        void operator =  (bool sou);

        void operator += (moVariable &sou);
        void operator -= (moVariable &sou);
        void operator *= (moVariable &sou);
        void operator /= (moVariable &sou);

        moVariable operator + (moVariable &sou);
        moVariable operator - (moVariable &sou);
        moVariable operator * (moVariable &sou);
        moVariable operator / (moVariable &sou);

        moVariable* operator + (moVariable *sou);
        moVariable* operator - (moVariable *sou);
        moVariable* operator * (moVariable *sou);
        moVariable* operator / (moVariable *sou);

        bool operator == (moVariable &sou);
        bool operator != (moVariable &sou);
        bool operator >= (moVariable &sou);
        bool operator <= (moVariable &sou);
        bool operator > (moVariable &sou);
        bool operator < (moVariable &sou);

};
//-----------------------------------------------------------------------
#endif
