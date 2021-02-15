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

size_t get_size_file_print_stat(const char* filename);

static inline void test_write(const char* filename)
{
	int fd = open(filename, O_WRONLY);
	if(fd <0)
	{
		printf("error open for test write: %s",strerror(errno));
		exit(-1);
	}
	int res_wr = write(fd,"qwerty",strlen("qwerty"));
	if(res_wr < 0)
	{
		printf("error write : %s\n",strerror(errno));
		exit(-1);
	}
	else
	{
		printf("write to file %db\n",res_wr);
	}
	close(fd);
}

static inline void test_read(const char* file_name, const size_t size)
{
	char* buf = malloc(size);
	if(buf)
	{
		int fd = open(file_name, O_RDONLY);
		if(fd < 0)
		{
			printf("error open test read %s\n", file_name);
			exit(-1);

		}
		int res_rd = read(fd, buf, size);
		if(res_rd < 0)
		{
			printf("error read file %s\n", strerror(errno));
			exit(-1);
		}
		printf("readed from file %lub:\n", res_rd);
		write(1,buf,res_rd);
		free(buf);
		close(fd);
	}
	else
	{
		printf("error alloc for read\n");
		exit(-1);
	}
					
}


int main(int argc, char *argv[])
{
	if(argc != 2)
	{
		printf("error: there are should be 1 argument\n");
		return -1;
	}
	const char* file_name = (const char*) argv[1];
	printf("try open file ");
	printf(argv[1]);
	printf("\n");
	
	if(access(file_name, F_OK) == -1)
	{
		printf("error acces file %s\n",file_name);
	}
	else
	{
		printf("get acces to file %s\n", file_name);
	}

	size_t size_file = get_size_file_print_stat(file_name);
	printf("%s size from stat - %lu\n", file_name, size_file);
	test_write(file_name);
	test_read(file_name, strlen("qwerty"));
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
