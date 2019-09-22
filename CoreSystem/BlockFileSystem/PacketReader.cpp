#include "stdafx.h"
#include "iniReader.h"
#include "PhenixAssert.h"
#include <fstream>
#include <stdio.h>
#include <vector>
#pragma hdrstop
//---------------------------------------------------------------------------
#include "PacketReader.h"

//#define RESMGR


#ifdef RESMGR
#pragma message("管理模式啟動") 
#else
#pragma message("一般模式啟動") 
#endif

namespace FDO
{
//-----------------------------------------------------------------------
PacketReader::PacketReader()
{
    pMFileManager = NULL;
    pPFileManager = NULL;


    pMFileThreadManager = NULL;   
    pPFileThreadManager = NULL ; 

    Create();
}
//-----------------------------------------------------------------------
PacketReader::~PacketReader()
{
    Destroy();
}
//-----------------------------------------------------------------------
// 預設值是讀取封裝檔，
// 除非在 config.ini 設定 packet = false;
//-----------------------------------------------------------------------
bool PacketReader::Create()
{
    Destroy();

    std::string str = GetConfig( "_DEBUG" );
    if( str == "true" )
        m_bDebug = true;
    else
        m_bDebug = false;

    str = GetConfig( "packet" );
    if( str == "false" )
        ReadPacket = false;
    else
        ReadPacket = true;

    path = GetConfig( "path" );
    if( !path.empty() ) // 在最後加上 
        if( path.find_last_of("\\") != path.length()-1 )
            path += "\\";

    if( ReadPacket )
    {
        std::string FileName;

        // 初始主封裝檔(主封裝檔一定要有)
        FileName = path + "MainData.Dat";
        //MFileManager.reset( new CBlockFileManager );
        pMFileManager = new CBlockFileManager;
        Assert( pMFileManager != 0, "Call new failed !!" );

        if( !pMFileManager->LoadBfmFile(FileName.c_str(),100,1000) )
        {
            // 如果指定路徑沒有就試同目錄下
            if( !pMFileManager->LoadBfmFile("MainData.Dat",100,1000) )
            {
                return false;
            }

            path = "";
        }

        // 初始 PATCH 封裝檔
        FileName = path + "Patch\\UpDate.Dat";
        FILE *fp;
        if( ( fp=fopen( FileName.c_str(), "r" ) )!= 0 )
        {
            fclose( fp );
            //PFileManager.reset( new CBlockFileManager );
            pPFileManager = new CBlockFileManager;
            Assert( pPFileManager != 0, "Call new failed !!" );

            pPFileManager->LoadBfmFile( FileName.c_str(), 100, 1000 );
        }


		///////////////				Thread用			///////////////

		{
			// 初始主封裝檔(主封裝檔一定要有)
			FileName = path + "MainData.Dat";
			//MFileManager.reset( new CBlockFileManager );
			pMFileThreadManager = new CBlockFileManager;
			Assert( pMFileThreadManager != 0, "Call new failed !!" );

			if( !pMFileThreadManager->LoadBfmFile(FileName.c_str(),100,1000) )
			{
				// 如果指定路徑沒有就試同目錄下
				if( !pMFileThreadManager->LoadBfmFile("MainData.Dat",100,1000) )
				{
					return false;
				}

				path = "";
			}

			// 初始 PATCH 封裝檔
			FileName = path + "Patch\\UpDate.Dat";
			FILE *fp;
			if( ( fp=fopen( FileName.c_str(), "r" ) )!= 0 )
			{
				fclose( fp );
				//PFileManager.reset( new CBlockFileManager );
				pPFileThreadManager = new CBlockFileManager;
				Assert( pPFileThreadManager != 0, "Call new failed !!" );

				pPFileThreadManager->LoadBfmFile( FileName.c_str(), 100, 1000 );
			}

		}

		{
			// 初始主封裝檔(主封裝檔一定要有)
			FileName = path + "MainData.Dat";
			//MFileManager.reset( new CBlockFileManager );
			pMFileThreadManager2 = new CBlockFileManager;
			Assert( pMFileThreadManager2 != 0, "Call new failed !!" );

			if( !pMFileThreadManager2->LoadBfmFile(FileName.c_str(),100,1000) )
			{
				// 如果指定路徑沒有就試同目錄下
				if( !pMFileThreadManager2->LoadBfmFile("MainData.Dat",100,1000) )
				{
					return false;
				}

				path = "";
			}

			// 初始 PATCH 封裝檔
			FileName = path + "Patch\\UpDate.Dat";
			FILE *fp;
			if( ( fp=fopen( FileName.c_str(), "r" ) )!= 0 )
			{
				fclose( fp );
				//PFileManager.reset( new CBlockFileManager );
				pPFileThreadManager2 = new CBlockFileManager;
				Assert( pPFileThreadManager2 != 0, "Call new failed !!" );

				pPFileThreadManager2->LoadBfmFile( FileName.c_str(), 100, 1000 );
			}

		}


	{
			// 初始主封裝檔(主封裝檔一定要有)
			FileName = path + "MainData.Dat";
			//MFileManager.reset( new CBlockFileManager );
			pMFileThreadManager3 = new CBlockFileManager;
			Assert( pMFileThreadManager3 != 0, "Call new failed !!" );

			if( !pMFileThreadManager3->LoadBfmFile(FileName.c_str(),100,1000) )
			{
				// 如果指定路徑沒有就試同目錄下
				if( !pMFileThreadManager3->LoadBfmFile("MainData.Dat",100,1000) )
				{
					return false;
				}

				path = "";
			}

			// 初始 PATCH 封裝檔
			FileName = path + "Patch\\UpDate.Dat";
			FILE *fp;
			if( ( fp=fopen( FileName.c_str(), "r" ) )!= 0 )
			{
				fclose( fp );
				//PFileManager.reset( new CBlockFileManager );
				pPFileThreadManager3 = new CBlockFileManager;
				Assert( pPFileThreadManager3 != 0, "Call new failed !!" );

				pPFileThreadManager3->LoadBfmFile( FileName.c_str(), 100, 1000 );
			}

		}
    }

    return true;
}

//-----------------------------------------------------------------------
void PacketReader::Destroy()
{
    if( pMFileManager )
    {
        delete pMFileManager;
        pMFileManager = NULL;
    }

    if( pPFileManager )
    {
        delete pPFileManager;
        pPFileManager = NULL;
    }

    if( pMFileThreadManager )
    {
        delete pMFileThreadManager;
        pMFileThreadManager = NULL;
    }

    if( pPFileThreadManager )
    {
        delete pPFileThreadManager;
        pPFileThreadManager = NULL;
    }



    if( pMFileThreadManager2 )
    {
        delete pMFileThreadManager2;
        pMFileThreadManager2 = NULL;
    }

    if( pPFileThreadManager2 )
    {
        delete pPFileThreadManager2;
        pPFileThreadManager2 = NULL;
    }


    if( pMFileThreadManager3 )
    {
        delete pMFileThreadManager3;
        pMFileThreadManager3 = NULL;
    }

    if( pPFileThreadManager3 )
    {
        delete pPFileThreadManager3;
        pPFileThreadManager3 = NULL;
    }
}
//-----------------------------------------------------------------------
memPack *PacketReader::LoadFromBinFile(const std::string &FileName)
{
    static unsigned int MaxDataSize = 0;
    static memPack s_memPack;
    static std::vector<unsigned char> Data;

	char m_strPath[_MAX_PATH];
    char m_strDrive[_MAX_DRIVE];
    char m_strDir[_MAX_DIR];
    char m_strFName[_MAX_FNAME];
    char m_strExt[_MAX_EXT];
    strcpy( m_strPath, FileName.c_str() );
    _splitpath( m_strPath, m_strDrive, m_strDir, m_strFName, m_strExt );

    // 絕對路徑
    std::string Tempfilename;
	Tempfilename.assign( path );
	Tempfilename.append( FileName );

    ifstream is;
    is.open( AnsiToUnicode(Tempfilename.c_str()), ios::binary );
    if( is.is_open() )
    {
        is.seekg (0, ios::end);
        s_memPack.DataSize = is.tellg();
        is.seekg (0, ios::beg);

        // 紀錄最大使用容量
        if( MaxDataSize < s_memPack.DataSize )
        {
            MaxDataSize = s_memPack.DataSize; 
            Data.resize( MaxDataSize ); // 重置新容量
        }

        s_memPack.Data = &Data.front(); // 重置指標位置

#ifdef RESMGR

			//cosiann
			ZeroMemory(s_memPack.Data , sizeof(char) * Data.size());

#else

#endif

		if(s_memPack.DataSize) // 防意外當機
			is.read( (char*)s_memPack.Data, s_memPack.DataSize );
        is.close();

        return &s_memPack;
    }
    else
    {
        if( m_bDebug )
        {
            Debug( "Can not find: " + FileName );
        }

        return NULL;
    }
}
//-----------------------------------------------------------------------



memPack *PacketReader::LoadFromFileEx2(const std::string &FileName)
{
	//跟LoadFormFile的一樣作法。我懶得改，主要給Thread用
   int Handle;
    static unsigned int MaxDataSize = 0;
    static memPack s_memPack;
    static std::vector<unsigned char> Data;

    // 讀取封裝檔
    if( ReadPacket )
    {
        // 檢查 Patch 及 MainData 兩個，順序不能顛倒
        CBlockFileManager *pBFM = 0;
        for( int i=0; i<2; ++i )
        {
            if( i )
                //pBFM = MFileManager.get(); // 再檢查 MainData
                pBFM = pMFileThreadManager3;
            else
                //pBFM = PFileManager.get(); // 先檢查 Patch
                pBFM = pPFileThreadManager3;

            if( !pBFM )
                continue;

            // 必須是相對路徑
            if( (Handle=pBFM->OpenFile(FileName.c_str(),true)) == -1 )
            {
                if(i) // 搜尋至主封裝檔也找不到
                {
                    if( m_bDebug )
                    {
                        if( FileName.find( "Texture\\" ) == FileName.npos )
                            Debug( "Can not find: " + FileName );
                    }

                    return 0; // 讀取失敗
                }
            }
            else
            {
                s_memPack.DataSize = pBFM->GetFileSize( Handle );

                // 紀錄最大使用容量
                if( MaxDataSize < s_memPack.DataSize )
                {
                    MaxDataSize = s_memPack.DataSize;

                    // 重置新容量
                    Data.resize( MaxDataSize );
                }


                s_memPack.Data = &Data[0]; // 重置指標位置

#ifdef RESMGR

				//cosiann
				ZeroMemory(s_memPack.Data , sizeof(char) * Data.size());

#else

#endif

				if(s_memPack.DataSize) // 防意外當機
					pBFM->ReadData( Handle, s_memPack.DataSize, s_memPack.Data );
                pBFM->CloseFile( Handle );
                return &s_memPack;
            }
        }
    }
    // 讀取散裝檔
    else
    {
        // 取得副檔名
        //std::string FileExt = FileName.substr( FileName.rfind(".") );
        //FileName.c_str();

        char m_strPath[_MAX_PATH];
        char m_strDrive[_MAX_DRIVE];
        char m_strDir[_MAX_DIR];
        char m_strFName[_MAX_FNAME];
        char m_strExt[_MAX_EXT];
        strcpy( m_strPath, FileName.c_str() );
        _splitpath( m_strPath, m_strDrive, m_strDir, m_strFName, m_strExt );

        /* if( FileExt.empty() )
        {
            if( m_bDebug )
                Debug( "Can not find: " + FileName );

            return 0;
        } */

        // 絕對路徑
        std::string filename( path + FileName );

        ifstream is;

		const WCHAR *wcFilename = AnsiToUnicodeThread2(filename.c_str()) ; 

        is.open( wcFilename , ios::binary );

        if( is.is_open() )
        {
            is.seekg (0, ios::end);
            s_memPack.DataSize = is.tellg();
            is.seekg (0, ios::beg);

            // 紀錄最大使用容量
            if( MaxDataSize < s_memPack.DataSize )
            {
                MaxDataSize = s_memPack.DataSize; 
                Data.resize( MaxDataSize  ); // 重置新容量

            }

            s_memPack.Data = &Data.front(); // 重置指標位置

#ifdef RESMGR

				//cosiann
				ZeroMemory(s_memPack.Data , sizeof(char) * Data.size());

#else

#endif

            is.read( (char*)s_memPack.Data, s_memPack.DataSize );
            is.close();

            return &s_memPack;
        }
        else
        {
            if( m_bDebug )
            {
                Debug( "Can not find: " + FileName );
            }

            return NULL;
        }
    }

    return NULL; // 在所有封裝檔中完全找不到檔案
}

memPack *PacketReader::LoadFromFileEx1(const std::string &FileName)
{
	//跟LoadFormFile的一樣作法。我懶得改，主要給Thread用
   int Handle;
    static unsigned int MaxDataSize = 0;
    static memPack s_memPack;
    static std::vector<unsigned char> Data;

    // 讀取封裝檔
    if( ReadPacket )
    {
        // 檢查 Patch 及 MainData 兩個，順序不能顛倒
        CBlockFileManager *pBFM = 0;
        for( int i=0; i<2; ++i )
        {
            if( i )
                //pBFM = MFileManager.get(); // 再檢查 MainData
                pBFM = pMFileThreadManager2;
            else
                //pBFM = PFileManager.get(); // 先檢查 Patch
                pBFM = pPFileThreadManager2;

            if( !pBFM )
                continue;

            // 必須是相對路徑
            if( (Handle=pBFM->OpenFile(FileName.c_str(),true)) == -1 )
            {
                if(i) // 搜尋至主封裝檔也找不到
                {
                    if( m_bDebug )
                    {
                        if( FileName.find( "Texture\\" ) == FileName.npos )
                            Debug( "Can not find: " + FileName );
                    }

                    return 0; // 讀取失敗
                }
            }
            else
            {
                s_memPack.DataSize = pBFM->GetFileSize( Handle );

                // 紀錄最大使用容量
                if( MaxDataSize < s_memPack.DataSize )
                {
                    MaxDataSize = s_memPack.DataSize;

                    // 重置新容量
                    Data.resize( MaxDataSize );
                }

                s_memPack.Data = &Data[0]; // 重置指標位置

#ifdef RESMGR

				//cosiann
				ZeroMemory(s_memPack.Data , sizeof(char) * Data.size());

#else

#endif
				if(s_memPack.DataSize) // 防意外當機
					pBFM->ReadData( Handle, s_memPack.DataSize, s_memPack.Data );
                pBFM->CloseFile( Handle );
                return &s_memPack;
            }
        }
    }
    // 讀取散裝檔
    else
    {
        // 取得副檔名
        //std::string FileExt = FileName.substr( FileName.rfind(".") );
        //FileName.c_str();

        char m_strPath[_MAX_PATH];
        char m_strDrive[_MAX_DRIVE];
        char m_strDir[_MAX_DIR];
        char m_strFName[_MAX_FNAME];
        char m_strExt[_MAX_EXT];
        strcpy( m_strPath, FileName.c_str() );
        _splitpath( m_strPath, m_strDrive, m_strDir, m_strFName, m_strExt );

        /* if( FileExt.empty() )
        {
            if( m_bDebug )
                Debug( "Can not find: " + FileName );

            return 0;
        } */

        // 絕對路徑
        std::string filename( path + FileName );

        ifstream is;

		const WCHAR *wcFilename = AnsiToUnicodeThread2(filename.c_str()) ; 

        is.open( wcFilename , ios::binary );

        if( is.is_open() )
        {
            is.seekg (0, ios::end);
            s_memPack.DataSize = is.tellg();
            is.seekg (0, ios::beg);

            // 紀錄最大使用容量
            if( MaxDataSize < s_memPack.DataSize )
            {
                MaxDataSize = s_memPack.DataSize; 
                Data.resize( MaxDataSize  ); // 重置新容量

            }

            s_memPack.Data = &Data.front(); // 重置指標位置

#ifdef RESMGR

				//cosiann
				ZeroMemory(s_memPack.Data , sizeof(char) * Data.size());

#else

#endif

            is.read( (char*)s_memPack.Data, s_memPack.DataSize );
            is.close();

            return &s_memPack;
        }
        else
        {
            if( m_bDebug )
            {
                Debug( "Can not find: " + FileName );
            }

            return NULL;
        }
    }

    return NULL; // 在所有封裝檔中完全找不到檔案
}

memPack *PacketReader::LoadFromFileEx(const std::string &FileName)
{
	//跟LoadFormFile的一樣作法。我懶得改，主要給Thread用
   int Handle;
    static unsigned int MaxDataSize = 0;
    static memPack s_memPack;
    static std::vector<unsigned char> Data;

    // 讀取封裝檔
    if( ReadPacket )
    {
        // 檢查 Patch 及 MainData 兩個，順序不能顛倒
        CBlockFileManager *pBFM = 0;
        for( int i=0; i<2; ++i )
        {
            if( i )
                //pBFM = MFileManager.get(); // 再檢查 MainData
                pBFM = pMFileThreadManager;
            else
                //pBFM = PFileManager.get(); // 先檢查 Patch
                pBFM = pPFileThreadManager;

            if( !pBFM )
                continue;

            // 必須是相對路徑
            if( (Handle=pBFM->OpenFile(FileName.c_str(),true)) == -1 )
            {
                if(i) // 搜尋至主封裝檔也找不到
                {
                    if( m_bDebug )
                    {
                        if( FileName.find( "Texture\\" ) == FileName.npos )
                            Debug( "Can not find: " + FileName );
                    }

                    return 0; // 讀取失敗
                }
            }
            else
            {
                s_memPack.DataSize = pBFM->GetFileSize( Handle );

                // 紀錄最大使用容量
                if( MaxDataSize < s_memPack.DataSize )
                {
                    MaxDataSize = s_memPack.DataSize;

                    // 重置新容量
                    Data.resize( MaxDataSize );
                }

                s_memPack.Data = &Data[0]; // 重置指標位置

#ifdef RESMGR

				//cosiann
				ZeroMemory(s_memPack.Data , sizeof(char) * Data.size());

#else

#endif

				if(s_memPack.DataSize) // 防意外當機
					pBFM->ReadData( Handle, s_memPack.DataSize, s_memPack.Data );
                pBFM->CloseFile( Handle );
                return &s_memPack;
            }
        }
    }
    // 讀取散裝檔
    else
    {
        // 取得副檔名
        //std::string FileExt = FileName.substr( FileName.rfind(".") );
        //FileName.c_str();

        char m_strPath[_MAX_PATH];
        char m_strDrive[_MAX_DRIVE];
        char m_strDir[_MAX_DIR];
        char m_strFName[_MAX_FNAME];
        char m_strExt[_MAX_EXT];
        strcpy( m_strPath, FileName.c_str() );
        _splitpath( m_strPath, m_strDrive, m_strDir, m_strFName, m_strExt );

        /* if( FileExt.empty() )
        {
            if( m_bDebug )
                Debug( "Can not find: " + FileName );

            return 0;
        } */

        // 絕對路徑
        std::string filename( path + FileName );

        ifstream is;

		const WCHAR *wcFilename = AnsiToUnicodeThread1(filename.c_str()) ;

        is.open(wcFilename , ios::binary );
        if( is.is_open() )
        {
            is.seekg (0, ios::end);
            s_memPack.DataSize = is.tellg();
            is.seekg (0, ios::beg);

            // 紀錄最大使用容量
            if( MaxDataSize < s_memPack.DataSize )
            {
                MaxDataSize = s_memPack.DataSize; 
                Data.resize( MaxDataSize  ); // 重置新容量

            }

            s_memPack.Data = &Data.front(); // 重置指標位置

#ifdef RESMGR

				//cosiann
				ZeroMemory(s_memPack.Data , sizeof(char) * Data.size());

#else

#endif

            is.read( (char*)s_memPack.Data, s_memPack.DataSize );
            is.close();

            return &s_memPack;
        }
        else
        {
            if( m_bDebug )
            {
                Debug( "Can not find: " + FileName );
            }

            return NULL;
        }
    }

    return NULL; // 在所有封裝檔中完全找不到檔案
}

memPack *PacketReader::LoadFromFile( const std::string &FileName )
{           
    int Handle;
    static unsigned int MaxDataSize = 0;
    static memPack s_memPack;
    static std::vector<unsigned char> Data;

    // 讀取封裝檔
    if( ReadPacket )
    {
        // 檢查 Patch 及 MainData 兩個，順序不能顛倒
        CBlockFileManager *pBFM = 0;
        for( int i=0; i<2; ++i )
        {
            if( i )
                //pBFM = MFileManager.get(); // 再檢查 MainData
                pBFM = pMFileManager;
            else
                //pBFM = PFileManager.get(); // 先檢查 Patch
                pBFM = pPFileManager;

            if( !pBFM )
                continue;

            // 必須是相對路徑
            if( (Handle=pBFM->OpenFile(FileName.c_str(),true)) == -1 )
            {
                if(i) // 搜尋至主封裝檔也找不到
                {
                    if( m_bDebug )
                    {
                        if( FileName.find( "Texture\\" ) == FileName.npos )
                            Debug( "Can not find: " + FileName );
                    }

                    return 0; // 讀取失敗
                }
            }
            else
            {
                s_memPack.DataSize = pBFM->GetFileSize( Handle );

                // 紀錄最大使用容量
                if( MaxDataSize < s_memPack.DataSize )
                {
                    MaxDataSize = s_memPack.DataSize;

                    // 重置新容量
                    Data.resize( MaxDataSize );
                }

                s_memPack.Data = &Data[0]; // 重置指標位置

#ifdef RESMGR

				//cosiann
				ZeroMemory(s_memPack.Data , sizeof(char) * Data.size());

#else

#endif

				if(s_memPack.DataSize) // 防意外當機
					pBFM->ReadData( Handle, s_memPack.DataSize, s_memPack.Data );
                pBFM->CloseFile( Handle );
                return &s_memPack;
            }
        }
    }
    // 讀取散裝檔
    else
    {
        // 取得副檔名
        //std::string FileExt = FileName.substr( FileName.rfind(".") );
        //FileName.c_str();

        char m_strPath[_MAX_PATH];
        char m_strDrive[_MAX_DRIVE];
        char m_strDir[_MAX_DIR];
        char m_strFName[_MAX_FNAME];
        char m_strExt[_MAX_EXT];
        strcpy( m_strPath, FileName.c_str() );
        _splitpath( m_strPath, m_strDrive, m_strDir, m_strFName, m_strExt );

        /* if( FileExt.empty() )
        {
            if( m_bDebug )
                Debug( "Can not find: " + FileName );

            return 0;
        } */

        // 絕對路徑
        std::string filename( path + FileName );

        ifstream is;
        is.open( AnsiToUnicode(filename.c_str()), ios::binary );
        if( is.is_open() )
        {
            is.seekg (0, ios::end);
            s_memPack.DataSize = is.tellg();
            is.seekg (0, ios::beg);

            // 紀錄最大使用容量
            if( MaxDataSize < s_memPack.DataSize )
            {
                MaxDataSize = s_memPack.DataSize; 
                Data.resize( MaxDataSize  ); // 重置新容量

            }

            s_memPack.Data = &Data.front(); // 重置指標位置

#ifdef RESMGR

				//cosiann
				ZeroMemory(s_memPack.Data , sizeof(char) * Data.size());

#else

#endif

            is.read( (char*)s_memPack.Data, s_memPack.DataSize );
            is.close();

            return &s_memPack;
        }
        else
        {
            if( m_bDebug )
            {
                Debug( "Can not find: " + FileName );
            }

            return NULL;
        }
    }

    return NULL; // 在所有封裝檔中完全找不到檔案
}

//-----------------------------------------------------------------------
void PacketReader::Debug(const std::string &FileName)
{
	_LOG(1,  AnsiToUnicode(FileName.c_str()));
 /*   static const char DEBUG_FILE[20] = "DebugInfo.txt";

    FILE *fp = fopen(DEBUG_FILE, "a+");
    fprintf( fp, "%s\n", FileName.c_str() );
    fclose(fp);*/
}

bool memPack::IsValid() const
{
    return ((Data != 0) && (DataSize != 0));
}

//-----------------------------------------------------------------------
// 由封裝檔取得資料
//-----------------------------------------------------------------------
memPack *LoadFromPacket( const std::string &FileName )
{

#ifdef RESMGR

	return LoadResource<memPack , ResMemObj>(FileName.c_str()) ;

#else
    static PacketReader PR;

    return PR.LoadFromFile( FileName );
#endif
}
//-----------------------------------------------------------------------
memPack *LoadFromBinFile( const std::string &FileName )
{
    static PacketReader PR;

	return PR.LoadFromBinFile( FileName );
}
//-----------------------------------------------------------------------
}