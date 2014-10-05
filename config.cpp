
#include "config.h"

int configInit(const char * file)
{
	lua = lua_open();
	luaL_openlibs(lua);
    return luaL_dofile(lua, file);
}

int configInt(const char * var)
{
	lua_getglobal(lua, var);
	return lua_tointeger(lua, -1);
}

const char *configString(const char * var)
{
	lua_getglobal(lua, var);
	return lua_tostring(lua, -1);
}

const char *configError()
{
    return lua_tostring(lua, -1);
}
