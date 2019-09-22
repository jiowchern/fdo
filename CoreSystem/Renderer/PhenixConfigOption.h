//---------------------------------------------------------------------------
#ifndef _PhenixConfigOption_H__
#define _PhenixConfigOption_H__

#include <string>
#include <map>

namespace FDO {

    typedef std::vector<std::string> StringVector;

    typedef struct _ConfigOption
    {
        std::string name;
        std::string currentValue;
        StringVector possibleValues;
    } ConfigOption;

    typedef std::map< std::string, ConfigOption > ConfigOptionMap;
}

#endif
