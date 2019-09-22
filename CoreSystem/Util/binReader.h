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
            char            File_ID[3];     //�ɮ׵��O
            unsigned char   Verson;         //�������O
            int             ExternCount;    //�ݩ��`��
            unsigned int    ExternOffset;   //�ݩ��ɮצ�m
            int             FieldCount;     //����`��
            unsigned int    FieldOffset;    //����ɮצ�m
            int             DataCount;      //����`��
            unsigned int    DataOffset;     //����ɮצ�m
        };

        struct BR_Extern_F
        {
            char            Name[255];
            int             Type;
            char            Value[255];
        };

        struct BR_Field_F
        {
            char            FieldName[255]; //���W��
            int             FieldType;      //��쫬�A
        };

        struct BR_Data_F
        {
            char            Value[255];     //���
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

        int _x, _y; // �����ثe��m

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

        // ���o�ĥ|�����
        isStr FastFind( const isStr &FindField, const isStr &FindValue, const isStr &GetField );
        isStr FastFind( const isStr &FindField, const int   &FindValue, const isStr &GetField );

        // �ѥثey��m���o�ĥ|�����
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