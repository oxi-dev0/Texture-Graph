#pragma once

extern "C" 
{
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

#include <string>
#include <vector>
#include <filesystem>
#include <fstream>

#include "../Utils/Log.h"

namespace LuaManager
{
	extern lua_State* L;
	extern void Init();
	extern void Exit();

	extern bool CheckLua(lua_State* L2, int r, std::string owner="");
	extern void ResetEnv();
	extern void NewEnv();
}
