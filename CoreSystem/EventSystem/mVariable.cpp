#include "stdafx.h"
#pragma hdrstop
//---------------------------------------------------------------------------
#include "mVariable.h"  
//---------------------------------------------------------------------------
moVariable::moVariable(bool Ddel)
{
    Name="";
    Var="";
    varKind=DATATYPE_NONE;
    canDirDel=Ddel;
}
//---------------------------------------------------------------------------
moVariable::moVariable(BYTE varKind1,String Name1)
{
    Name=Name1;
    Var= (varKind1==DATATYPE_TEXT) ? "":"0";
    varKind=static_cast<mEventMacroProcess_enum>(varKind1);
    canDirDel=false;
}
//---------------------------------------------------------------------------
moVariable::moVariable(EVS_Unit_Class *ctp)
{
    Name="";//獶跑计
    Var=ctp->Var;
    varKind=static_cast<mEventMacroProcess_enum>(ctp->Type);
    canDirDel=false;
}
//---------------------------------------------------------------------------
moVariable::~moVariable()
{
}
//---------------------------------------------------------------------------
void moVariable::operator = (String &sou)
{
    Var=sou;
}
//---------------------------------------------------------------------------
void moVariable::operator = (moVariable &sou)
{
    varKind=sou.varKind;
    Var=sou.Var;
}
//---------------------------------------------------------------------------
void moVariable::operator = (moVariable *sou)
{
    varKind=sou->varKind;
    Var=sou->Var;
    if(sou->canDirDel)
        delete sou;
}
//---------------------------------------------------------------------------
void moVariable::operator = (int sou)
{
    Var=ToString(sou);
}
//---------------------------------------------------------------------------
void moVariable::operator = (double sou)
{
    Var=ToString(sou);
}
//---------------------------------------------------------------------------
void moVariable::operator = (float sou)
{
    Var=ToString(sou);
}
//---------------------------------------------------------------------------
void moVariable::operator = (short sou)
{
    Var=ToString(sou);
}
//---------------------------------------------------------------------------
void moVariable::operator = (bool sou)
{
    Var=(sou==true) ? "1":"0";
}
//---------------------------------------------------------------------------
void moVariable::operator += (moVariable &sou)
{
    switch(varKind)
    {
        //case DATATYPE_TEXT://ゅ
        //        Var=Var+sou.Var;
        //        break;
        case DATATYPE_FLOAT://疊翴计
                Var=ToString( ToDouble(Var)+ToDouble(sou.Var) );
                break;
		default :
                Var=ToString( ToInt(Var)+ToInt(sou.Var) );
                break;

    };
}
//---------------------------------------------------------------------------
void moVariable::operator -= (moVariable &sou)
{
    switch(varKind)
    {
        case DATATYPE_FLOAT://疊翴计
                Var=ToString( ToDouble(Var)-ToDouble(sou.Var) );
                break;
		default :
                Var=ToString( ToInt(Var)-ToInt(sou.Var) );
                break;

    };
}
//---------------------------------------------------------------------------
void moVariable::operator *= (moVariable &sou)
{
    switch(varKind)
    {
        case DATATYPE_FLOAT://疊翴计
                Var= ToString( ToDouble(Var)*ToDouble(sou.Var) );
                break;
		default :
                Var= ToString( ToInt(Var)*ToInt(sou.Var) );
                break;
    };
}
//---------------------------------------------------------------------------
void moVariable::operator /= (moVariable &sou)
{
    switch(varKind)
    {
        case DATATYPE_FLOAT://疊翴计
                Var=ToString( ToDouble(Var)/ToDouble(sou.Var) );
                break;
		default :
                Var=ToString( ToInt(Var)/ToInt(sou.Var) );
                break;
    };
}
//---------------------------------------------------------------------------
moVariable moVariable::operator + (moVariable &sou)
{
    moVariable Vt;
    Vt=*this;
    Vt+=sou;
    return Vt;
}
//---------------------------------------------------------------------------
moVariable moVariable::operator - (moVariable &sou)
{
    moVariable Vt;
    Vt=*this;
    Vt-=sou;
    return Vt;
}
//---------------------------------------------------------------------------
moVariable moVariable::operator * (moVariable &sou)
{
    moVariable Vt;
    Vt=*this;
    Vt*=sou;
    return Vt;
}
//---------------------------------------------------------------------------
moVariable moVariable::operator / (moVariable &sou)
{
    moVariable Vt;
    Vt=*this;
    Vt/=sou;
    return Vt;
}
//---------------------------------------------------------------------------
moVariable* moVariable::operator + (moVariable *sou)
{
    moVariable *Vt=new moVariable();
    Vt->canDirDel=true;
    *Vt=*this;
    *Vt+=*sou;
    return Vt;
}
//---------------------------------------------------------------------------
moVariable* moVariable::operator - (moVariable *sou)
{
    moVariable *Vt=new moVariable();
    Vt->canDirDel=true;
    *Vt=*this;
    *Vt-=*sou;
    return Vt;
}
//---------------------------------------------------------------------------
moVariable* moVariable::operator * (moVariable *sou)
{
    moVariable *Vt=new moVariable();
    Vt->canDirDel=true;
    *Vt=*this;
    *Vt*=*sou;
    return Vt;
}
//---------------------------------------------------------------------------
moVariable* moVariable::operator / (moVariable *sou)
{
    moVariable *Vt=new moVariable();
    Vt->canDirDel=true;
    *Vt=*this;
    *Vt/=*sou;
    return Vt;
}
//---------------------------------------------------------------------------
bool moVariable::operator == (moVariable &sou)
{
    return (this->Var == sou.Var);
}
//---------------------------------------------------------------------------
bool moVariable::operator != (moVariable &sou)
{
    return (ToDouble(this->Var) != ToDouble(sou.Var));
}
//---------------------------------------------------------------------------
bool moVariable::operator >= (moVariable &sou)
{
    return (ToDouble(this->Var) >= ToDouble(sou.Var));
}
//---------------------------------------------------------------------------
bool moVariable::operator <= (moVariable &sou)
{
    return (ToDouble(this->Var) <= ToDouble(sou.Var));
}
//---------------------------------------------------------------------------
bool moVariable::operator > (moVariable &sou)
{
    return (ToDouble(this->Var) > ToDouble(sou.Var));
}
//---------------------------------------------------------------------------
bool moVariable::operator < (moVariable &sou)
{
    return (ToDouble(this->Var) < ToDouble(sou.Var));
}
//---------------------------------------------------------------------------

