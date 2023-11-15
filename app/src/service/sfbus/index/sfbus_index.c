
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>

#include <pthread.h>

#include "../sfbus.h"

/*******************************************************************************
@description:	index table
*******************************************************************************/
typedef struct sfbus_index_table
{
	pthread_mutex_t	mutex;

	int 	iSize;
	int 	iCount;
	int*	eIdArray;
	void**	pvIndexArray;
}sfbus_index_table_t;

static int sfbus_index_binary_search( int eId, int* eIdArray, int iCount )
{	
	do{
		if( 0 >= iCount )
			break;

		int min = 0;
		if( eIdArray[min] > eId )
			break;
		
		int max = iCount - 1;
		if( eIdArray[max] < eId )
			break;
		
		for( ; max >= min; )
		{
			int mid = min + (max - min) / 2;

			if( eIdArray[mid] == eId )
			{
				return mid;
			}
			else if( eIdArray[mid] < eId )
			{
				min = mid + 1;
			}
			else
			{
				max = mid - 1;
			}
		}
	}while( 0 );
	
	return -1;
}

static int sfbus_index_binary_insert( int eId, int* eIdArray, int iCount )
{
	int min = 0;
	int max = iCount - 1;
	
	if( 0 == iCount )
	{
		max = 0;
	}
	else if( eIdArray[min] == eId || eIdArray[max] == eId )
	{
		max = -1;
	}
	else if( eIdArray[min] > eId )
	{
		max = 0;
	}
	else if( eIdArray[max] < eId )
	{
		max = iCount;
	}
	else
	{
		do{
			int mid = min + (max - min) / 2;

			if( eIdArray[mid] < eId )
				min = mid;
			else if( eIdArray[mid] > eId )
				max = mid;
			else
				return -1;
		}while( max - min > 1 );
	}

	return max;
}

void* sfbus_index_create( int iTableSize )
{
	struct sfbus_index_table* psIndexTable = (struct sfbus_index_table*)malloc( sizeof(struct sfbus_index_table) );

	do{
		if( NULL == psIndexTable )
		{
			sfbus_log( SFBUS_LOG_LEVEL_ERR, "malloc err" );
			break;
		}

		psIndexTable->eIdArray		= malloc( iTableSize * sizeof(psIndexTable->eIdArray[0]) );
		if( NULL == psIndexTable->eIdArray )
		{
			sfbus_log( SFBUS_LOG_LEVEL_ERR, "malloc err" );
			free( psIndexTable );
			psIndexTable = NULL;
			break;
		}
		
		psIndexTable->pvIndexArray	= malloc( iTableSize * sizeof(psIndexTable->pvIndexArray[0]) );
		if( NULL == psIndexTable->pvIndexArray )
		{
			sfbus_log( SFBUS_LOG_LEVEL_ERR, "malloc err" );
			free( psIndexTable->eIdArray );
			free( psIndexTable );
			psIndexTable = NULL;
			break;
		}

		psIndexTable->iCount	= 0;
		psIndexTable->iSize		= iTableSize;

		if( 0 != pthread_mutex_init(&(psIndexTable->mutex),NULL) )
		{
			sfbus_log( SFBUS_LOG_LEVEL_ERR, "create mutex err %s", strerror(errno) );
			free( psIndexTable->eIdArray );
			free( psIndexTable );
			psIndexTable = NULL;
			break;
		}

		sfbus_log( SFBUS_LOG_LEVEL_INFO, "create index %p size %d", psIndexTable, iTableSize );
	}while( 0 );

	return psIndexTable;
}

int sfbus_index_delete( void* pvIndexTable )
{
	int eRet = -1;

	do{
		if( NULL == pvIndexTable )
		{
			sfbus_log( SFBUS_LOG_LEVEL_WARNING, "psIndexTable == null" );
			break;
		}

		struct sfbus_index_table* psIndexTable = (struct sfbus_index_table*)pvIndexTable;

		if( 0 != pthread_mutex_destroy(&(psIndexTable->mutex)) )
		{
			sfbus_log( SFBUS_LOG_LEVEL_ERR, "delete mutex err %s", strerror(errno) );
			break;
		}
		
		free( psIndexTable->eIdArray );
		free( psIndexTable->pvIndexArray );
		free( psIndexTable );

		sfbus_log( SFBUS_LOG_LEVEL_INFO, "create index %p", psIndexTable );
		eRet = 0;
	}while( 0 );

	return eRet;
}

int sfbus_index_get_empty_id( void* pvIndexTable )
{
	struct sfbus_index_table* psIndexTable = (struct sfbus_index_table*)pvIndexTable;

	if( psIndexTable->iSize == psIndexTable->iCount )
		return -1;

	pthread_mutex_lock( &psIndexTable->mutex );

	int eId = 1;
	for( int i = 0; psIndexTable->iCount > i; ++i )
	{
		if( psIndexTable->eIdArray[i] != eId )
			break;
		else
			++eId;
	}

	pthread_mutex_unlock( &psIndexTable->mutex );

	return eId;
}

void* sfbus_index_find_id( int eId, void* pvIndexTable )
{
	void* pvIndex = NULL;

	struct sfbus_index_table* psIndexTable = (struct sfbus_index_table*)pvIndexTable;

	pthread_mutex_lock( &psIndexTable->mutex );
	
	int iNum = sfbus_index_binary_search( eId, psIndexTable->eIdArray, psIndexTable->iCount );
	
	if( 0 <= iNum )
	{
		pvIndex = psIndexTable->pvIndexArray[iNum];
	}

	pthread_mutex_unlock( &psIndexTable->mutex );

	return pvIndex;
}

int sfbus_index_add_id( int eId, void* pvIndex, void* pvIndexTable )
{
	int i = -1;

	struct sfbus_index_table* psIndexTable = (struct sfbus_index_table*)pvIndexTable;

	int	iSize	= psIndexTable->iSize;
	int iCount	= psIndexTable->iCount;
	do{
		if( 0 >= eId )
		{
			break;
		}
		
		if( iSize <= iCount )
		{
			i = -2;
			break;
		}

		int* eIdArray = psIndexTable->eIdArray;

		pthread_mutex_lock( &psIndexTable->mutex );
		
		i = sfbus_index_binary_insert( eId, eIdArray, iCount );
		if( 0 <= i )
		{
			void** pvIndexArray = psIndexTable->pvIndexArray;
			for( ; i < iCount; --iCount )
			{
				eIdArray[iCount]		= eIdArray[iCount-1];
				pvIndexArray[iCount]	= pvIndexArray[iCount-1];
			}
			
			eIdArray[i]		= eId;
			pvIndexArray[i]	= pvIndex;
			
			++psIndexTable->iCount;
		}
		
		pthread_mutex_unlock( &psIndexTable->mutex );
	}while( 0 );

	if( 0 > i )
		sfbus_log( SFBUS_LOG_LEVEL_ERR, "add err id=%d index=%p num=%d", eId, pvIndex, i );
	else
		sfbus_log( SFBUS_LOG_LEVEL_DEBUG, "add id=%d index=%p num=%d", eId, pvIndex, i );

	return i;
}

int sfbus_index_del_id( int eId, void* pvIndexTable )
{
	struct sfbus_index_table* psIndexTable = (struct sfbus_index_table*)pvIndexTable;

	int 	iCount		= psIndexTable->iCount;
	int* 	eIdArray	= psIndexTable->eIdArray;

	pthread_mutex_lock( &psIndexTable->mutex );
	
	int i = sfbus_index_binary_search( eId, eIdArray, iCount );
	if( 0 <= i )
	{
		void** 	pvIndexArray	= psIndexTable->pvIndexArray;
		for( --iCount; iCount > i; ++i )
		{
			eIdArray[i]		= eIdArray[i+1];
			pvIndexArray[i]	= pvIndexArray[i+1];
		}

		--psIndexTable->iCount;
	}

	pthread_mutex_unlock( &psIndexTable->mutex );

	if( 0 > i )
		sfbus_log( SFBUS_LOG_LEVEL_ERR, "del id=%d num=%d", eId, i );
	else
		sfbus_log( SFBUS_LOG_LEVEL_DEBUG, "del id=%d num=%d", eId, i );
	
	return i;
}

