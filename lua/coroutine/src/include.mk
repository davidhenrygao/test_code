CUR_DIR:=src
SRC:=$(wildcard $(CUR_DIR)/*.c)
OBJS:=$(SRC:.c=.o)
