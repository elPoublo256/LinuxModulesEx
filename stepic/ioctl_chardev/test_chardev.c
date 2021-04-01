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


//#define IOC_MAGIC 'k'

//#define SUM_LENGTH _IOWR(IOC_MAGIC, 1, char*)
//#define SUM_CONTENT _IOWR(IOC_MAGIC, 2, char*)


static void test_ioctl()
{
	int fd = open("/dev/solution_node", O_WRONLY);
	int res = ioctl(fd, 4444, "qwerty");
	printf("result ioctl=%d, fd=%d\n", res, fd);
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
