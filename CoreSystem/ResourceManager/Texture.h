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

        LPDIRECT3DTEXTURE9  m_Texture;  // 材質指標

        //呼叫此函式來生成一個影像物件
        bool LoadFromFileInMemory(BYTE *Data, UINT DataSize);

    public:

        //以檔名當作影像檔名稱
         Texture(const std::string &FileName) : Object(FileName) { Clear(); }
        ~Texture() { Destory(); }

        void Clear();
        void Destory();
        bool Release();
        bool Create(BYTE *Data, UINT DataSize)
            { return LoadFromFileInMemory( Data, DataSize ); }

        //這個影像所使用的記憶體大小
        //virtual int GetSize() = 0;

        LPDIRECT3DTEXTURE9 Get() { return m_Texture; }

    };

}


//---------------------------------------------------------------------------
#endif // TEXTURE_H