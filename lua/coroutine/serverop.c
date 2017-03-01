#include "lua.h"
#include "lauxlib.h"

#include "pipeop.h"
#include "def.h"

#define CALL_PARAM_CNT 3
static int lsop_call(lua_State *L) {
  int i;
  int destfd;
  ServiceReq request;
  int ret;
  int arg_cnt = lua_gettop(L);
  if (arg_cnt < CALL_PARAM_CNT) {
    return luaL_error(L, 
        "call function need %d parameters, \
        but there are only %d parameters.\n", 
        CALL_PARAM_CNT, arg_cnt);
  }
  for (i = 0; i < CALL_PARAM_CNT; ++i) {
    if (lua_isinteger(L, i) == 0) {
      return luaL_argerror(L, i, "need a integer");
    }
  }
  request.req = lua_tointeger(L, 1);
  request.session = lua_tointeger(L, 2);
  destfd = lua_tointeger(L, 3);
  ret = WritePipe(destfd, (void*)&request, sizeof(request));
  if (ret == -1) {
    return luaL_error(L, "write pipe(%d) error.\n", destfd);
  }
  return lua_yield(L, 0);
}

#define SEND_PARAM_CNT 2
static int lsop_send(lua_State *L) {
  int i;
  int destfd;
  int resp;
  int ret;
  int arg_cnt = lua_gettop(L);
  if (arg_cnt < SEND_PARAM_CNT) {
    return luaL_error(L, 
        "call function need %d parameters, \
        but there are only %d parameters.\n", 
        SEND_PARAM_CNT, arg_cnt);
  }
  for (i = 0; i < SEND_PARAM_CNT; ++i) {
    if (lua_isinteger(L, i) == 0) {
      printf("what the fuck type you are.\n");
      return luaL_argerror(L, i, "need a integer");
    }
  }
  printf("lsop_send get params.\n");
  resp = lua_tointeger(L, 1);
  destfd = lua_tointeger(L, 2);
  printf("lsop_send WritePipe begin.\n");
  ret = WritePipe(destfd, (void*)&resp, sizeof(resp));
  printf("lsop_send WritePipe end, ret: %d.\n", ret);
  if (ret == -1) {
    return luaL_error(L, "write pipe(%d) error.\n", destfd);
  }
  return 0;
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
