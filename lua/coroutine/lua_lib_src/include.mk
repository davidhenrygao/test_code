CUR_DIR:=lua_lib_src
LUA_LIB_SRC:=$(wildcard $(CUR_DIR)/*.c)
BASE_SRC:=$(basename $(notdir $(LUA_LIB_SRC)))
SPECIAL_SRC:=serverop
LUA_LIB_OBJS:=$(LUA_LIB_SRC:.c=.o)

PROJECT_LUA_LIB_PATH?=lua_lib
PROJECT_LUA_LIB:=$(foreach v, $(BASE_SRC), $(PROJECT_LUA_LIB_PATH)/$(v).so)

$(PROJECT_LUA_LIB_PATH):
	mkdir $(PROJECT_LUA_LIB_PATH)

$(PROJECT_LUA_LIB): |$(PROJECT_LUA_LIB_PATH)

define LUA_LIB_TEMP
$$(PROJECT_LUA_LIB_PATH)/$(1).so: $(CUR_DIR)/$(1).c
	$$(CC) -shared -fPIC $$(CFLAGS) $$^ -o $$@
endef

$(foreach v, $(filter-out $(SPECIAL_SRC),$(BASE_SRC)), $(eval $(call LUA_LIB_TEMP,$(v))))

$(PROJECT_LUA_LIB_PATH)/serverop.so: $(CUR_DIR)/serverop.c lib_src/pipeop.c
	$(CC) -shared -fPIC $(CFLAGS) $^ -o $@
