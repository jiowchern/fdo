#include "stdafx.h"
#pragma hdrstop
#include "iniReader.h"  
#include <stdio.h>
#include "PacketReader.h"

#ifdef RESMGR
#pragma message("�޲z�Ҧ��Ұ�") 
#else
#pragma message("�@��Ҧ��Ұ�") 
#endif

namespace FDO
{

const fstring iniReader::sEmpty;
//---------------------------------------------------------------------------
//���� *** modify

bool iniReader::LoadFromFile(const string& strFileName, bool packeted)
{
    if (packeted == false)
    {
        FILE *FP;
        if ((FP=fopen(strFileName.c_str(),"r")) == 0)
        {
            return false;
        }
        // �p���ɮפj�p
        fseek( FP, 0, SEEK_END);
        std::vector<unsigned char> in(ftell(FP));
        fseek( FP, 0, SEEK_SET);
        // Ū����Ƭy�������
        fread(&in.front(), 1, in.size(), FP);
        // �����@�Ӥw�g�}�Ҫ��ɮ׸�Ƭy
        fclose(FP);

        return LoadFromFileInMemory(&in.front(), in.size());
    }
    else
    {
        // Ū�����Y�ɮ�
        MemPackPtr mem = LoadFromPacket(strFileName);
        F_RETURN(mem != NULL);
        F_RETURN(mem->IsValid());
        return LoadFromFileInMemory(mem->Data, mem->DataSize);
    }
}
//---------------------------------------------------------------------------
void iniReader::Clear()
{
    mDataMap.clear();
}
//---------------------------------------------------------------------------
iniReader::iniReader()
{
}
//---------------------------------------------------------------------------
iniReader::~iniReader()
{
}
//---------------------------------------------------------------------------
iniReader::iniReader(const string& strFileName, bool packeted)
{
    this->LoadFromFile(strFileName, packeted);
}
//---------------------------------------------------------------------------
iniReader::iniReader(const unsigned char* pData, UINT uiDataSize)
{
    this->LoadFromFileInMemory(pData, uiDataSize);
}
//---------------------------------------------------------------------------
bool iniReader::LoadFromFileInMemory(const unsigned char* pData, unsigned int uiDataSize)
{
    F_RETURN(pData != NULL);
    F_RETURN(uiDataSize != 0);

    FStringList StringArray;

#ifdef RESMGR

	//cosiann 
	char *ptemp = new char[uiDataSize + 1] ;
	ZeroMemory(ptemp , sizeof(char) * (uiDataSize + 1)) ; 
	//cosiann 
	memcpy(ptemp , pData , sizeof(char) * uiDataSize ) ;

    // �H��@�Ϥ�
   // char* pToken = strtok( (char*)pData, "\r\n" );

	//cosiann
	 char* pToken = strtok( (char*)ptemp, "\r\n" );

    while( pToken )
    {
        StringArray.push_back(_F(pToken));

        // ��U�@��
        pToken = strtok( 0, "\r\n" );
    }

	//cosiann  �������A�w�g���ݭn�F
	delete[] ptemp ;
	ptemp = NULL ;

#else
    // �H��@�Ϥ�
    char* pToken = strtok( (char*)pData, "\r\n" );
    while (pToken)
    {
        StringArray.push_back(_F(pToken));
        // ��U�@��
        pToken = strtok( 0, "\r\n" );
    }

#endif

    const fstring strComment = _F("//");
    const fstring strDelims  = _F(" \t\r");
    const fstring strEqual   = _F("=");
    const fstring strComma   = _F("\"");

    // �M�� map
    mDataMap.clear();
    _ULOOP(StringArray.size(), n)
    {
        fstring strText(StringArray[n]);

        int pos = strText.find(strComment);
        if (pos != -1)
        {
            strText.erase(pos);
        }

        // �H����Ÿ����Ϥ��A��X��q
        fstring str1(strText.substr(0, strText.find(strEqual)));
        fstring str2(strText.substr(strText.find(strEqual) + 1));

        // �R���e��ť�
        str1.erase(str1.find_last_not_of(strDelims) + 1);   // trim right
        str1.erase(0, str1.find_first_not_of( strDelims));  // trim left
        str2.erase(str2.find_last_not_of(strDelims) + 1);   // trim right
        str2.erase(0, str2.find_first_not_of( strDelims));  // trim left

        // �R�����޸�(str2 only)
        str2.erase(str2.find_last_not_of(strComma) + 1);
        str2.erase(0, str2.find_first_not_of(strComma));

        // �إ߸`�I
        if (!str1.empty())
        {
            //FDOCh key[MAX_PATH];
            //memcpy(&key, str1.c_str(), MAX_PATH - 1);
            //FDOCh value[MAX_PATH];
            //memcpy(&value, str2.c_str(), MAX_PATH - 1);

            mDataMap.insert(make_pair(str1, str2));
        }
    }
    StringArray.clear();
    return true;
}
/**
 *
 */
int iniReader::GetInteger(const fstring& key) const
{
    fstring value(GetString(key.c_str()));
    return (value.empty()) ? 0 : _F(value);
}

double iniReader::GetDoubler(const fstring &key) const
{
    fstring value(GetString(key.c_str()));
    return (value.empty()) ? 0.0 : _D(value);

}


const fstring& iniReader::GetString(const FDOCh* fmt, ...) const
{
    FDOCh key[MAX_PATH];

    va_list vl;
    va_start(vl, fmt);
    _vsnprintf_s(key, MAX_PATH, fmt, vl);
    va_end(vl);

    //strupr(key);

    ConstDataMapIterator pos = mDataMap.find(key);
    return (pos != mDataMap.end()) ? pos->second : sEmpty;
}

//---------------------------------------------------------------------------
const fstring& GetConfig(const fstring& strFileName)
{
    static iniReader Config(_F("config.ini"));
    return Config.GetString(strFileName.c_str());
}
/**
 * @brief global public function
 */
const fstring& GetSoundMap(const fstring& strTag)
{
    static iniReader Config("Sound\\SoundMap.ini", true );  //establish the unique SoundMap.ini reader
    return Config.GetString(strTag.c_str());    //���� ***
}

} // namespace FDO
