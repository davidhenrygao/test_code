#include <stdio.h>

#include "lua.h"
#include "lauxlib.h"

#define CALL_PARAM_CNT 3
static int lsop_call(lua_State *L) {
  int i;
  int arg_cnt = lua_gettop(L);
  printf("lsop_call arg_cnt: %d.\n", arg_cnt);
  return 0;
}

#define SEND_PARAM_CNT 2
static int lsop_send(lua_State *L) {
  int i;
  int arg_cnt = lua_gettop(L);
  printf("lsop_send arg_cnt: %d.\n", arg_cnt);
  return 0;
}

static const luaL_Reg lsop_funcs[] = {
  {"call", lsop_call},
  {"send", lsop_send},
  {NULL, NULL},
};

int luaopen_serverop1(lua_State *L) {
  luaL_newlib(L, lsop_funcs);
  return 1;
}
