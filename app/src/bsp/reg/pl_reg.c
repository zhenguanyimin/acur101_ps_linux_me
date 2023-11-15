
#include "pl_reg.h"
#include <pthread.h>
#include <fcntl.h>
#include <sys/time.h>
#include <semaphore.h>
#include <mqueue.h>
#include <unistd.h>

#include <sys/mman.h>
uint32_t *pl_reg_base = NULL;

void pl_reg_init( void )
{
	int fd;

	fd = open("/dev/mem", O_RSYNC|O_RDWR);
	if (fd < 0)
	{
		perror("pl reg map error");
		return ;
	}

	pl_reg_base = (uint32_t *)mmap(NULL, PL_REG_REGION_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, PL_REG_PHY_ADDR);

	close( fd );
	printf( "PL reg virtual address:%p\n", pl_reg_base );
}

void pl_reg_deinit( void )
{
	if (pl_reg_base != NULL)
	{
		munmap(pl_reg_base, PL_REG_REGION_SIZE);
		pl_reg_base = NULL;
	}
}

