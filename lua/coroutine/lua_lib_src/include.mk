CUR_DIR:=lua_lib_src
LUA_LIB_SRC:=$(wildcard $(CUR_DIR)/*.c)
BASE_SRC:=$(basename $(notdir $(LUA_LIB_SRC)))
LUA_LIB_OBJS:=$(LUA_LIB_SRC:.c=.o)
TEMP_LUA_LIB:=$(LUA_LIB_OBJS:.o=.so)

PROJECT_LUA_LIB_PATH?=lua_lib
PROJECT_LUA_LIB:=$(foreach v, $(TEMP_LUA_LIB), $(PROJECT_LUA_LIB_PATH)/$(v))

$(PROJECT_LUA_LIB_PATH):
	mkdir $(PROJECT_LUA_LIB_PATH)

$(PROJECT_LUA_LIB): |$(PROJECT_LUA_LIB_PATH)

define LUA_LIB_TEMP
$$(PROJECT_LUA_LIB_PATH)/$$(1).so: $(CUR_DIR)/$$(1).o $(PROJECT_LIB_PATH)/$(PROJECT_LIB)
	$$(CC) -shared -fpic $$(CFLAGS) $$^ -o $$@
endef

$(foreach v, $(BASE_SRC), $(eval $(call LUA_LIB_TEMP)))
