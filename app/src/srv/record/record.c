
#include "record.h"

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

#include <dirent.h>
#include <mqueue.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include "sys/socket.h"
#include "sys/types.h"
#include <sys/stat.h>
#include <unistd.h>

const char *record_folder = "record/";

int record_get_list( char *name, void *arg, int (*fun_cbk)(void *arg, int file_type, int file_size, char* name) )
{
	int eRet = 0;

	int name_len = 0;
	int folder_name_len = 0;
	int record_len = sizeof(record_folder) - 1;
	char *folder_name = NULL;

	if( name )
		name_len = strlen( name );

	folder_name = malloc(name_len + record_len + 1 + 256);
	if( folder_name )
	{
		memcpy( folder_name, record_folder, record_len );
		folder_name_len = record_len + name_len;
		if( name )
		{
			memcpy( folder_name + record_len, name, name_len );
			folder_name[folder_name_len++] = '/';
		}
		folder_name[folder_name_len] = '\0';

		DIR *dir;
    	struct dirent *ent;
		dir = opendir( folder_name );
		if( dir )
		{
			char *ret_name = folder_name + record_len;
			while( NULL != (ent = readdir(dir)) )
			{
				int ret_cbk = 0;
				if( 0 != strcmp(ent->d_name, ".") && 0 != strcmp(ent->d_name, "..") )
				{
					switch( ent->d_type )
					{
						case DT_REG:
							if( fun_cbk )
							{
								memcpy( folder_name + folder_name_len, ent->d_name, strlen(ent->d_name) + 1 );
								struct stat file_stat;
								if( 0 == lstat(folder_name,&file_stat) )
								{
									ret_cbk = fun_cbk( arg, 1, file_stat.st_size, ret_name );
								}
								folder_name[folder_name_len] = '\0';
							}
							break;
						case DT_DIR:
							if( fun_cbk )
							{
								memcpy( folder_name + folder_name_len, ent->d_name, strlen(ent->d_name) + 1 );
								ret_cbk = fun_cbk( arg, 0, 0, ret_name );
								folder_name[folder_name_len] = '\0';
							}
							break;
						default:
							break;
					}
					if( ret_cbk )
						break;
				}
			}

			fun_cbk( arg, 0, 0, NULL );
			closedir(dir);
		}
		else
		{
			perror("record_get_list open failled");
			eRet = -1;
		}

		free( folder_name );
	}
	else
	{
		eRet = -1;
	}

	return eRet;
}

int record_read( char *name, int offset, char *buffer, int size )
{
	int read_size = 0;

	if( name )
	{
		int name_len = strlen(name);
		int record_len = sizeof(record_folder) - 1;
		char *read_file = malloc( name_len + record_len + 1 );

		if( read_file )
		{
			memcpy( read_file, record_folder, record_len );
			memcpy( read_file + record_len, name, name_len );
			read_file[record_len + name_len] = '\0';

			umask(0);
			int fd = open( read_file, O_RDWR, 0666 );
			if(fd < 0)
			{
				perror( "record_read open failed" );
			}
			else
			{
			    if ( 0 > lseek(fd, offset, SEEK_SET) )
				{
			        perror( "record_read sleek failed" );
				}
				else
				{
					int len = read( fd, buffer, size );
					if( 0 > len )
					{
						perror( "record_read read failed" );
					}
					else
					{
						read_size = len;
					}
				}
				
				close(fd);
			}

			free( read_file );
		}

	}

	return read_size;
}

int record_delete( char *name )
{
	int eRet = 0;
	
	if( name )
	{
		int name_len = strlen(name);
		int record_len = sizeof(record_folder) - 1;
		char *del_file = malloc( name_len + record_len + 1 );

		if( del_file )
		{
			struct stat path_stat;
		
			memcpy( del_file, record_folder, record_len );
			memcpy( del_file + record_len, name, name_len );
			del_file[record_len + name_len] = '\0';
			
			if( 0 == lstat(del_file, &path_stat) )
			{
				if( S_ISDIR(path_stat.st_mode) )
				{
					//delete folder
					char *del_folder = malloc( name_len + record_len + 8 );
					if( del_folder )
					{
						memcpy( del_folder, "rm -rf ", 7 );
						memcpy( del_folder + 7, del_file, name_len + record_len );
						del_folder[ 7 + name_len + record_len ] = '\0';
						
						eRet = system( del_folder );
						if( eRet )
							perror( "command failed" );
					}
					else
					{
						eRet = -1;
					}
				}
				else
				{
					//delete file
					eRet = unlink(del_file);
					if( eRet )
						perror( "del error" );
				}
			}
			else
			{
				eRet = -1;
			}

			free( del_file );
		}
		else
		{
			eRet = -1;
		}
	}
	else
	{
		eRet = -1;
	}

	return eRet;
}


