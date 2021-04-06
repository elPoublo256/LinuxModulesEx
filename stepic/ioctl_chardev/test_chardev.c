#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <malloc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#include <linux/ioctl.h>
#include <linux/kdev_t.h> /* for MKDEV */

    #define DEVICE_NAME "my_dev"
    #define DEVICE_PATH "/dev/my_dev"
#define IOC_MAGIC 'k'

#define SUM_LENGTH _IOWR(IOC_MAGIC, 1, char*)
#define SUM_CONTENT _IOWR(IOC_MAGIC, 2, char*)


static void test_ioctl()
{
	int fd = open("/dev/solution_node", O_RDWR);
	write(fd,"qwe",3);
	int res = ioctl(fd, SUM_LENGTH, "qwerty");
	printf("result ioctl SUM_LEN=%d, fd=%d\n", res, fd);
	res=ioctl(fd,SUM_CONTENT, "12");
	printf("result ioctl SUM_CONTENT=%d, fd=%d\n", res, fd);
	res=ioctl(fd,SUM_CONTENT, "328");
        printf("result ioctl SUM_CONTENT=%d, fd=%d\n", res, fd);
	res=ioctl(fd,4444, "328");
        printf("result ioctl 44444=%d, fd=%d\n", res, fd);
	res=ioctl(fd,5555, "328");
        printf("result ioctl 5555=%d, fd=%d\n", res, fd);
	close(fd);
}

int main(int argc, char *argv[])
{
 test_ioctl();
 return 0;
}

size_t get_size_file_print_stat(const char* filename)
{
	struct stat file_stat;
	errno = 0;
	if(stat(filename,&file_stat))
	{
		printf("error get stat: %s\n",strerror(errno));
		exit(-1);
	}

	return file_stat.st_size;
}
