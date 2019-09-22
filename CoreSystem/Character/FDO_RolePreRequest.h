/**
 * @file FDO_RolePreRequest.h
 * @author Yuming Ho
 * @brief Forward declarations
 */
#ifndef FDO_RolePreRequestH
#define FDO_RolePreRequestH

#include <list>

namespace FDO
{

struct sDirHeader;
struct sEmoHeader;
struct sImage;
struct sFrame;
struct sComponentDir;
struct sRoleComponent;
struct sRoleAction;

typedef sImage          RoleImg, *RoleImgPtr;
typedef sFrame          RoleFrm, *RoleFrmPtr;
//typedef sComponentDir   RoleDir, *RoleDirPtr;
typedef sRoleComponent  RoleCom, *RoleComPtr;
typedef sRoleAction     RoleAct, *RoleActPtr;

typedef std::list<RoleFrmPtr>   sFrameList;
typedef std::list<RoleImgPtr>   sImageList;
typedef std::list<RoleComPtr>   sRoleComponentList;
typedef std::list<RoleActPtr>   sRoleActionList;

typedef std::list<RoleImgPtr>   RoleImgList;
typedef sFrameList              RoleFrmList;
typedef sImageList              RoleImgList;
typedef sRoleComponentList      RoleComList;
typedef sRoleActionList         RoleActList;

typedef RoleFrmList::iterator itFrm;
//typedef RoleImgList::iterator itImg;
typedef RoleComList::iterator itCom;
typedef RoleActList::iterator itAct;

typedef RoleActList::const_iterator citAct;
typedef RoleComList::const_iterator citCom;
typedef RoleFrmList::const_iterator citFrm;

namespace LINEAR
{
    enum E
    {
        POSITION = 0x00000001,
        ROTATION = 0x00000002,
        SCALE    = 0x00000004,
        COLOR    = 0x00000008,
        SIZE     = 0x00000010,
        UV       = 0x00000020
    };
}

//---------------------------------------------------------------------------
// ¨¤¦â¤è¦V
//---------------------------------------------------------------------------
enum RoleDirect
{
    RD_South,
    RD_ESouth,
    RD_East,
    RD_ENorth,
    RD_North,
    RD_WNorth,
    RD_West,
    RD_WSouth,
    RD_Count,
    RD_None
};

const unsigned int ID_LEN = 4;
const unsigned int NAME_LEN = 20;
const unsigned int FILE_LEN = 50;

} //namespace FDO

#endif // FDO_RolePreRequestH