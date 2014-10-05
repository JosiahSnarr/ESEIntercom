
#include "config.h"

void configInit(const char * file)
{
	lua = lua_open();
	luaL_openlibs(lua);
	luaL_loadfile(lua, file);
	lua_pcall(lua, 0, 0, 0);
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