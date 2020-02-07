#ubuntu的内核源码树，如果要编译在ubuntu中安装的模块就打开这2个
KERN_VER = $(shell uname -r)
KERN_DIR = /lib/modules/$(KERN_VER)/build	

# 开发板的linux内核的源码树目录
#KERN_DIR = /home/lupingguang/lpg/x210bsp/kernel

#指定编译相关：编译器与头文件路径
CC  = $(CROSS_COMPILE)gcc
ccflags-y += -I$(src)/include		

#ko文件名称，当多个源文件时，该名称不与任何一个源文件同名
OBJ_NAME := test
obj-m	+= $(OBJ_NAME).o

#所有依赖文件集合
$(OBJ_NAME)-objs += main.o
$(OBJ_NAME)-objs += include/sub.o

all:
	make -C $(KERN_DIR) M=`pwd`  $(CFLAGS) modules 
	@echo $(CFLAGS) $(CC)

.PHONY: clean	
clean:
	make -C $(KERN_DIR) M=`pwd` modules clean
	

