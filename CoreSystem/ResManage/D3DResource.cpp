#include "stdafx.h" 
#include "resourceMgr.h"
#include "PhenixD3D9RenderSystem.h"
#include "Command.h"
#include "filestream.h"


//////////////				資源相關：刪除器		///////////////////

//特化：記憶體區塊
template<>
void CD3DFilter::operator( ) (FDO::memPack *mem)
{
	//先刪除內部資料
	delete[] mem->Data ;
	mem->Data = NULL ;
	//在刪除外部
	delete mem ;
	mem = NULL ;
}

//特化：sImage物件
template<>
void CD3DFilter::operator( ) (FDO::sImage *image)
{
	if ( image != NULL && image->pTexture != NULL)
	{
		//釋放Texture
		image->pTexture->Release() ;
		image->pTexture = NULL ;
	}
}
//特化：skinmesh資源
template<>
void CD3DFilter::operator( ) (FDO::JLSkinMesh *skin)
{
	delete skin ;
	skin = NULL ;

}


////特化：刪除skybox
//template<>
//void CD3DFilter::operator( ) (SkyboxData *sky)
//{
//	unsigned int i = 0 ;
//
//	if(sky != NULL)
//	{
//		LPD3DXMESH mesh = boost::tuples::get<0>(*sky) ;
//
//		std::vector<D3DMATERIAL9> &mat = boost::tuples::get<1>(*sky) ;
//
//		mat.clear() ;
//
//		std::vector<LPDIRECT3DTEXTURE9> &tex = boost::tuples::get<2>(*sky) ;
//
//
//		for_each(tex.begin() , tex.end() , CD3DRelease() );
//
//		tex.clear() ;
//
//	}
//
//}



//
////特化：讀取log's Texture
//template<>
//void CD3DFilter::operator( ) (stLogtexture *logo)
//{
//	delete logo ;
//	logo = NULL ;
//}
//


//////////////////////////		讀取資源	//////////////////////////



//讀取資源：特化，Texture
template<>
boost::shared_ptr<FDO::sImage> RessourceLoad(const std::string &name ,unsigned int &resize)
{
	
	boost::shared_ptr<FDO::memPack> mem ;

	//從壓縮檔中，抓取文件的記憶體區塊
	mem = GetMemForPacket(name , resize) ;

	//建立Texture物件。並且用特別的刪除器。
	boost::shared_ptr<FDO::sImage> res(new FDO::sImage , CD3DFilter()) ;

	if( mem->Data == NULL)
	{
		res->pTexture = NULL ;

		GetFileStream("missres.txt").Wirte(name) ;

		return res ;
	 
	}

	////		從記體區段中，讀取Texture並且計算資源大小		////////////
	GetTexFormMem(mem->Data , mem->DataSize , resize , &res->pTexture) ;


	Assert( res->pTexture != NULL , "LoadTexture Error") ;

	strcpy(res->szTexFile , name.c_str()) ; 


	return res ;

}

//支援thread 的讀取方式
template<>
boost::shared_ptr<FDO::sImage> RessourceLoad(const std::string &name , FDO::memPack *pMem ,  unsigned int &resize)
{

	//建立Texture物件。並且用特別的刪除器。
	boost::shared_ptr<FDO::sImage> res(new FDO::sImage , CD3DFilter()) ;


	////		從記體區段中，讀取Texture並且計算資源大小		////////////
	GetTexFormMem(pMem->Data , pMem->DataSize , resize , &res->pTexture) ;


	Assert( res->pTexture != NULL , "LoadTexture Error") ;

	strcpy(res->szTexFile , name.c_str()) ; 

	return res;
}



//讀取資源：特化，X file（單純只有網格）
template<>
boost::shared_ptr<ID3DXMesh> RessourceLoad(const std::string &name , unsigned int &resize)
{
    ID3DXMesh *pMesh = NULL;


    // 建立模型資源
    LPD3DXBUFFER pAdjBuffer = NULL;

	boost::shared_ptr<FDO::memPack> mem ;

	//從壓縮檔中，抓取文件的記憶體區塊
	mem = GetMemForPacket(name , resize) ;

	if (mem->Data == NULL)
	{
		//一個空的資源
		boost::shared_ptr<ID3DXMesh> res(pMesh , CD3DFilter()) ;

		GetFileStream("missres.txt").Wirte(name) ;


		return res ;
	}

	//////////////			從記憶體區塊讀取Xfile		///////////////////
    if( SUCCEEDED( D3DXLoadMeshFromXInMemory(
        mem->Data, mem->DataSize,
        D3DXMESH_SYSTEMMEM ,             // 旗標--該網格將放於受管理的記憶體區
        FDO::g_RenderSystem.GetD3DDevice(),  // Our d3dDevice
        &pAdjBuffer,                    // 用於描述網格的鄰接資訊
        0,                              // materials buffer 包含網格的材質資料
        0,                              //
        0,                              // materials number
        &pMesh )))						// Our mesh
    {
        DWORD* pAdjacency = (DWORD*)pAdjBuffer->GetBufferPointer();
        // Optimize the mesh for performance
        pMesh->OptimizeInplace(
            D3DXMESHOPT_COMPACT | D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_VERTEXCACHE,
            pAdjacency, NULL, NULL, NULL );
        pAdjBuffer->Release();
    }

	/////////////		建立smart point
	//配置特別的刪除器
	boost::shared_ptr<ID3DXMesh> res(pMesh , CD3DFilter()) ;

	return res ;

}

//讀取資源：特化，SkinMesh資源
template<>
boost::shared_ptr<FDO::JLSkinMesh> RessourceLoad(const std::string &name , unsigned int &resize)
{
	boost::shared_ptr<FDO::memPack> mem ;

	//從壓縮檔中，抓取文件的記憶體區塊(因為只有單純讀取網格，所以用此方式計算大小就足夠了)

	mem = GetMemForPacket(name , resize) ;

	boost::shared_ptr<FDO::JLSkinMesh> res(new FDO::JLSkinMesh(name) , CD3DFilter()) ;

    res->SetDevice(FDO::g_RenderSystem.GetD3DDevice());
	res->Create(mem->Data , mem->DataSize) ;

	return res ;

}


//讀取資源：特化，Texture
template<>
boost::shared_ptr<IDirect3DTexture9> RessourceLoad(const std::string &name , unsigned int &resize)
{
	IDirect3DTexture9 *tex = NULL ;

	boost::shared_ptr<FDO::memPack> mem ;

	//從壓縮檔中，抓取文件的記憶體區塊
	mem = GetMemForPacket(name , resize) ;

	if( mem->Data == NULL)
	{
		//配置特別的刪除器
		boost::shared_ptr<IDirect3DTexture9> res(tex , CD3DFilter()) ;

		GetFileStream("missres.txt").Wirte(name) ;


		return res ;
	 
	}


	//從記憶體區塊抓取Texture並且計算資源大小
	GetTexFormMem(mem->Data , mem->DataSize , resize , &tex) ;

	/////////////		建立smart point
	//配置特別的刪除器
	boost::shared_ptr<IDirect3DTexture9> res(tex , CD3DFilter()) ;

	return res ;

}

template<>
boost::shared_ptr<IDirect3DTexture9> RessourceLoad(const std::string &name , FDO::memPack *pMem ,  unsigned int &resize)
{

	IDirect3DTexture9 *tex = NULL ;

	resize = pMem->DataSize ; 

	//從記憶體區塊抓取Texture並且計算資源大小
	GetTexFormMem(pMem->Data , pMem->DataSize , resize , &tex) ;

	/////////////		建立smart point
	//配置特別的刪除器
	boost::shared_ptr<IDirect3DTexture9> res(tex , CD3DFilter()) ;

	return res ;
}



//讀取資源：特化，讀取log's Texture
template<>
boost::shared_ptr<stLogtexture> RessourceLoad(const std::string &name , unsigned int &resize)
{
	IDirect3DTexture9 *tex = NULL;


	boost::shared_ptr<FDO::memPack> mem ;

	//從壓縮檔中，抓取文件的記憶體區塊
	mem = GetMemForPacket(name , resize) ;

	/////////////		建立smart point
	//配置特別的刪除器
	boost::shared_ptr<stLogtexture> res(new stLogtexture) ;

	if( mem->Data == NULL)
	{
		//設定空的
		res->tex = NULL ;

		GetFileStream("missres.txt").Wirte(name) ;


		return res ;
	 
	}


	//從記憶體區塊抓取Texture並且計算資源大小
	GetTexFormMemEx(mem->Data , mem->DataSize , resize , &tex) ;

	
	//傳遞剛建立的資源
	res->tex = tex ;

	return res ;
	
}

////////////////////	資源相關：讀取空白Texture資源(RenderTarget)		////////////////
template<>
boost::shared_ptr<IDirect3DTexture9> RessourceLoad(unsigned int &resize ,
												   unsigned int width ,
												   unsigned int height)
{
	IDirect3DTexture9 *tex = NULL ;

	

	//從記憶體區塊抓取Texture並且計算資源大小
	GetTexture(resize , width , height , &tex) ;

	/////////////		建立smart point
	//配置特別的刪除器
	boost::shared_ptr<IDirect3DTexture9> res(tex , CD3DFilter()) ;

	return res ;




}

//讀取資源：特化FDO::sRoleActionList的物件資源分享
//template<>
//boost::shared_ptr<CShareAction> RessourceLoad(const std::string &name , unsigned int &resize)
//{
//	CShareAction *act = NULL ;
//
//	//尋找是否有副檔名
//	std::string filename(name) ;
//
//	size_t found = filename.find("-") ;
//
//	//有的話，將它剔除
//	if (found != string::npos )
//	{
//		filename.erase(found) ;
//	}
//
//	boost::shared_ptr<FDO::memPack> mem ;
//
//	//從壓縮檔中，抓取文件的記憶體區塊
//	mem = GetMemForPacket(filename , resize) ;
//
//	if( mem->Data == NULL)
//	{
//		boost::shared_ptr<CShareAction> res(act) ;
//
//		GetFileStream("missres.txt").Wirte(filename) ;
//
//		return res ;
//	}
//
//	BYTE *pdate = mem->Data ;
//
//	
//	boost::shared_ptr<CShareAction> res(new CShareAction) ;
//
//	res->Create(pdate) ;
//
//	return res ;
//}


////////////			讀取資源；天空盒
template<>
boost::shared_ptr<SkyboxData> RessourceLoad(const std::string &name , unsigned int &resize)
{
	boost::shared_ptr<SkyboxData> res(new SkyboxData ) ;

	//mesh
	LPD3DXMESH pMesh = boost::tuples::get<0>(*res) ;
	//material
	std::vector<D3DMATERIAL9> &vecmat = boost::tuples::get<1>(*res) ;
	//texture
	std::vector<LPDIRECT3DTEXTURE9> &vectex = boost::tuples::get<2>(*res) ;
	//texture nums 
	DWORD &dwNumMaterials = boost::tuples::get<3>(*res) ;


	LPD3DXBUFFER                    pAdjBuffer;                           // for get DWORD*
    LPD3DXBUFFER                    pMtrlBuffer;                          // for get D3DXMATERIAL*
	D3DXMATERIAL                    *pD3DXMaterials ;
	DWORD                           *pAdjacency;


	boost::shared_ptr<FDO::memPack> mem ;

	//從壓縮檔中，抓取文件的記憶體區塊
	mem = GetMemForPacket(name , resize) ;

    //-----------------------------------------------------------------------
    // Load the XFile data.
    //-----------------------------------------------------------------------
    HRESULT hr = D3DXLoadMeshFromXInMemory( mem->Data, mem->DataSize,
                                            D3DXMESH_SYSTEMMEM,         // 旗標--該網格將放於受管理的記憶體區
                                            FDO::g_RenderSystem.GetD3DDevice() , // Our d3dDevice
                                            &pAdjBuffer,              // 用於描述網格的鄰接資訊
                                            &pMtrlBuffer,             // materials buffer 包含網格的材質資料
                                            NULL,                       //
                                            &dwNumMaterials,          // materials number
                                            &pMesh );                 // Our mesh

	if( FAILED( hr ) )
    {
		Assert( 0, "D3DXLoadMeshFromX Failed !!" );
    }

    pD3DXMaterials = (D3DXMATERIAL*)pMtrlBuffer->GetBufferPointer();        // has mtrl and tex information
    pAdjacency     = (DWORD*)pAdjBuffer->GetBufferPointer();

    // Optimize the mesh for performance
    hr = pMesh->OptimizeInplace( D3DXMESHOPT_COMPACT | D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_VERTEXCACHE,
                               pAdjacency, NULL, NULL, NULL );

    if( FAILED( hr ) )
    {
	  Assert( 0, "OptimizeInplace Failed !!" );
    }

 //-----------------------------------------------------------------------
    // Load Texture
    //-----------------------------------------------------------------------
    vecmat.resize( dwNumMaterials );
    vectex.resize( dwNumMaterials );

    // 取得檔案路徑
    string strFilePath = name;
    strFilePath.erase( strFilePath.find_last_of( "\\" ) +1 );
    char szFileName[100];

   for( DWORD i=0; i< dwNumMaterials; ++i )
    {
		// Copy the material
		vecmat[i] = pD3DXMaterials[i].MatD3D;

		// Set the ambient color for the material (D3DX does not do this)
		vecmat[i].Ambient = vecmat[i].Diffuse;

		if( pD3DXMaterials[i].pTextureFilename )
		{
			boost::shared_ptr<FDO::memPack> mem2 ;

			sprintf( szFileName, "%s%s", strFilePath.c_str(), pD3DXMaterials[i].pTextureFilename );

			//從壓縮檔中，抓取文件的記憶體區塊
			mem2 = GetMemForPacket(name , resize) ;
			//從記憶體中，讀取資源
			GetTexFormMem(mem2->Data , mem2->DataSize , resize , &vectex[i]) ;

			if( FAILED( hr ) )
			{
				Assert( 0, "D3DXCreateTextureFromFile Failed !!" );
			}
		}
		else
		{
			vectex[i] = NULL;
		}
    }


	return res ;

}




//讀取資源：特化，記憶體區塊
template<>
boost::shared_ptr<FDO::memPack> RessourceLoad(const std::string &name ,unsigned int &resize)
{
	boost::shared_ptr<FDO::memPack> res ;

	//從壓縮檔中，抓取文件的記憶體區塊
	res = GetMemForPacket(name , resize) ;

	return res ;

}

//讀取資源：特化，記憶體區塊(Thread用)
template<>
boost::shared_ptr<FDO::memPack> RessourceLoad(const std::string &name , FDO::memPack *pMem , unsigned int &resize)
{
	//建立一筆記憶體區塊資料：配置特別的刪除器
	boost::shared_ptr<FDO::memPack> res(new FDO::memPack , CD3DFilter()) ;

	res->Data = new unsigned char[pMem->DataSize] ;

	//清空

	memset(res->Data , 0 , pMem->DataSize) ;
		

	//複製資料
	memcpy(res->Data , pMem->Data , pMem->DataSize );
	//填入資料
	res->DataSize = pMem->DataSize ; 
	//紀錄資源大小
	resize = pMem->DataSize ;

	return res ; 

}


//////////////////////			讀取解壓縮封包			/////////////////

boost::shared_ptr<FDO::memPack> GetMemForPacket(const std::string &name ,unsigned int &resize)
{

	///////////////////////////////////////
    static FDO::PacketReader PR;

	FDO::memPack *data = NULL;

	//從壓縮檔中，解壓縮
    data = PR.LoadFromFile( name );	

//	Assert(data != NULL , "GetMemForPacket() error") ;

	//建立一筆記憶體區塊資料：配置特別的刪除器
	boost::shared_ptr<FDO::memPack> res(new FDO::memPack , CD3DFilter()) ;

	if (data == NULL)
	{
		res->Data = NULL ;
		res->DataSize = 0 ;


		GetFileStream("missres.txt").Wirte(name) ;



		return res ;
	}




	res->Data = new unsigned char[data->DataSize] ;

	//清空

	memset(res->Data , 0 , data->DataSize) ;
	//ZeroMemory(res->Data , data->DataSize * sizeof(unsigned char) ) ;

	
	

	//複製資料
	memcpy(res->Data , data->Data , data->DataSize );
	//填入資料
	res->DataSize = data->DataSize ; 
	//紀錄資源大小
	resize = data->DataSize ;

	return res ;

}

//////////////////////		從記憶體區塊抓取Texture並且計算資源大小		/////////////////

//第三和第四個參數為out。
void GetTexFormMem(unsigned char* data , unsigned int DataSize ,unsigned int &resize ,
				   IDirect3DTexture9 **tex)
{
	///////////			從記體區段中，讀取Texture		////////////
	

	HRESULT hr  ;



	if(FAILED(hr = D3DXCreateTextureFromFileInMemory(
							FDO::g_RenderSystem.GetD3DDevice(),
							data,
							DataSize,
							tex)   )) 
	{

		if(hr == D3DERR_NOTAVAILABLE)
		{
			Assert(0 , "D3DERR_NOTAVAILABLE") ;
		}
		else if(hr == D3DERR_OUTOFVIDEOMEMORY)
		{
			Assert(0 , "恭喜你！顯示卡記憶體爆掉") ;
		}
		else if(hr == D3DERR_INVALIDCALL)
		{
			Assert(0 , "texutre point valid") ;
		}
		else if(hr == D3DXERR_INVALIDDATA)
		{
			Assert(0 , "D3DXERR_INVALIDDATA") ;
		}
		else if(hr == E_OUTOFMEMORY)
		{
			Assert(0 , "E_OUTOFMEMORY") ;

		}
		else
		{
			Assert(0 , "D3DXCreateTextureFromFileInMemory error") ;

		}

	}

	 //並且計算Textrue大小
	// GetTextrueResSize(resize , tex) ;


}

//第三個參數和第四個參數為out
void GetTexFormMemEx(unsigned char* data , unsigned int DataSize , 
				   unsigned int &resize , IDirect3DTexture9 **tex)
{

	//用Ex作讀Texture方式
    D3DXCreateTextureFromFileInMemoryEx( 
		 FDO::g_RenderSystem.GetD3DDevice(),
		 data,
		 DataSize,
		 FDO::g_RenderSystem.GetClientWidth(),
		 FDO::g_RenderSystem.GetClientHeight(),
		 1,
		 D3DUSAGE_RENDERTARGET,
		 FDO::g_RenderSystem.GetRTTFormat(),
		 D3DPOOL_DEFAULT,
		 D3DX_DEFAULT ,
		 D3DX_DEFAULT ,
		 0,
		 NULL,
		 NULL,
		 tex );

	 Assert( (*tex) != NULL , "GetTexForMemEx Error") ;

	 //並且計算Textrue大小
	 GetTextrueResSize(resize , tex) ;

}

//建立一個空白Texture(RenderTarget用)
void GetTexture(unsigned int &resize , unsigned int width , unsigned int height , 
				IDirect3DTexture9 **tex)
{


	//建立一個空白Texture：RenderTarget用
	FDO::g_RenderSystem.GetD3DDevice()->CreateTexture(
		width , height , 1 ,
      D3DUSAGE_RENDERTARGET, 
	  FDO::g_RenderSystem.GetRTTFormat(),
      D3DPOOL_DEFAULT, tex , NULL ) ; 


	 Assert( (*tex) != NULL , "GetTexForMemEx Error") ;


	 //並且計算Textrue大小
	 GetTextrueResSize(resize , tex) ;


}


//取得texture的資源記憶體大小
void GetTextrueResSize(unsigned int &resize , IDirect3DTexture9 **tex)
{

	D3DSURFACE_DESC desc ;

	if((*tex) == NULL)
	{
		return ; 
	}

	 (*tex)->GetLevelDesc(0 , &desc) ; 

	 //////////				判斷圖檔格式		/////////////
	 switch(desc.Format)
	 {
	 case D3DFMT_X8R8G8B8:
	 case D3DFMT_A8R8G8B8: //32bit = 4byte
		 {
			 resize = desc.Width * desc.Height * 4 ;
		 }break ;
	 case D3DFMT_R8G8B8://24bit = 3byte
		 {
			 resize = desc.Width * desc.Height * 3 ;
		 }break ;
	 case D3DFMT_R5G6B5: //16bit = 2byte
		 {
			 resize = desc.Width * desc.Height * 2 ;
		 }break ;
	 default:	//其他
		 {
			resize = desc.Width * desc.Height ;
		 }break ;
	 }
}
