//
// InlineImage.h
//
#ifndef __InlineImage_h__
#define __InlineImage_h__

class CInlineImage
{
public:
    IDirect3DTexture9* m_pD3DTexture;
    RECT* m_pSrcRect; // If this pointer is NULL, the entire image will be rendered.
    RECT m_SrcRect;
    RECT m_OrgSrcRect;
    SIZE m_ImageSize;
    DWORD m_dwAlignFormat;
public:
    CInlineImage();
    virtual ~CInlineImage();
    void SetImage(LPCWSTR szFilename, RECT* pSrcRect);
    void Render(const RECT& rcDest,bool bStretch = false);
};

#endif // __InlineImage_h__

