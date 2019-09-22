#ifndef BINREADER_H
#define BINREADER_H
//---------------------------------------------------------------------------

#include <vector>
#include <string>


namespace FDO
{

    class binReader
    {
    private:

        struct BR_Header
        {
            char            File_ID[3];     //檔案視別
            unsigned char   Verson;         //版本視別
            int             ExternCount;    //屬性總數
            unsigned int    ExternOffset;   //屬性檔案位置
            int             FieldCount;     //欄位總數
            unsigned int    FieldOffset;    //欄位檔案位置
            int             DataCount;      //資料總數
            unsigned int    DataOffset;     //資料檔案位置
        };

        struct BR_Extern_F
        {
            char            Name[255];
            int             Type;
            char            Value[255];
        };

        struct BR_Field_F
        {
            char            FieldName[255]; //欄位名稱
            int             FieldType;      //欄位型態
        };

        struct BR_Data_F
        {
            char            Value[255];     //資料
        };

        typedef std::string                 isStr;
        typedef std::vector<BR_Extern_F>    nfo;
        typedef std::vector<BR_Field_F>     row;
        typedef std::vector<BR_Data_F>      data;

        nfo  _nfo;
        row  _row;
        data _data;

        unsigned int _numCellsPerRow;
    	unsigned int _numCellsPerCol;

        int _x, _y; // 紀錄目前位置

        // help function
        bool OutOfRange( const int &x, const int &y );
        int  getX( const isStr &Value );
        int  getY( const int &x, const isStr &Value );
        isStr getValue( const int &x, const int &y );

    public:

         binReader();
        ~binReader();

        bool LoadFromFile( const isStr &FileName );
        bool LoadFromFileInMemory( unsigned char* Data, unsigned int DataSize );

        // 取得第四個欄位
        isStr FastFind( const isStr &FindField, const isStr &FindValue, const isStr &GetField );
        isStr FastFind( const isStr &FindField, const int   &FindValue, const isStr &GetField );

        // 由目前y位置取得第四個欄位
        isStr FindField( const isStr &GetField );
        isStr FindField( const int &x );

        int GetNumCellsPerRow() { return _numCellsPerRow; }
        int GetNumCellsPerCol() { return _numCellsPerCol; }

        void First() { _y=0; }
        void Next()  { ++_y; }

    };

}


//---------------------------------------------------------------------------
#endif // BINREADER_H