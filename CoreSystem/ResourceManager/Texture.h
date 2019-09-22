#ifndef TEXTURE_H
#define TEXTURE_H
//---------------------------------------------------------------------------
#include "Object.h"
#include <d3d9.h> // for LPDIRECT3DTEXTURE9


namespace FDO
{

    class Texture : public Object
    {
    private:

        LPDIRECT3DTEXTURE9  m_Texture;  // �������

        //�I�s���禡�ӥͦ��@�Ӽv������
        bool LoadFromFileInMemory(BYTE *Data, UINT DataSize);

    public:

        //�H�ɦW��@�v���ɦW��
         Texture(const std::string &FileName) : Object(FileName) { Clear(); }
        ~Texture() { Destory(); }

        void Clear();
        void Destory();
        bool Release();
        bool Create(BYTE *Data, UINT DataSize)
            { return LoadFromFileInMemory( Data, DataSize ); }

        //�o�Ӽv���ҨϥΪ��O����j�p
        //virtual int GetSize() = 0;

        LPDIRECT3DTEXTURE9 Get() { return m_Texture; }

    };

}


//---------------------------------------------------------------------------
#endif // TEXTURE_H