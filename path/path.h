#ifndef LUA_PATH_EXTEND
#define LUA_PATH_EXTEND
#include"lua.h"
#ifdef __cplusplus
extern "C" {
#endif
extern int luaopen_path(lua_State *L);
#ifdef __cplusplus
}
#endif
#endif