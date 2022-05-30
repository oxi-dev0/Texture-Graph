#include "LuaManager.h"

namespace LuaManager
{
	lua_State* L;
	void Init() {
		if (L != nullptr) {
			lua_close(L);
		}
		L = luaL_newstate();
		luaL_openlibs(L);
	}
	void Exit() {
		lua_close(L);
	}

	bool CheckLua(lua_State* L2, int r, std::string owner) {
		if (r != LUA_OK) {
			LOG_ERROR("{0}'s lua code did not execute successfully. Error: '{1}'", owner, lua_tostring(L2, -1));
			return false;
		}
		return true;
	}

	void ResetEnv() {
		lua_getglobal(L, "user variables");
		if (!lua_istable(L, -1)) { return; }
		lua_pushnil(L);
		while (lua_next(L, -2) != 0) {
			auto varKey = (const char*)lua_tostring(L, -2); // get user defined key name
			lua_pushnil(L); // put nil onto stack
			lua_setglobal(L, varKey); // set user defined global to nil
			lua_pop(L, 1); // stack = -1: key, -2: table
		}
		lua_pop(L, 1); // Remove table
	}

	void NewEnv() {
		lua_settop(L, 0);
		ResetEnv();
		const char* usrVarSetup = "local U = {}"
			"_G[\"user variables\"] = U"
			"local function trace(t, k, v)"
			"U[k] = true"
			"rawset(t, k, v)"
			"end"
			"setmetatable(_G, {__newindex = trace})";
		luaL_dostring(L, usrVarSetup); // This creates a table called "user variables" that keeps track of variables created by the script
	}
}
