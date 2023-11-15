
#include "flash_raw.h"
#include "string.h"
#include "errno.h"

#include <mqueue.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include "sys/socket.h"
#include "sys/types.h"
#include <sys/stat.h>
#include <unistd.h>

const char strFlashIndex[] = "/run/media/mtd_user/";

/**
 *  @b Description
 *  @n
 *      Init FlashRaw module. None of the FlashRaw API's can be used without invoking this API.
 *
 *  @retval
 *      Success    - 0
 *      Fail       < -1
 */
int32_t FlashRaw_Init(void)
{
    int32_t ret = 0;

out:
    return ret;
}

/**
 *  @b Description
 *  @n
 *      Set a NV item to flash.
 *
 *  @param[in]  key
 *      Key of the NV.
 *
 *  @param[in]  value_buf
 *      Buffer address of the contents.
 *
 *  @param[in]  buf_len
 *      Length of the contents in the buffer.
 *
 *  @retval
 *      Success    - 0
 *      Fail       < -1
 */
int32_t FlashRaw_Set(const char *key, const void *value_buf, size_t buf_len)
{
    int32_t ret = 0;

    if ((NULL == key) || (NULL == value_buf) || (0 == buf_len))
    {
        ret = -1;
        goto out;
    }

//	printf( "set key %s\n", key );

	char strName[256];
	memcpy( strName, strFlashIndex, sizeof(strFlashIndex)-1 );
	memcpy( strName + sizeof(strFlashIndex) - 1, key, strlen(key) + 1 );

	umask(0);
	int fd = open( strName, O_RDWR | O_CREAT | O_TRUNC, 0666 );
	if(fd < 0)
	{
		perror( "set open error" );
		ret = -1;
	}
	else
	{		
		if( 0 > write( fd, value_buf, buf_len ) )
		{
			printf( "FlashRaw_Set err %s\n", key );
			ret = -1;
		}
		
		close(fd);
	}

out:
    return ret;
}

/**
 *  @b Description
 *  @n
 *      Get a NV item from flash.
 *
 *  @param[in]  key
 *      Key of the NV.
 *
 *  @param[in]  value_buf
 *      Buffer address used to store the contents.
 *
 *  @param[in]  buf_len
 *      Length of the buffer.
 *
 *  @param[out]  saved_value_len
 *      Length of the NV contents in the flash.
 *
 *  @retval
 *      Length of data store in the buffer.
 */
size_t FlashRaw_Get(const char *key, void *value_buf, size_t buf_len, size_t *saved_value_len)
{
    size_t ret = 0;

    if ((NULL == key) || (NULL == value_buf) || (0 == buf_len))
    {
        ret = 0;
        goto out;
    }

//	printf( "get key %s\n", key );

        char strName[256];
        memcpy( strName, strFlashIndex, sizeof(strFlashIndex)-1 );
        memcpy( strName + sizeof(strFlashIndex) - 1, key, strlen(key) + 1 );

	umask(0);
	int fd = open( strName, O_RDWR, 0666 );
	if(fd < 0)
	{
		perror( "get open error" );
		ret = 0;
	}
	else
	{
		int len = read( fd, value_buf, buf_len );
		if( 0 > len )
		{
			printf( "FlashRaw_Get err %s\n", key );
			ret = 0;
		}
		else
		{
			ret = len;
			if( saved_value_len )
				*saved_value_len = len;
		}
		
		close(fd);
	}
out:
    return ret;
}

/**
 *  @b Description
 *  @n
 *      Delete a NV item.
 *
 *  @param[in]  key
 *      Key of the NV.
 *
 *  @retval
 *      Success    - 0
 *      Fail       < -1
 */
int32_t FlashRaw_Del(const char *key)
{
    int32_t ret = 0;

    if (NULL == key)
    {
        ret = -1;
        goto out;
    }

//	printf( "del key %s\n", key );

        char strName[256];
        memcpy( strName, strFlashIndex, sizeof(strFlashIndex)-1 );
        memcpy( strName + sizeof(strFlashIndex) - 1, key, strlen(key) + 1 );

	int file_ret = unlink(strName);
	if( 0 > file_ret )
	{
		perror( "del error" );
		ret = -1;
	}

out:
    return ret;
}

