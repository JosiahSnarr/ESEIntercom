
#pragma once

#ifndef CONFIG_H
#define CONFIG_H

#include <lua.hpp>

static lua_State* lua;

int configInit(const char * configFile);

int configInt(const char * var);
const char *configString(const char * var);

#endif
