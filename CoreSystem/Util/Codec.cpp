//---------------------------------------------------------------------------
#include "stdafx.h"
#include "Codec.h"

//---------------------------------------------------------------------------
cCodec::cCodec( void )
{
	InitialData = false;
	StoreKeyWord = NULL;
	StoreKeyMode = NULL;
	Password = NULL;
	CodeData = NULL;
}

//---------------------------------------------------------------------------
cCodec::~cCodec( void )
{
	Close();
}

//---------------------------------------------------------------------------
//							清除密碼資料
void cCodec::Close( void )
{
	if( StoreKeyWord  )
	{
		free( StoreKeyWord );
		StoreKeyWord = NULL;
	}
	if( StoreKeyMode )
	{
		free( StoreKeyMode );
		StoreKeyMode = NULL;
	}
	if( Password )
	{
		free( Password );
		Password = NULL;
	}
	if( CodeData )
	{
		free( CodeData );
		CodeData = NULL;
	}
	InitialData = false;
}

//---------------------------------------------------------------------------
//							密碼資料初始化
bool cCodec::Create( char KeyWord[], int KeyBit, int MaxPasswordSize, int MaxDataSize )
{
	Close();
	// 檢查Key字串
	StoreKeySize = strlen( KeyWord );
	if( StoreKeySize <= 0 || StoreKeySize >= _StoreKeySize ) return( false );
	StoreKeyWord = (char *)malloc( StoreKeySize + 1 );
	strcpy( StoreKeyWord, KeyWord );
	// 檢查與計算KeyBit
	if( KeyBit < 1 || KeyBit > 256 ) return( false );
	BYTE *Data = (BYTE *)StoreKeyWord;
	StoreKeyMode = (BYTE *)malloc( StoreKeySize );
	for( int Loop = 0; Loop < StoreKeySize; Loop++ )
	{
		StoreKeyMode[Loop] = Data[Loop] % KeyBit;
	}
	// 檢查最大密碼長度
	if( MaxPasswordSize < 1 || MaxPasswordSize >= _MaxPasswordSize ) return( false );
	PasswordMaxSize = MaxPasswordSize;
	Password = (char *)malloc( MaxPasswordSize + 1 );
	// 檢查最大資料長度
	if( MaxDataSize < 1 ) return( false );
	CodeDataMaxSize = MaxDataSize;
	CodeData = (BYTE *)malloc( MaxDataSize + 1 );

	InitialData = true;
	return( true );
}

//---------------------------------------------------------------------------
//									編碼
BYTE *cCodec::Encode( char *Word, BYTE SumCode, BYTE XorCode )
{
	return( EncodeCore( Word, strlen( Word ), SumCode, XorCode ) );
}

BYTE *cCodec::EncodeInt( int Number, BYTE SumCode, BYTE XorCode )
{
	return( EncodeCore( &Number, 4, SumCode, XorCode ) );
}

BYTE *cCodec::EncodeShort( short Number, BYTE SumCode, BYTE XorCode )
{
	return( EncodeCore( &Number, 2, SumCode, XorCode ) );
}

BYTE *cCodec::EncodeCore( void *Word, int ByteCount, BYTE SumCode, BYTE XorCode )
{
	BYTE BitData[] = { 1, 2, 4, 8, 16, 32, 64, 128 };
	if( InitialData == false ) return( NULL );
	int StrSize = ByteCount;
	if( StrSize <= 0 || StrSize > PasswordMaxSize ) return( NULL );
	memmove( Password, Word, ByteCount );

	if( SumCode == 0 ) SumCode = _SumCode;
	if( XorCode == 0 ) XorCode = _XorCode;
	BYTE *Pass = (BYTE *)Password;
	CodeData[2] = (BYTE)StrSize;
	CodeData[3] = 0;
	WORD DataLength = _CodeHeadSize;
	int SkipBitCount = 0;
	KeyByteCount = 0;
	KeyBitCount = 0;
	// BYTE 迴圈
	int Bit = 0;
	BYTE *Data = CodeData + DataLength;
	*Data = 0;
	for( int ByteLoop =0; ByteLoop < StrSize; ByteLoop++ )
	{
		// BIT迴圈
		for( int BitLoop = 0; BitLoop < 8; BitLoop++ )
		{
			// Skip迴圈
			int SkipBit = GetSkipBit( &SkipBitCount );
			for( int SkipLoop = 0; SkipLoop < SkipBit; SkipLoop++ )
			{
			  if( GetKey() )
			  {
				*Data |= BitData[Bit];
			  }
			  Bit++;
			  if( Bit >= 8 )
			  {
				*Data -= SumCode;
				*Data ^= XorCode;
				Bit = 0;
				DataLength++;
				if( DataLength >= CodeDataMaxSize - 1 ) return( NULL );
				Data = CodeData + DataLength;
				*Data = 0;
			  }
			}
			// 填入Password Bit
			if( Pass[ByteLoop] & BitData[BitLoop] )
			{
				CodeData[3]++;
				*Data |= BitData[Bit];
			}
			Bit++;
			if( Bit >= 8 )
			{
				*Data -= SumCode;
				*Data ^= XorCode;
				Bit = 0;
				DataLength++;
				if( DataLength >= CodeDataMaxSize - 1 ) return( NULL );
				Data = CodeData + DataLength;
				*Data = 0;
			}
		}
	}
	if( Bit > 0 )
	{
		*Data -= SumCode;
		*Data ^= XorCode;
	}
	*(WORD *)CodeData = DataLength + 1;
	return( CodeData );
}

//---------------------------------------------------------------------------
//									解碼
bool cCodec::DecodeInt( BYTE *Data, int *Number, BYTE SumCode, BYTE XorCode )
{
	if( Data[2] != 4 ) return( false );
	BYTE Buf[_MaxPasswordSize];
	char *Ptr = Decode( Buf, Data, SumCode, XorCode );
	if( Ptr == NULL ) return( false );
	else
	{
		*Number = *(int *)Ptr;
		return( true );
	}
}

bool cCodec::DecodeShort( BYTE *Data, short *Number, BYTE SumCode, BYTE XorCode )
{
	if( Data[2] != 2 ) return( false );
	BYTE Buf[_MaxPasswordSize];
	char *Ptr = Decode( Buf, Data, SumCode, XorCode );
	if( Ptr == NULL ) return( false );
	else
	{
		*Number = *(short *)Ptr;
		return( true );
	}
}

char *cCodec::Decode( BYTE *PasData, BYTE *Data, BYTE SumCode , BYTE XorCode )
{
	BYTE BitData[] = { 1, 2, 4, 8, 16, 32, 64, 128 };
	if( InitialData == false ) return( NULL );
	int StrSize = Data[2];
	if( StrSize <= 0 || StrSize >= PasswordMaxSize ) return( NULL );

	if( SumCode == 0 ) SumCode = _SumCode;
	if( XorCode == 0 ) XorCode = _XorCode;
	int DataOffset = _CodeHeadSize;
	int DataOffsetBit = 0;
	int SkipBitCount = 0;
	BYTE CheckSum = 0, CheckSumCount = 0;
	int KeyCount = 0;
	BYTE Bit1Count = 0;
	BYTE *KeyWord = (BYTE *)StoreKeyWord;
	char Ret;
	// BYTE 迴圈
	for( int ByteLoop = 0; ByteLoop < StrSize; ByteLoop++ )
	{
		PasData[ByteLoop] = 0;
		// BIT迴圈
		for( int BitLoop = 0; BitLoop < 8; BitLoop++ )
		{
			// Skip迴圈
			int SkipBit = GetSkipBit( &SkipBitCount );
			for( int SkipLoop = 0; SkipLoop < SkipBit; SkipLoop++ )
			{
				Ret = GetData( Data, &DataOffset, &DataOffsetBit, SumCode, XorCode );
				if( Ret == -1 ) return( NULL );
				else if( Ret )
				{
					CheckSum |= BitData[CheckSumCount];
				}
				CheckSumCount++;
				if( CheckSumCount >= 8 )
				{
					if( CheckSum != KeyWord[KeyCount] ) return( NULL );
					CheckSumCount = 0;
					CheckSum = 0;
					KeyCount++;
					if( KeyCount >= StoreKeySize )
					{
						KeyCount = 0;
					}
				}
			}
			// 讀取密碼
			Ret = GetData( Data, &DataOffset, &DataOffsetBit, SumCode, XorCode );
			if( Ret == -1 ) return( NULL );
			else if( Ret )
			{
				Bit1Count++;
				PasData[ByteLoop] |= BitData[BitLoop];
			}
		}
	}
	if( Bit1Count != Data[3] )
	{
		return( NULL );
	}
	PasData[StrSize] = 0;
	return( (char *)PasData );
}

//---------------------------------------------------------------------------
//								取得略過的Bit數
int cCodec::GetSkipBit( int *SkipBitCount )
{
	int Ret = StoreKeyMode[*SkipBitCount];
	(*SkipBitCount)++;
	if( (*SkipBitCount) >= StoreKeySize )
	{
		*SkipBitCount = 0;
	}
	return( Ret );
}

//---------------------------------------------------------------------------
//								從Key中取一個Bit
bool cCodec::GetKey( void )
{
	BYTE BitData[] = { 1, 2, 4, 8, 16, 32, 64, 128 };

	BYTE *Key = (BYTE *)StoreKeyWord;
	bool Ret = 0!=((Key[KeyByteCount] & BitData[KeyBitCount]));

	KeyBitCount++;
	if( KeyBitCount >= 8 )
	{
		KeyBitCount = 0;
		KeyByteCount++;
		if( KeyByteCount >= StoreKeySize )
		{
			KeyByteCount = 0;
		}
	}
	return( Ret );
}

//---------------------------------------------------------------------------
//								從資料中取一個Bit
char cCodec::GetData( BYTE *Data, int *DataOffset, int *DataOffsetBit, BYTE SumCode, BYTE XorCode )
{
	BYTE BitData[] = { 1, 2, 4, 8, 16, 32, 64, 128 };
	if( *DataOffset >= *(WORD *)Data ) return( -1 );
	int Ret = ((Data[*DataOffset] ^ XorCode) + SumCode) & BitData[*DataOffsetBit];
	(*DataOffsetBit)++;
	if( *DataOffsetBit >= 8 )
	{
		*DataOffsetBit = 0;
		(*DataOffset)++;
	}
	if( Ret != 0 ) return( 1 );
	else return( 0 );
}

//---------------------------------------------------------------------------
//							密碼資料初始化
bool cCodec::ChangeKeyWord( char KeyWord[] )
{
	if( StoreKeyWord )
	{
		free( StoreKeyWord );
		StoreKeyWord = NULL;
	}
	// 檢查Key字串
	StoreKeySize = strlen( KeyWord );
	if( StoreKeySize <= 0 || StoreKeySize >= _StoreKeySize ) return( false );
	StoreKeyWord = (char *)malloc( StoreKeySize + 1 );
	strcpy( StoreKeyWord, KeyWord );
	return( true );
}
