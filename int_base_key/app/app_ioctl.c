#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/input.h>
#include <string.h>


#define DEVICE_KEY			"/dev/input/event1"
#define DEVICE_MOUSE		"/dev/input/event3"

#define X210_KEY			"/dev/input/event2"


int main(void)
{
	int fd = -1, ret = -1;
	struct input_event ev;
	
	// 第1步：打开设备文件
	fd = open(X210_KEY, O_RDONLY);
	if (fd < 0)
	{
		perror("open");
		return -1;
	}
	
	while (1)
	{
		// 第2步：读取一个event事件包
		memset(&ev, 0, sizeof(struct input_event));
		ret = read(fd, &ev, sizeof(struct input_event));
		if (ret != sizeof(struct input_event))
		{
			perror("read");
			close(fd);
			return -1;
		}
		
		// 第3步：解析event包，才知道发生了什么样的输入事件
		printf("-------------------------\n");
		printf("type: %hd\n", ev.type);
		printf("code: %hd\n", ev.code);
		printf("value: %d\n", ev.value);
		printf("\n");
	}
	
	
	// 第4步：关闭设备
	close(fd);
	
	return 0;
}
