/**
 * @file FdoAllocateHierarchy.h
 * @author Yuming Ho
 * @brief allcate hierarchy
 */
#ifndef FdoAllocateHierarchyH
#define FdoAllocateHierarchyH

#include <d3dx9.h>
#include <string>
#include <map>
#include "FdoSkinMeshDefine.h"

namespace FDO
{

//---------------------------------------------------------------------------
// Class CAllocateHierarchy
//---------------------------------------------------------------------------
class CAllocateHierarchy : public ID3DXAllocateHierarchy
{
private:
    typedef std::map<std::string, LPDIRECT3DTEXTURE9> TextureMap;
	static TextureMap s_meshTextures;

private:
    std::string mPath;
	bool	m_bPlaneModel ; 
public:
    HRESULT WINAPI CreateFrame(LPCSTR Name, LPD3DXFRAME *ppNewFrame);
    HRESULT WINAPI CreateMeshContainer
    (
        LPCSTR Name,
        CONST D3DXMESHDATA *pMeshData,
        CONST D3DXMATERIAL *pMaterials,
        CONST D3DXEFFECTINSTANCE *pEffectInstances,
        DWORD NumMaterials,
        CONST DWORD *pAdjacency,
        LPD3DXSKININFO pSkinInfo,
        LPD3DXMESHCONTAINER *ppNewMeshContainer
    );
    HRESULT WINAPI DestroyFrame(LPD3DXFRAME pFrameToFree);
    HRESULT WINAPI DestroyMeshContainer(LPD3DXMESHCONTAINER pMeshContainerBase);

    CAllocateHierarchy(const std::string& path , bool bPlaneModel);
    CAllocateHierarchy();
};

} // namespace FDO

#endif // FdoAllocateHierarchyH