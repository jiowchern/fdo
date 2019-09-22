//-----------------------------------------------------------------------
#ifndef mo_Variable_H
#define mo_Variable_H
//-----------------------------------------------------------------------
#pragma pack(push) //�T�O��ƪ��w��@�w���T
#pragma pack(1)
//-----------------------------------------------------------------------
//�ܼƫ��Aenum
enum mEventMacroProcess_enum
{
	DATATYPE_NONE=0,
	DATATYPE_BOOL,				        //TF
    DATATYPE_BYTE,				        //byte
    DATATYPE_SHORT,				        //�uint
	DATATYPE_LONG,				        //��int
	DATATYPE_FLOAT,				        //���T�ׯB�I��
	DATATYPE_TEXT,				        //��r
	DATATYPE_SELECTION,			        //?
	DATATYPE_FLAG,				        //�X��
	DATATYPE_VARIABLE_INDEX,	        //�ϰ��ܼƯ���
  	DATATYPE_LABEL_INDEX		        //msg��r����
};
//-----------------------------------------------------------------------
//����Tfor����class
struct EVS_Unit_Class
{
	int 	Type;                       //��쫬�O
	String  Var;                        //����
};
//-----------------------------------------------------------------------
#pragma pack( pop)
//-----------------------------------------------------------------------
//�����ܼư϶�
//-----------------------------------------------------------------------
class moVariable
{
    private:
        bool canDirDel;//�i�_�ǻ��ȫ�b�t�@�Ӧa��R��
    public:
        moVariable(bool Ddel=false);
        moVariable(BYTE varKind,String Name);//�ܼƨϥ�
        moVariable(EVS_Unit_Class *ctp);//�­Ȩϥ�
        ~moVariable();

        String Name;
        String Var;
	    mEventMacroProcess_enum varKind;    //�ܼƫ��A

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
