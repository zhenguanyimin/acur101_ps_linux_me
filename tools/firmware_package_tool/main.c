#include <stdio.h>
#include <fcntl.h>
#include <linux/types.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>

typedef struct file_info_s
{
	uint8_t flag[4];
	uint8_t name[64];
	uint8_t len[4];
}file_info_t;

static int copy_file(int target_fd, unsigned char *source_file_name)
{
	unsigned char buffer[4096];
	int len;
	int fd;
	
	fd = open(source_file_name, O_RDONLY);
	if (fd < 0)
	{
		return -1;
	}
	
	while(1)
	{
		len = read(fd, buffer, sizeof(buffer));
		if (len <= 0)
			break;
		
		write(target_fd, buffer, len);
	}
	
	close(fd);
	return 0;
}

static off_t get_file_size(const char *file_name)
{
	off_t len = 0;
	int fd;
	struct stat file_stat;
	
	fd = open(file_name, O_RDONLY);
	if (fd >= 0)
	{
		fstat(fd, &file_stat);
		
		len = file_stat.st_size;
		
		close(fd);
	}
	
	return len;
}

static char * get_file_name(const char *file_path)
{
	int i;
	char *p;
	
	for (p=file_path, i=strlen(file_path)-1; i > 0; i--)
	{
		if (p[i-1] == '/')
		{
			break;
		}
	}

	return &p[i];
}


int main(int argc, void *argv[])
{
	int fd;
	int i;
	file_info_t info;
	int len;
	
	if (argc < 2)
	{
		printf("error");
		return -1;
	}
	
	unlink("out.bin");
	
	fd = open("out.bin", O_CREAT|O_RDWR, 0666);
	if (fd < 0)
	{
		perror("create target file error:"); 
		return -1;
	}
	
	for (i=1;i<argc; i++)
	{
		memset(&info, 0, sizeof(info));
		memset(&info.flag, 'A', sizeof(info.flag));
		//sprintf(info.name, "%s", (char *)argv[i]);
		sprintf(info.name, "%s", get_file_name(argv[i]));
		printf("file:%s\n", info.name);

		len = get_file_size(argv[i]);
		info.len[0] = (len >> 24);
		info.len[1] = ((len >> 16) & 0xFF);
		info.len[2] = ((len >> 8) & 0xFF);
		info.len[3] = (len & 0xFF);
		
		write(fd, &info, sizeof(info));
		copy_file(fd, argv[i]);
	}
	
	close(fd);
	printf("create file OK\n");
	return 0;
}
