#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <malloc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
size_t get_size_file_print_stat(const char* filename);

void test_write(const char filename)
{
	int fd = open(filename, O_WRONLY);
	if(fd <0)
	{
		printf("error open for test write: %s",strerror(errno));
		exit(-1);
	}
	int res_wr = write(fd,"qwerty",strlen("qwerty"));
}


int main(int argc, char *argv[])
{
	if(argc != 2)
	{
		printf("error: there are should be 1 argument\n");
		return -1;
	}
	const char file_name = argv[2];
	printf("try open file ");
	printf(argv[1]);
	printf("\n");
	if(access(argv[1], F_OK) == -1)
	{
		printf("error acces file %s\n",argv[1]);
	}
	else
	{
		printf("get acces to file\n");
	}

	size_t size_file = get_size_file_print_stat(file_name);
	printf("%s size from stat - %lu", file_name, size_file);
	

	return 0;

}

size_t get_size_file_print_stat(const char* filename)
{
	struct stat file_stat;
	errno = 0;
	if(stat(filename,&file_stat))
	{
		printf("error get stat: %s",strerror(errno));
		exit(-1);
	}

	return file_stat.st_size;
}
