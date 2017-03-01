#include "lua.h"
#include "lauxlib.h"

static int lsop_call(lua_State *L) {
  
}

static int lsop_send(lua_State *L) {
  
}

static const luaL_Reg lsop_funcs[] = {
  {"call", lsop_call},
  {"send", lsop_send},
  {NULL, NULL},
};

int luaopen_serverop(lua_State *L) {
  luaL_newlib(L, lsop_funcs);
  return 1;
}
