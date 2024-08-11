#pragma once

#include <string>

class DirMessageOps {
public:
    inline static const std::string OPERATION_INVALID = "invalidOperation";

    inline static const std::string OPERATION_LOGIN = "login";
    inline static const std::string OPERATION_LOGIN_OK = "loginOk";
    inline static const std::string OPERATION_LOGIN_FAIL = "loginFail";
    inline static const std::string OPERATION_NOT_LOGGED = "notLogged"; 

    inline static const std::string OPERATION_USERLIST = "userlist";
    inline static const std::string OPERATION_USERLIST_OK = "userlistResponse";

    inline static const std::string OPERATION_LOGOUT = "logout";
    inline static const std::string OPERATION_LOGOUT_OK = "logoutOk";

    inline static const std::string OPERATION_BGSERVE = "bgserve";
    inline static const std::string OPERATION_BGSERVE_OK = "bgserveOk";

    inline static const std::string OPERATION_FILELIST = "filelist";
    inline static const std::string OPERATION_FILELIST_OK = "filelistOk";

    inline static const std::string OPERATION_PUBLISH = "publish";
    inline static const std::string OPERATION_PUBLISH_OK = "publishOk";

    inline static const std::string OPERATION_SEARCH = "search";
    inline static const std::string OPERATION_SEARCH_OK = "searchOk";
    inline static const std::string OPERATION_SEARCH_FAIL = "searchFail";

    inline static const std::string OPERATION_GET_ADDRESS = "getAddress";
    inline static const std::string OPERATION_GET_ADDRESS_OK = "getAddressOk";
    inline static const std::string OPERATION_GET_ADDRESS_FAIL = "getAddressFail"; 

    inline static const std::string OPERATION_STOPSERVER = "stopserver";
    inline static const std::string OPERATION_STOPSERVER_OK = "stopserverOk";
    inline static const std::string OPERATION_NOT_SERVER = "notServer";
};