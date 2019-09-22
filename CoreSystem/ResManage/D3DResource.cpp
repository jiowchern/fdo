#include "stdafx.h" 
#include "resourceMgr.h"
#include "PhenixD3D9RenderSystem.h"
#include "Command.h"
#include "filestream.h"


//////////////				�귽�����G�R����		///////////////////

//�S�ơG�O����϶�
template<>
void CD3DFilter::operator( ) (FDO::memPack *mem)
{
	//���R���������
	delete[] mem->Data ;
	mem->Data = NULL ;
	//�b�R���~��
	delete mem ;
	mem = NULL ;
}

//�S�ơGsImage����
template<>
void CD3DFilter::operator( ) (FDO::sImage *image)
{
	if ( image != NULL && image->pTexture != NULL)
	{
		//����Texture
		image->pTexture->Release() ;
		image->pTexture = NULL ;
	}
}
//�S�ơGskinmesh�귽
template<>
void CD3DFilter::operator( ) (FDO::JLSkinMesh *skin)
{
	delete skin ;
	skin = NULL ;

}


////�S�ơG�R��skybox
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
////�S�ơGŪ��log's Texture
//template<>
//void CD3DFilter::operator( ) (stLogtexture *logo)
//{
//	delete logo ;
//	logo = NULL ;
//}
//


//////////////////////////		Ū���귽	//////////////////////////



//Ū���귽�G�S�ơATexture
template<>
boost::shared_ptr<FDO::sImage> RessourceLoad(const std::string &name ,unsigned int &resize)
{
	
	boost::shared_ptr<FDO::memPack> mem ;

	//�q���Y�ɤ��A�����󪺰O����϶�
	mem = GetMemForPacket(name , resize) ;

	//�إ�Texture����C�åB�ίS�O���R�����C
	boost::shared_ptr<FDO::sImage> res(new FDO::sImage , CD3DFilter()) ;

	if( mem->Data == NULL)
	{
		res->pTexture = NULL ;

		GetFileStream("missres.txt").Wirte(name) ;

		return res ;
	 
	}

	////		�q�O��Ϭq���AŪ��Texture�åB�p��귽�j�p		////////////
	GetTexFormMem(mem->Data , mem->DataSize , resize , &res->pTexture) ;


	Assert( res->pTexture != NULL , "LoadTexture Error") ;

	strcpy(res->szTexFile , name.c_str()) ; 


	return res ;

}

//�䴩thread ��Ū���覡
template<>
boost::shared_ptr<FDO::sImage> RessourceLoad(const std::string &name , FDO::memPack *pMem ,  unsigned int &resize)
{

	//�إ�Texture����C�åB�ίS�O���R�����C
	boost::shared_ptr<FDO::sImage> res(new FDO::sImage , CD3DFilter()) ;


	////		�q�O��Ϭq���AŪ��Texture�åB�p��귽�j�p		////////////
	GetTexFormMem(pMem->Data , pMem->DataSize , resize , &res->pTexture) ;


	Assert( res->pTexture != NULL , "LoadTexture Error") ;

	strcpy(res->szTexFile , name.c_str()) ; 

	return res;
}



//Ū���귽�G�S�ơAX file�]��¥u������^
template<>
boost::shared_ptr<ID3DXMesh> RessourceLoad(const std::string &name , unsigned int &resize)
{
    ID3DXMesh *pMesh = NULL;


    // �إ߼ҫ��귽
    LPD3DXBUFFER pAdjBuffer = NULL;

	boost::shared_ptr<FDO::memPack> mem ;

	//�q���Y�ɤ��A�����󪺰O����϶�
	mem = GetMemForPacket(name , resize) ;

	if (mem->Data == NULL)
	{
		//�@�ӪŪ��귽
		boost::shared_ptr<ID3DXMesh> res(pMesh , CD3DFilter()) ;

		GetFileStream("missres.txt").Wirte(name) ;


		return res ;
	}

	//////////////			�q�O����϶�Ū��Xfile		///////////////////
    if( SUCCEEDED( D3DXLoadMeshFromXInMemory(
        mem->Data, mem->DataSize,
        D3DXMESH_SYSTEMMEM ,             // �X��--�Ӻ���N�����޲z���O�����
        FDO::g_RenderSystem.GetD3DDevice(),  // Our d3dDevice
        &pAdjBuffer,                    // �Ω�y�z���檺�F����T
        0,                              // materials buffer �]�t���檺������
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

	/////////////		�إ�smart point
	//�t�m�S�O���R����
	boost::shared_ptr<ID3DXMesh> res(pMesh , CD3DFilter()) ;

	return res ;

}

//Ū���귽�G�S�ơASkinMesh�귽
template<>
boost::shared_ptr<FDO::JLSkinMesh> RessourceLoad(const std::string &name , unsigned int &resize)
{
	boost::shared_ptr<FDO::memPack> mem ;

	//�q���Y�ɤ��A�����󪺰O����϶�(�]���u�����Ū������A�ҥH�Φ��覡�p��j�p�N�����F)

	mem = GetMemForPacket(name , resize) ;

	boost::shared_ptr<FDO::JLSkinMesh> res(new FDO::JLSkinMesh(name) , CD3DFilter()) ;

    res->SetDevice(FDO::g_RenderSystem.GetD3DDevice());
	res->Create(mem->Data , mem->DataSize) ;

	return res ;

}


//Ū���귽�G�S�ơATexture
template<>
boost::shared_ptr<IDirect3DTexture9> RessourceLoad(const std::string &name , unsigned int &resize)
{
	IDirect3DTexture9 *tex = NULL ;

	boost::shared_ptr<FDO::memPack> mem ;

	//�q���Y�ɤ��A�����󪺰O����϶�
	mem = GetMemForPacket(name , resize) ;

	if( mem->Data == NULL)
	{
		//�t�m�S�O���R����
		boost::shared_ptr<IDirect3DTexture9> res(tex , CD3DFilter()) ;

		GetFileStream("missres.txt").Wirte(name) ;


		return res ;
	 
	}


	//�q�O����϶����Texture�åB�p��귽�j�p
	GetTexFormMem(mem->Data , mem->DataSize , resize , &tex) ;

	/////////////		�إ�smart point
	//�t�m�S�O���R����
	boost::shared_ptr<IDirect3DTexture9> res(tex , CD3DFilter()) ;

	return res ;

}

template<>
boost::shared_ptr<IDirect3DTexture9> RessourceLoad(const std::string &name , FDO::memPack *pMem ,  unsigned int &resize)
{

	IDirect3DTexture9 *tex = NULL ;

	resize = pMem->DataSize ; 

	//�q�O����϶����Texture�åB�p��귽�j�p
	GetTexFormMem(pMem->Data , pMem->DataSize , resize , &tex) ;

	/////////////		�إ�smart point
	//�t�m�S�O���R����
	boost::shared_ptr<IDirect3DTexture9> res(tex , CD3DFilter()) ;

	return res ;
}



//Ū���귽�G�S�ơAŪ��log's Texture
template<>
boost::shared_ptr<stLogtexture> RessourceLoad(const std::string &name , unsigned int &resize)
{
	IDirect3DTexture9 *tex = NULL;


	boost::shared_ptr<FDO::memPack> mem ;

	//�q���Y�ɤ��A�����󪺰O����϶�
	mem = GetMemForPacket(name , resize) ;

	/////////////		�إ�smart point
	//�t�m�S�O���R����
	boost::shared_ptr<stLogtexture> res(new stLogtexture) ;

	if( mem->Data == NULL)
	{
		//�]�w�Ū�
		res->tex = NULL ;

		GetFileStream("missres.txt").Wirte(name) ;


		return res ;
	 
	}


	//�q�O����϶����Texture�åB�p��귽�j�p
	GetTexFormMemEx(mem->Data , mem->DataSize , resize , &tex) ;

	
	//�ǻ���إߪ��귽
	res->tex = tex ;

	return res ;
	
}

////////////////////	�귽�����GŪ���ť�Texture�귽(RenderTarget)		////////////////
template<>
boost::shared_ptr<IDirect3DTexture9> RessourceLoad(unsigned int &resize ,
												   unsigned int width ,
												   unsigned int height)
{
	IDirect3DTexture9 *tex = NULL ;

	

	//�q�O����϶����Texture�åB�p��귽�j�p
	GetTexture(resize , width , height , &tex) ;

	/////////////		�إ�smart point
	//�t�m�S�O���R����
	boost::shared_ptr<IDirect3DTexture9> res(tex , CD3DFilter()) ;

	return res ;




}

//Ū���귽�G�S��FDO::sRoleActionList������귽����
//template<>
//boost::shared_ptr<CShareAction> RessourceLoad(const std::string &name , unsigned int &resize)
//{
//	CShareAction *act = NULL ;
//
//	//�M��O�_�����ɦW
//	std::string filename(name) ;
//
//	size_t found = filename.find("-") ;
//
//	//�����ܡA�N���簣
//	if (found != string::npos )
//	{
//		filename.erase(found) ;
//	}
//
//	boost::shared_ptr<FDO::memPack> mem ;
//
//	//�q���Y�ɤ��A�����󪺰O����϶�
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


////////////			Ū���귽�F�ѪŲ�
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

	//�q���Y�ɤ��A�����󪺰O����϶�
	mem = GetMemForPacket(name , resize) ;

    //-----------------------------------------------------------------------
    // Load the XFile data.
    //-----------------------------------------------------------------------
    HRESULT hr = D3DXLoadMeshFromXInMemory( mem->Data, mem->DataSize,
                                            D3DXMESH_SYSTEMMEM,         // �X��--�Ӻ���N�����޲z���O�����
                                            FDO::g_RenderSystem.GetD3DDevice() , // Our d3dDevice
                                            &pAdjBuffer,              // �Ω�y�z���檺�F����T
                                            &pMtrlBuffer,             // materials buffer �]�t���檺������
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

    // ���o�ɮ׸��|
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

			//�q���Y�ɤ��A�����󪺰O����϶�
			mem2 = GetMemForPacket(name , resize) ;
			//�q�O���餤�AŪ���귽
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




//Ū���귽�G�S�ơA�O����϶�
template<>
boost::shared_ptr<FDO::memPack> RessourceLoad(const std::string &name ,unsigned int &resize)
{
	boost::shared_ptr<FDO::memPack> res ;

	//�q���Y�ɤ��A�����󪺰O����϶�
	res = GetMemForPacket(name , resize) ;

	return res ;

}

//Ū���귽�G�S�ơA�O����϶�(Thread��)
template<>
boost::shared_ptr<FDO::memPack> RessourceLoad(const std::string &name , FDO::memPack *pMem , unsigned int &resize)
{
	//�إߤ@���O����϶���ơG�t�m�S�O���R����
	boost::shared_ptr<FDO::memPack> res(new FDO::memPack , CD3DFilter()) ;

	res->Data = new unsigned char[pMem->DataSize] ;

	//�M��

	memset(res->Data , 0 , pMem->DataSize) ;
		

	//�ƻs���
	memcpy(res->Data , pMem->Data , pMem->DataSize );
	//��J���
	res->DataSize = pMem->DataSize ; 
	//�����귽�j�p
	resize = pMem->DataSize ;

	return res ; 

}


//////////////////////			Ū�������Y�ʥ]			/////////////////

boost::shared_ptr<FDO::memPack> GetMemForPacket(const std::string &name ,unsigned int &resize)
{

	///////////////////////////////////////
    static FDO::PacketReader PR;

	FDO::memPack *data = NULL;

	//�q���Y�ɤ��A�����Y
    data = PR.LoadFromFile( name );	

//	Assert(data != NULL , "GetMemForPacket() error") ;

	//�إߤ@���O����϶���ơG�t�m�S�O���R����
	boost::shared_ptr<FDO::memPack> res(new FDO::memPack , CD3DFilter()) ;

	if (data == NULL)
	{
		res->Data = NULL ;
		res->DataSize = 0 ;


		GetFileStream("missres.txt").Wirte(name) ;



		return res ;
	}




	res->Data = new unsigned char[data->DataSize] ;

	//�M��

	memset(res->Data , 0 , data->DataSize) ;
	//ZeroMemory(res->Data , data->DataSize * sizeof(unsigned char) ) ;

	
	

	//�ƻs���
	memcpy(res->Data , data->Data , data->DataSize );
	//��J���
	res->DataSize = data->DataSize ; 
	//�����귽�j�p
	resize = data->DataSize ;

	return res ;

}

//////////////////////		�q�O����϶����Texture�åB�p��귽�j�p		/////////////////

//�ĤT�M�ĥ|�ӰѼƬ�out�C
void GetTexFormMem(unsigned char* data , unsigned int DataSize ,unsigned int &resize ,
				   IDirect3DTexture9 **tex)
{
	///////////			�q�O��Ϭq���AŪ��Texture		////////////
	

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
			Assert(0 , "���ߧA�I��ܥd�O�����z��") ;
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

	 //�åB�p��Textrue�j�p
	// GetTextrueResSize(resize , tex) ;


}

//�ĤT�ӰѼƩM�ĥ|�ӰѼƬ�out
void GetTexFormMemEx(unsigned char* data , unsigned int DataSize , 
				   unsigned int &resize , IDirect3DTexture9 **tex)
{

	//��Ex�@ŪTexture�覡
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

	 //�åB�p��Textrue�j�p
	 GetTextrueResSize(resize , tex) ;

}

//�إߤ@�Ӫť�Texture(RenderTarget��)
void GetTexture(unsigned int &resize , unsigned int width , unsigned int height , 
				IDirect3DTexture9 **tex)
{


	//�إߤ@�Ӫť�Texture�GRenderTarget��
	FDO::g_RenderSystem.GetD3DDevice()->CreateTexture(
		width , height , 1 ,
      D3DUSAGE_RENDERTARGET, 
	  FDO::g_RenderSystem.GetRTTFormat(),
      D3DPOOL_DEFAULT, tex , NULL ) ; 


	 Assert( (*tex) != NULL , "GetTexForMemEx Error") ;


	 //�åB�p��Textrue�j�p
	 GetTextrueResSize(resize , tex) ;


}


//���otexture���귽�O����j�p
void GetTextrueResSize(unsigned int &resize , IDirect3DTexture9 **tex)
{

	D3DSURFACE_DESC desc ;

	if((*tex) == NULL)
	{
		return ; 
	}

	 (*tex)->GetLevelDesc(0 , &desc) ; 

	 //////////				�P�_���ɮ榡		/////////////
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
	 default:	//��L
		 {
			resize = desc.Width * desc.Height ;
		 }break ;
	 }
}
