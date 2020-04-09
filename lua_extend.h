
#ifndef EXTEND_LUA
#define EXTEND_LUA
#include "lua/lprefix.h"


#include <stddef.h>

#include "lua/lua.h"

#include "lua/lualib.h"
#include "lua/lauxlib.h"
#include "lfs/lfs.h"
#ifdef __cplusplus
extern "C" {
#endif
LUALIB_API void luaL_openExtendlibs(lua_State *L);
#ifdef __cplusplus
}
#endif
#endif