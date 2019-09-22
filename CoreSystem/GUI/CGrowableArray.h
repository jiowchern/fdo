#pragma once
#ifndef CGrowableArray_H
#define CGrowableArray_H

//#define NDEBUG
#include <assert.h>

#ifndef __max
#define __max(a,b) (((a)>(b))?(a):(b))
#endif
#ifndef __min
#define __min(a,b) (((a)<(b))?(a):(b))
#endif

//--------------------------------------------------------------------------------------
// A growable array
//--------------------------------------------------------------------------------------
template< typename TYPE >
class CGrowableArray
{
public:
    CGrowableArray()  {  }
    //CGrowableArray( const CGrowableArray<TYPE>& a ) { for( int i=0; i < a.m_nSize; i++ ) Add( a.m_pData[i] ); }
    ~CGrowableArray() { RemoveAll(); }

    const TYPE& operator[]( int nIndex ) const { return GetAt( nIndex ); }
    TYPE& operator[]( int nIndex ) { return GetAt( nIndex ); }
   
    CGrowableArray& operator=( const CGrowableArray<TYPE>& a ) { if( this == &a ) return *this; RemoveAll(); for( int i=0; i < a.m_nSize; i++ ) Add( a.m_pData[i] ); return *this; }

    HRESULT SetSize( int nNewMaxSize );
    HRESULT Add( const TYPE& value );
    HRESULT Insert( int nIndex, const TYPE& value );
    HRESULT SetAt( int nIndex, const TYPE& value );

	TYPE*	FindAt( int nIndex )
	{
		return mDatas[nIndex];
	}
    TYPE&   GetAt( int nIndex ) 
	{ 		
		return mDatas[nIndex]; 
	}
    int     GetSize() const { return (int)mDatas.size(); }
    //TYPE*   GetData() { return mDatas; }
    bool    Contains( const TYPE& value ){ return ( -1 != IndexOf( value ) ); }

    int     IndexOf( const TYPE& value ) { return ( mDatas.size() > 0 ) ? IndexOf( value, 0, (int)mDatas.size() ) : -1; }
    int     IndexOf( const TYPE& value, int iStart ) { return IndexOf( value, iStart, (int)mDatas.size() - iStart ); }
    int     IndexOf( const TYPE& value, int nIndex, int nNumElements );

    int     LastIndexOf( const TYPE& value ) { return ( mDatas.size() > 0 ) ? LastIndexOf( value, (int)mDatas.size()-1, (int)mDatas.size() ) : -1; }
    int     LastIndexOf( const TYPE& value, int nIndex ) { return LastIndexOf( value, nIndex, nIndex+1 ); }
    int     LastIndexOf( const TYPE& value, int nIndex, int nNumElements );

    HRESULT Remove( int nIndex );
    void    RemoveAll() { mDatas.clear(); }

protected:
	typedef std::vector<TYPE> TYPESET;
	TYPESET	mDatas;
    
};

//--------------------------------------------------------------------------------------
// Implementation of CGrowableArray
//--------------------------------------------------------------------------------------

// This version doesn't call ctor or dtor.



//--------------------------------------------------------------------------------------
template< typename TYPE >
HRESULT CGrowableArray<TYPE>::SetSize( int nNewMaxSize )
{
	mDatas.resize(nNewMaxSize);
//     int nOldSize = m_nSize;
// 
//     if( nOldSize > nNewMaxSize )
//     {
//         // Removing elements. Call dtor.
// 
//         for( int i = nNewMaxSize; i < nOldSize; ++i )
//             m_pData[i].~TYPE();
//     }
// 
//     // Adjust buffer.  Note that there's no need to check for error
//     // since if it happens, nOldSize == nNewMaxSize will be true.)
//     HRESULT hr = SetSizeInternal( nNewMaxSize );
// 
//     if( nOldSize < nNewMaxSize )
//     {
//         // Adding elements. Call ctor.
//         for( int i = nOldSize; i < nNewMaxSize; ++i )
//             ::new (&m_pData[i]) TYPE;
//     }

    return S_OK;
}


//--------------------------------------------------------------------------------------
template< typename TYPE >
HRESULT CGrowableArray<TYPE>::Add( const TYPE& value )
{
	assert(mDatas.size()<INT_MAX-1);
	mDatas.push_back(value);
//     HRESULT hr;
//     if( FAILED( hr = SetSizeInternal( m_nSize + 1 ) ) )
//         return hr;
// 
//     // Construct the new element
//     ::new (&m_pData[m_nSize]) TYPE;
// 
//     // Assign
//     m_pData[m_nSize] = value;
// 	
// 	mDatas.insert( make_pair(m_nSize, &m_pData[m_nSize]) );
//     ++m_nSize;

    return S_OK;
}


//--------------------------------------------------------------------------------------
template< typename TYPE >
HRESULT CGrowableArray<TYPE>::Insert( int nIndex, const TYPE& value )
{
	TYPESET::const_iterator it = mDatas.begin();
	mDatas.insert(it + nIndex , value);
    return S_OK;

//     // Validate index
//     if( nIndex < 0 || 
//         nIndex > m_nSize )
//     {
//         assert( false );
//         return E_INVALIDARG;
//     }
// 
//     // Prepare the buffer
//     if( FAILED( hr = SetSizeInternal( m_nSize + 1 ) ) )
//         return hr;
// 
//     // Shift the array
//     MoveMemory( &m_pData[nIndex+1], &m_pData[nIndex], sizeof(TYPE) * (m_nSize - nIndex) );
// 
//     // Construct the new element
//     ::new (&m_pData[nIndex]) TYPE;
// 
//     // Set the value and increase the size
//     m_pData[nIndex] = value;	
// 	mDatas.insert( make_pair(nIndex, &m_pData[nIndex]) );
//     ++m_nSize;
// 
//     return S_OK;
}


//--------------------------------------------------------------------------------------
template< typename TYPE >
HRESULT CGrowableArray<TYPE>::SetAt( int nIndex, const TYPE& value )
{
	if(nIndex < (int)mDatas.size() )
	{
		mDatas[nIndex] = value;
	}
	return S_OK;
//     // Validate arguments
//     if( nIndex < 0 ||
//         nIndex >= m_nSize )
//     {
//         assert( false );
//         return E_INVALIDARG;
//     }
// 
//     m_pData[nIndex] = value;
//     return S_OK;
}


//--------------------------------------------------------------------------------------
// Searches for the specified value and returns the index of the first occurrence
// within the section of the data array that extends from iStart and contains the 
// specified number of elements. Returns -1 if value is not found within the given 
// section.
//--------------------------------------------------------------------------------------
template< typename TYPE >
int CGrowableArray<TYPE>::IndexOf( const TYPE& value, int iStart, int nNumElements )
{
	int i = 0;
	for (	i = 0 + iStart ; 
			i < (int)mDatas.size() && i < nNumElements + iStart; 
			++i)
	{
		if(mDatas[i] != value)
		{
			continue;
		}
		return i;
	}
	return -1;
    // Validate arguments
//     if( iStart < 0 || 
//         iStart >= m_nSize ||
//         nNumElements < 0 ||
//         iStart + nNumElements > m_nSize )
//     {
//         assert( false );
//         return -1;
//     }
// 
//     // Search
//     for( int i = iStart; i < (iStart + nNumElements); i++ )
//     {
//         if( value == m_pData[i] )
//             return i;
//     }
// 
//     // Not found
//     return -1;
}


//--------------------------------------------------------------------------------------
// Searches for the specified value and returns the index of the last occurrence
// within the section of the data array that contains the specified number of elements
// and ends at iEnd. Returns -1 if value is not found within the given section.
//--------------------------------------------------------------------------------------
template< typename TYPE >
int CGrowableArray<TYPE>::LastIndexOf( const TYPE& value, int iEnd, int nNumElements )
{
	

    // Validate arguments
    if( iEnd < 0 || 
        iEnd >= (int)mDatas.size() ||
        nNumElements < 0 ||
        iEnd - nNumElements < 0 )
    {
        assert( false );
        return -1;
    }

    // Search
    for( int i = iEnd; i > (iEnd - nNumElements); i-- )
    {
        if( value == mDatas[i] )
            return i;
    }

    // Not found
    return -1;
}



//--------------------------------------------------------------------------------------
template< typename TYPE >
HRESULT CGrowableArray<TYPE>::Remove( int nIndex )
{
	mDatas.erase(mDatas.begin() + nIndex);
	return S_OK;

//     if( nIndex < 0 || 
//         nIndex >= m_nSize )
//     {
//         assert( false );
//         return E_INVALIDARG;
//     }
// 
//     // Destruct the element to be removed
//     m_pData[nIndex].~TYPE();
// 
//     // Compact the array and decrease the size
//     MoveMemory( &m_pData[nIndex], &m_pData[nIndex+1], sizeof(TYPE) * (m_nSize - (nIndex+1)) );
// //	mDatas.erase(nIndex);	
//     --m_nSize;
// 
// 	mDatas.clear();		
// 	for( sint i = 0 ; i < m_nSize ; ++i)
// 	{		
// 		mDatas.insert( make_pair( i , &m_pData[i]));
// 	}
// 
//     return S_OK;
}

#endif