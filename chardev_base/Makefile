#ubuntu的内核源码树，如果要编译在ubuntu中安装的模块就打开这2个
#KERN_VER = $(shell uname -r)
#KERN_DIR = /lib/modules/$(KERN_VER)/build	
CC  = $(CROSS_COMPILE)gcc
		
# 开发板的linux内核的源码树目录
KERN_DIR = /home/lupingguang/lpg/x210bsp/kernel

obj-m	+= char_dev_read_write.o

all:
	make -C $(KERN_DIR) M=`pwd` modules 

	echo $(CC)
#cp:
#	cp *.ko /root/porting_x210/rootfs/rootfs/driver_test

.PHONY: clean	
clean:
	make -C $(KERN_DIR) M=`pwd` modules clean
	

