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

    // �p���ɮפj�p
    fseek( FP, 0, SEEK_END);
    std::vector<unsigned char> in( ftell(FP) );
    fseek( FP, 0, SEEK_SET);

    // Ū����Ƭy�������
    fread( &in[0], 1, in.size(), FP );

    // �����@�Ӥw�g�}�Ҫ��ɮ׸�Ƭy
    fclose( FP );

    return LoadFromFileInMemory( &in[0], in.size() );
}

//---------------------------------------------------------------------------
bool FDO::binReader::LoadFromFileInMemory( unsigned char* Data,
      unsigned int DataSize )
{
    // Ū�����Y��
    BR_Header* Header = (BR_Header*)Data;

    // �P�w�ɮ׮榡
    if( strstr(Header->File_ID,"BIN") == NULL )
        return false;

    // �إ��ݩʸ��
    _nfo.resize( Header->ExternCount );
    memcpy( &_nfo[0], Data+Header->ExternOffset,
        sizeof(BR_Extern_F) * Header->ExternCount );

    // �إ������
    _numCellsPerRow = Header->FieldCount;
    _row.resize( _numCellsPerRow );
    memcpy( &_row[0], Data+Header->FieldOffset,
        sizeof(BR_Field_F) * _numCellsPerRow );

    // �إ߸��
    _numCellsPerCol = Header->DataCount;
    _data.resize( _numCellsPerRow * _numCellsPerCol );
    memcpy( &_data[0], Data+Header->DataOffset,
        sizeof(BR_Data_F) * _numCellsPerRow * _numCellsPerCol  );

    return true;
}

//---------------------------------------------------------------------------
// ���o�ĥ|�����
// �z�w�s�w�s�w�{
// �x1 �x  �x3 �x
// �u�w�q�w�q�w�t
// �x  �x  �x  �x
// �u�w�q�w�q�w�t
// �x2 �x  �x4 �x<- wanted!
// �|�w�r�w�r�w�}
//---------------------------------------------------------------------------
std::string FDO::binReader::FastFind( const isStr &FindField,
      const isStr &FindValue, const isStr &GetField )
{
    _x = getX( FindField );     // �ѲĤ@�ѼƧ��ĤG�Ѽ� x
    _y = getY( _x, FindValue ); // �ѲĤG�ѼƧ��ĥ|�Ѽ� y
    _x = getX( GetField  );     // �ѲĤT�ѼƧ��ĥ|�Ѽ� x

    return getValue( _x, _y );  // �ܦ��w���o�ĥ|�Ѽƪ� x y ��m
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
// �ˬd�d��
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
// �brow���Ѽƪ�x��m
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
    _itoa( FindValue, FV, 10 ); // �H 10 �����A�N int �� char

    return FastFind( FindField, FV, GetField );
}
