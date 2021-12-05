DIR_INC = ./inc
DIR_SRC = ./src
DIR_OBJ = ./obj

INSTALL_LIB_PATH := ./../lib
LIB_TARGET := ose.a

SRC = $(wildcard ${DIR_SRC}/*.c)
DIR = $(notdir ${SRC})
OBJ = $(patsubst %.c,${DIR_OBJ}/%.o,$(notdir ${SRC}))
#把.c换成.o  
OBJS = $(patsubst %.c,%.o,$(DIR))
#把所有.o加上想要的路径信息  
OBJS := $(foreach X,$(OBJS),$(DIR_OBJ)/$(X)) 

#包含需要的头文件路径  
CFLAGS = -g -O2 -Wall -I${DIR_INC} 

${DIR_OBJ}/%.o:${DIR_SRC}/%.c
	$(CC) $(CFLAGS) -c  $< -o $@

#用.o生成静态库文件.a,并放到指定目录下  
$(LIB_TARGET):$(OBJS)  
	ar crus $(LIB_TARGET) $^  
	install -m 0755 $(LIB_TARGET) $(INSTALL_LIB_PATH)  
	rm -rf $(LIB_TARGET)

.PHONY:clean
clean:
	rm -rf $(DIR_OBJ)