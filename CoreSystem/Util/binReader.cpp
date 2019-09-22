#include "stdafx.h"
#include "binReader.h"
#include <stdio.h>
#include <stdlib.h>
//#include <mem.h>

//---------------------------------------------------------------------------
FDO::binReader::binReader()
{
    _numCellsPerRow = 0;
    _numCellsPerCol = 0;

    _x = -1;
    _y = -1;
}
//---------------------------------------------------------------------------
FDO::binReader::~binReader()
{
}
//---------------------------------------------------------------------------
bool FDO::binReader::LoadFromFile( const isStr &FileName )
{
    FILE *FP;
    if( (FP=fopen(FileName.c_str(),"rb")) == 0 )
        return false;

    // 計算檔案大小
    fseek( FP, 0, SEEK_END);
    std::vector<unsigned char> in( ftell(FP) );
    fseek( FP, 0, SEEK_SET);

    // 讀取資料流中的資料
    fread( &in[0], 1, in.size(), FP );

    // 關閉一個已經開啟的檔案資料流
    fclose( FP );

    return LoadFromFileInMemory( &in[0], in.size() );
}

//---------------------------------------------------------------------------
bool FDO::binReader::LoadFromFileInMemory( unsigned char* Data,
      unsigned int DataSize )
{
    // 讀取標頭檔
    BR_Header* Header = (BR_Header*)Data;

    // 判定檔案格式
    if( strstr(Header->File_ID,"BIN") == NULL )
        return false;

    // 建立屬性資料
    _nfo.resize( Header->ExternCount );
    memcpy( &_nfo[0], Data+Header->ExternOffset,
        sizeof(BR_Extern_F) * Header->ExternCount );

    // 建立欄位資料
    _numCellsPerRow = Header->FieldCount;
    _row.resize( _numCellsPerRow );
    memcpy( &_row[0], Data+Header->FieldOffset,
        sizeof(BR_Field_F) * _numCellsPerRow );

    // 建立資料
    _numCellsPerCol = Header->DataCount;
    _data.resize( _numCellsPerRow * _numCellsPerCol );
    memcpy( &_data[0], Data+Header->DataOffset,
        sizeof(BR_Data_F) * _numCellsPerRow * _numCellsPerCol  );

    return true;
}

//---------------------------------------------------------------------------
// 取得第四個欄位
// ┌─┬─┬─┐
// │1 │  │3 │
// ├─┼─┼─┤
// │  │  │  │
// ├─┼─┼─┤
// │2 │  │4 │<- wanted!
// └─┴─┴─┘
//---------------------------------------------------------------------------
std::string FDO::binReader::FastFind( const isStr &FindField,
      const isStr &FindValue, const isStr &GetField )
{
    _x = getX( FindField );     // 由第一參數找到第二參數 x
    _y = getY( _x, FindValue ); // 由第二參數找到第四參數 y
    _x = getX( GetField  );     // 由第三參數找到第四參數 x

    return getValue( _x, _y );  // 至此已取得第四參數的 x y 位置
}

//---------------------------------------------------------------------------
std::string FDO::binReader::FindField( const isStr &GetField )
{
    int x = getX(GetField);

    return FindField(x);
}

//---------------------------------------------------------------------------
std::string FDO::binReader::FindField( const int &x )
{
    return getValue( x, _y );
}

//---------------------------------------------------------------------------
// 檢查範圍
//---------------------------------------------------------------------------
bool FDO::binReader::OutOfRange( const int &x, const int &y )
{
    if( x <  0 ||
        y <  0 ||
        x >= (int)_numCellsPerRow ||
        y >= (int)_numCellsPerCol )
        return true;
    else
        return false;
}

//---------------------------------------------------------------------------
// 在row找到參數的x位置
//---------------------------------------------------------------------------
int FDO::binReader::getX( const isStr &Value )
{
    for( unsigned int x=0; x<_numCellsPerRow; ++x )
        if( Value.compare(_row[x].FieldName) == 0 )
            return x;

    return -1;
}

//---------------------------------------------------------------------------
int FDO::binReader::getY( const int &x, const isStr &Value )
{
    // check range
    if( x<0 || x>=(int)_numCellsPerRow )
        return -1;

    static const isStr delims = " \t\r";
    isStr FindValue;

    int index = 0;
    for( unsigned int y=0; y<_numCellsPerCol; ++y )
    {
        index = y * _numCellsPerRow + x;
        FindValue = _data[index].Value;
        FindValue.erase(FindValue.find_last_not_of(delims)+1);
        //FindValue.erase(0, FindValue.find_first_not_of(delims));
        if( Value.compare(FindValue) == 0 )
            return y;
    }

    return -1;
}

//---------------------------------------------------------------------------
std::string FDO::binReader::getValue( const int &x, const int &y )
{
    if( OutOfRange(x,y) )
        return "";

    int index = y * _numCellsPerRow + x;
    return _data[index].Value;
}

//---------------------------------------------------------------------------
std::string FDO::binReader::FastFind( const isStr &FindField,
      const int &FindValue, const isStr &GetField )
{
    char FV[10];
    _itoa( FindValue, FV, 10 ); // 以 10 為底，將 int 轉 char

    return FastFind( FindField, FV, GetField );
}
