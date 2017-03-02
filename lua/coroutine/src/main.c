#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <signal.h>
#include <sys/select.h>
#include <assert.h>

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "pipeop.h"
#include "def.h"

typedef void* (ThreadRoutine)(void*);

typedef union Channel {
  int fds[2];
  struct {
    int rfd;
    int wfd;
  };
}Channel_t;

#define MAX_ERR_BUF_LEN 128
char err_buf[MAX_ERR_BUF_LEN];

Channel_t g_rrch; //request read channel
Channel_t g_rwch; //request write channel
Channel_t g_srch; //service read channel
Channel_t g_swch; //service write channel

void OpenChannel(Channel_t *);
void CreateThread(pthread_t *, ThreadRoutine *);

lua_State *L;
lua_State* CreateLuaMachine();
int LoadEntranceScript(lua_State *, const char *);
int DoRequest(int, int);
int DoResponse(ServiceResp, int);

ThreadRoutine ClientRountine;
ThreadRoutine ServerRountine;
ThreadRoutine InnerServerRountine;
ThreadRoutine WorkRoutine;

int main(int argc, char *argv[]) {
  int ret;
  pthread_t client_td;
  pthread_t server_td;
  pthread_t inner_server_td;

  srand(time(NULL));

  signal(SIGPIPE, SIG_IGN);

  OpenChannel(&g_rrch);
  OpenChannel(&g_rwch);
  OpenChannel(&g_srch);
  OpenChannel(&g_swch);

  L = CreateLuaMachine();
  if (L == NULL) {
    return -1;
  }
  ret = LoadEntranceScript(L, "lua_script/entrance.lua");
  if (ret != LUA_OK) {
    return -1;
  }

  CreateThread(&client_td, &ClientRountine);
  CreateThread(&server_td, &ServerRountine);
  CreateThread(&inner_server_td, &InnerServerRountine);

  pthread_join(client_td, NULL);
  pthread_join(server_td, NULL);
  pthread_join(inner_server_td, NULL);

  return 0;
}

void OpenChannel(Channel_t *pch) {
  int ret = pipe(pch->fds);
  if (ret != 0) {
    printf("Open pipe error: %s!\n", 
        strerror_r(ret, err_buf, MAX_ERR_BUF_LEN));
    exit(-1);
  }
  int flags = fcntl(pch->rfd, F_GETFL);
  ret = fcntl(pch->rfd, F_SETFL, flags | O_NONBLOCK);
  flags = fcntl(pch->wfd, F_GETFL);
  ret = fcntl(pch->wfd, F_SETFL, flags | O_NONBLOCK);
}

void CreateThread(pthread_t *ptid, ThreadRoutine *routine) {
  int ret = pthread_create(ptid, NULL, routine, NULL);
  if (ret != 0) {
    printf("Create Thread failed: %s!\n", 
        strerror_r(ret, err_buf, MAX_ERR_BUF_LEN));
    exit(-1);
  }
}

void* ClientRountine(void* arg) {
  printf("ClientRountine run.\n");
  int buf[10];
  int i;
  size_t wbs = 0;
  size_t total;
  int ret;
  int maxfd;
  fd_set rfs;
  for (i = 0; i < 10; ++i) {
    buf[i] = i;
  }
  total = sizeof(buf);
  ret = WritePipe(g_rwch.wfd, (void*)buf, total);
  if (ret != 0) {
    goto ClientRountineEnd;
  }
  maxfd = g_rrch.rfd + 1;
  for (i = 0; i < 10; ++i) {
    FD_SET(g_rrch.rfd, &rfs);
    ret = select(maxfd, &rfs, NULL, NULL, NULL);
    if (ret == -1) {
      printf("ClientRountineEnd select error: %s.\n", 
          strerror_r(errno, err_buf, MAX_ERR_BUF_LEN));
      goto ClientRountineEnd;
    }
    if (FD_ISSET(g_rrch.rfd, &rfs)) {
      ret = ReadPipe(g_rrch.rfd, (void*)(buf+i), sizeof(int));
      if (ret != 0) {
        goto ClientRountineEnd;
      }
    }
  }
  printf("Return result: \n");
  for (i = 0; i < 10; ++i) {
    printf("%d ", buf[i]);
  }
  printf("\n");

ClientRountineEnd:
  printf("ClientRountine end.\n");
  close(g_rwch.wfd);
  close(g_rrch.rfd);
  return NULL;
}

#define MAX_IDLE 10

void* ServerRountine(void* arg) {
  printf("ServerRountine run.\n");
  int req, resp;
  int ret, ret2;
  int maxfd;
  fd_set rfs;
  int session;
  ServiceReq request;
  ServiceResp response;
  //struct timeval timeout;

  maxfd = g_rwch.rfd > g_srch.rfd ? g_rwch.rfd + 1 : g_srch.rfd + 1;

  while (1) {
    FD_ZERO(&rfs);
    FD_SET(g_rwch.rfd, &rfs);
    FD_SET(g_srch.rfd, &rfs);
    //timeout.tv_sec = 10;
    //timeout.tv_usec = 0;
    //ret = select(maxfd, &rfs, NULL, NULL, &timeout);
    ret = select(maxfd, &rfs, NULL, NULL, NULL);
    if (ret == -1) {
      printf("ServerRountine select error: %s.\n", 
          strerror_r(errno, err_buf, MAX_ERR_BUF_LEN));
      break;
    }
    if (ret == 0) {
      printf("ServerRountineEnd read timeout!\n");
      break;
    }
    if (FD_ISSET(g_rwch.rfd, &rfs)) {
      ret = ReadPipe(g_rwch.rfd, (void*)&req, sizeof(req));
      if (ret == -1) {
        printf("ServerRountine read requst channel error.\n");
        goto ServerRountineEnd;
      } else {
        ret2 = DoRequest(req, g_swch.wfd);
        if (ret2 != LUA_OK) {
          printf("ServerRountine DoRequest error.\n");
          break;
        }
        //request.req = req;
        //request.session = session;
        //ret2 = WritePipe(g_swch.wfd, (void*)&request, sizeof(request));
        //if (ret2 == -1) {
          //printf("ServerRountine write service channel error.\n");
          //break;
        //}
      }
    }
    if (FD_ISSET(g_srch.rfd, &rfs)) {
      ret = ReadPipe(g_srch.rfd, (void*)&response, sizeof(response));
      if (ret == -1) {
        printf("ServerRountine read service channel error.\n");
        goto ServerRountineEnd;
      } else {
        //ret2 = WritePipe(g_rrch.wfd, (void*)&(response.resp), sizeof(int));
        //if (ret2 == -1) {
          //printf("ServerRountine write request channel error.\n");
          //break;
        //}
        ret2 = DoResponse(response, g_rrch.wfd);
        if (ret2 != LUA_OK) {
          printf("ServerRountine DoResponse error.\n");
          break;
        }
      }
    }
  }
ServerRountineEnd:
  printf("ServerRountine end.\n");
  close(g_rwch.rfd);
  close(g_rrch.wfd);
  close(g_swch.wfd);
  close(g_srch.rfd);
  return NULL;
}

void* InnerServerRountine(void* arg) {
  printf("InnerServerRountine run.\n");
  int req, resp;
  int ret, ret2;
  int maxfd;
  fd_set rfs;
  pthread_t tid;
  ServiceReq buf[10];
  int pos = 0;
  ServiceReq request;
  ServiceResp response;

  maxfd = g_swch.rfd + 1;

  while (1) {
    FD_ZERO(&rfs);
    FD_SET(g_swch.rfd, &rfs);
    ret = select(maxfd, &rfs, NULL, NULL, NULL);
    if (ret == -1) {
      printf("InnerServerRountine select error: %s.\n", 
          strerror_r(errno, err_buf, MAX_ERR_BUF_LEN));
      break;
    }
    if (ret == 0) {
      printf("InnerServerRountine read timeout!\n");
      break;
    }
    if (FD_ISSET(g_swch.rfd, &rfs)) {
      ret = ReadPipe(g_swch.rfd, (void*)&request, sizeof(request));
      if (ret == -1) {
        printf("InnerServerRountine read service channel error.\n");
        goto InnerServerRountineEnd;
      } else {
        buf[pos] = request;
        ret2 = pthread_create(&tid, NULL, &WorkRoutine, (void*)(buf+pos));
        pos++;
        if (ret2 != 0) {
          printf("InnerServerRountine create work thread error: %s.\n", 
              strerror_r(ret, err_buf, MAX_ERR_BUF_LEN));
          response.resp = request.req;
          response.session = request.session;
          ret2 = WritePipe(g_srch.wfd, (void*)&response, sizeof(response));
          if (ret2 != 0) {
            printf("InnerServerRountine write service channel error.\n");
            break;
          }
        } else {
          pthread_detach(tid);
        }
      }
    }
  }
InnerServerRountineEnd:
  printf("InnerServerRountine end.\n");
  close(g_swch.rfd);
  close(g_srch.wfd);
  return NULL;
}

//pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void* WorkRoutine(void* arg) {
  long r = rand() % 10 + 1;
  int ret;
  ServiceReq *preq = (ServiceReq*)arg;
  ServiceResp response;
  printf("Request %d (session %d) sleep %ld seconds.\n", 
      preq->req, preq->session, r);
  sleep((unsigned int)r);
  //pthread_mutex_lock(&lock);
  response.resp = preq->req;
  response.session = preq->session;
  ret = WritePipe(g_srch.wfd, (void*)&response, sizeof(ServiceResp));
  //pthread_mutex_unlock(&lock);
  if (ret != 0) {
    printf("WorkRoutine write service channel error.\n");
  }
  return arg;
}

lua_State *CreateLuaMachine() {
  int ret;
  lua_State *L = luaL_newstate();
  luaL_openlibs(L);
  ret = luaL_loadfile(L, "lua_script/init.lua");
  if (ret != LUA_OK) {
    printf("CreateLuaMachine load init script error: %d.\n", ret);
    lua_close(L);
    return NULL;
  }
  ret = lua_pcall(L, 0, 0, 0);
  if (ret != LUA_OK) {
    printf("CreateLuaMachine  execute init script error:%d.\n\t%s.\n", 
        ret, lua_tostring(L, -1));
    lua_close(L);
    return NULL;
  }
  return L;
}

int LoadEntranceScript(lua_State *L, const char *script) {
  int ret;
  if (L == NULL) {
    printf("LoadEntranceScript lua_State* parameter is NULL.\n");
    return -1;
  }
  ret = luaL_loadfile(L, script);
  if (ret != LUA_OK) {
    printf("LoadEntranceScript load script error: %d.\n", ret);
    return ret;
  }
  ret = lua_pcall(L, 0, LUA_MULTRET, 0);
  if (ret != LUA_OK) {
    printf("LoadEntranceScript execute script error:%d.\n\t%s.\n", 
        ret, lua_tostring(L, -1));
    lua_pop(L, 1);
    return ret;
  }
  return ret;
}

int DoRequest(int req, int swch_wfd) {
  int msgh;
  int ret;
  if(lua_istable(L, 1) == 0) {
    printf("Entrance script haven't been loaded.\n");
    return -1;
  }
  lua_pushstring(L, "debugTrace");
  lua_rawget(L, 1);
  if (lua_isnil(L, -1)) {
    printf("debugTrace entrance can't be found.\n");
    lua_settop(L, 1);
    return -1;
  }
  msgh = lua_gettop(L);
  lua_pushstring(L, "doRequest");
  lua_rawget(L, 1);
  if (lua_isnil(L, -1)) {
    printf("doRequest entrance can't be found.\n");
    lua_settop(L, 1);
    return -1;
  }
  lua_pushinteger(L, req);
  lua_pushinteger(L, swch_wfd);
  ret = lua_pcall(L, 2, LUA_MULTRET, msgh);
  if (ret != LUA_OK) {
    printf("DoRequest pcall error: %d.\n", ret);
    return ret;
  }
  lua_settop(L, 1);
  return ret;
}

int DoResponse(ServiceResp resp, int rrch_wfd) {
  int msgh;
  int ret;
  if(lua_istable(L, 1) == 0) {
    printf("Entrance script haven't been loaded.\n");
    return -1;
  }
  lua_pushstring(L, "debugTrace");
  lua_rawget(L, 1);
  if (lua_isnil(L, -1)) {
    printf("debugTrace entrance can't be found.\n");
    lua_settop(L, 1);
    return -1;
  }
  msgh = lua_gettop(L);
  lua_pushstring(L, "doResponse");
  lua_rawget(L, 1);
  if (lua_isnil(L, -1)) {
    printf("DoResponse entrance can't be found.\n");
    lua_settop(L, 1);
    return -1;
  }
  lua_pushinteger(L, resp.resp);
  lua_pushinteger(L, resp.session);
  lua_pushinteger(L, rrch_wfd);
  ret = lua_pcall(L, 3, LUA_MULTRET, msgh);
  if (ret != LUA_OK) {
    printf("DoResponse pcall error: %d.\n", ret);
    return ret;
  }
  return ret;
}
