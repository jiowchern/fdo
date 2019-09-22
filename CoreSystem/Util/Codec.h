//---------------------------------------------------------------------------
#ifndef _CODEC_H
#define _CODEC_H

#include <windows.H>

const int 	_MaxPasswordSize = 64;
const int 	_StoreKeySize = 64;
const int 	_CodeHeadSize = 4;
const BYTE 	_XorCode = 0x2c;
const BYTE 	_SumCode = 35;

class cCodec
{
	private:
		bool InitialData;
		char *StoreKeyWord;
		BYTE *StoreKeyMode;
		int  StoreKeySize;
		char *Password;
		int  PasswordMaxSize;
		BYTE *CodeData;
		int  CodeDataMaxSize;
		int  DataLength;
		int  KeyByteCount; 
		int  KeyBitCount;
		bool GetKey( void );
		int  GetSkipBit( int *SkipBitCount );
		BYTE *EncodeCore( void *Word, int ByteCount, BYTE SumCode = 0, BYTE XorCode = 0 );
		char GetData( BYTE *Data, int *DataOffset, int *DataOffsetBit, BYTE SumCode , BYTE XorCode );
	public:
		cCodec( void );
		~cCodec( void );
		void Close( void );
		bool Create( char KeyWord[], int KeyBit, int MaxPasswordSize, int MaxDataSize );
		BYTE *Encode( char *Word, BYTE SumCode = 0, BYTE XorCode = 0 );
		BYTE *EncodeInt( int Number, BYTE SumCode = 0, BYTE XorCode = 0 );
		BYTE *EncodeShort( short Number, BYTE SumCode = 0, BYTE XorCode = 0 );
		char *Decode( BYTE *PasData, BYTE *Data, BYTE SumCode = 0, BYTE XorCode = 0 );
		bool DecodeInt( BYTE *Data, int *Number, BYTE SumCode = 0, BYTE XorCode = 0 );
		bool DecodeShort( BYTE *Data, short *Number, BYTE SumCode = 0, BYTE XorCode = 0 );
		bool ChangeKeyWord( char KeyWord[] );
};

//---------------------------------------------------------------------------
#endif
