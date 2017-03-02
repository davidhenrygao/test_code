CC:=gcc
CFLAGS=-g -O2 -I$(PROJECT_INCLUDE_PATH) -I$(LUA_INCLUDE_PATH)
LDFLAGS=-L$(PROJECT_LIB_PATH) -L$(LUA_LIB_PATH)
AR=ar rcu
RANLIB=ranlib

LUA_INCLUDE_PATH:=lua/include
LUA_LIB_PATH:=lua
LUA_LIB:=lua53

PROJECT_INCLUDE_PATH:=inc
PROJECT_LIB_PATH:=lib
PROJECT_LIB:=base.a

PROJECT_LUA_LIB_PATH:=lua_lib
PROJECT_LUA_LIB:=

TARGET:=program

#SRC_DIR:=$(wildcard *src)
SRC_DIR:=src lib_src lua_lib_src
SRC:=
LIB_SRC:=
LUA_LIB_SRC:=
OBJS:=
LIB_OBJS:=
LUA_LIB_OBJS:=

all: $(TARGET)

-include $(patsubst %, %/include.mk, $(SRC_DIR))

DEPENDS:=$(OBJS:.o=.d) $(LIB_OBJS:.o=.d) $(LUA_LIB_OBJS:.o=.d)

-include $(DEPENDS)

%.d: %.c
	./depend.sh `dirname $*` $(CFLAGS) $^ > $@

$(TARGET): $(OBJS) $(PROJECT_LIB_PATH)/$(PROJECT_LIB) $(PROJECT_LUA_LIB)
	$(CC) $(CFLAGS) $(LDFLAGS) -l$(LUA_LIB) -ldl -pthread -o $@ $(OBJS) $(PROJECT_LIB_PATH)/$(PROJECT_LIB)

.PHONY: all clean cleanall baselib luaclib

baselib: $(PROJECT_LIB_PATH)/$(PROJECT_LIB)

luaclib: $(PROJECT_LUA_LIB)

clean:
	$(RM) $(TARGET) $(OBJS) $(LIB_OBJS) $(LUA_LIB_OBJS); \
	
cleanall:
	$(RM) $(TARGET) $(OBJS) $(LIB_OBJS) $(LUA_LIB_OBJS); \
	$(RM) $(DEPENDS); \
	$(RM) $(PROJECT_LIB_PATH)/$(PROJECT_LIB) $(PROJECT_LUA_LIB);
