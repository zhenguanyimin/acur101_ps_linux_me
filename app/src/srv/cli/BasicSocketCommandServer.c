/*
 *
 */

/* Standard includes. */
#include "stdlib.h"
#include "string.h"
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include<errno.h>
#include <pthread.h>
#include <semaphore.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include <sys/mman.h>
#include "../../inc/radar_defines.h"
#include "../../srv/cli/FreeRTOS_CLI.h"
#include <mqueue.h>

/* Dimensions the buffer into which input characters are placed. */
#define cmdMAX_INPUT_SIZE	100

/* Dimensions the buffer into which string outputs can be placed. */
#define cmdMAX_OUTPUT_SIZE	1024

typedef struct cli_func
{
	bool				bFlagInit;
	uint8_t 			byState;
	uint8_t 			byTimeCount;
	mqd_t 				mqCmd;
	pthread_mutex_t 	mutex;
	pthread_t 			pid;
}cli_func_t;

static cli_func_t sCli = { 0 };

/*-----------------------------------------------------------*/

static void *vBasicSocketsCommandInterpreterTask(void *pvParameters)
{
	int ret;
	unsigned char buffer[64];
	long lSocket, lClientFd, lBytes, lAddrLen = sizeof( struct sockaddr_in ), lInputIndex;
	struct sockaddr_in sLocalAddr;
	struct sockaddr_in client_addr;
	const char *pcWelcomeMessage = "Linux command server - connection accepted.\r\nType Help to view a list of registered commands.\r\n\r\n>";
	char cInChar;
	static char cInputString[ cmdMAX_INPUT_SIZE ], cOutputString[ cmdMAX_OUTPUT_SIZE ];
	long xReturned;
	struct timeval tv_out;
	(void) pvParameters;

	lSocket = socket(AF_INET, SOCK_STREAM, 0);

	if(lSocket >= 0)
	{
		memset((char *)&sLocalAddr, 0, sizeof(sLocalAddr));
		sLocalAddr.sin_family = AF_INET;
		//sLocalAddr.sin_len = sizeof(sLocalAddr);
		sLocalAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		sLocalAddr.sin_port = ntohs( ( ( unsigned short ) 23));

		if(bind(lSocket, (struct sockaddr*) &sLocalAddr, sizeof(sLocalAddr)) < 0)
		{
			close(lSocket);
		}
		if(listen(lSocket, 20) != 0 )
		{
			close(lSocket);
		}
		printf("Come on!!! \r\n");

		for( ;; )
		{
			lClientFd = accept(lSocket, (struct sockaddr *) &client_addr, (uint32_t *) &lAddrLen );

			if(lClientFd > 0L)
			{
        		int ret = 0;
				tv_out.tv_sec = 1;
				tv_out.tv_usec = 0;
				//ret = setsockopt(lClientFd, SOL_SOCKET, SO_SNDTIMEO, &tv_out, sizeof(tv_out));
				//ret = setsockopt(lClientFd, SOL_SOCKET, SO_RCVTIMEO, &tv_out, sizeof(tv_out));

				send(lClientFd, pcWelcomeMessage, strlen((const char *) pcWelcomeMessage), 0);
				lInputIndex = 0;
				memset(cInputString, 0x00, cmdMAX_INPUT_SIZE);

				do
				{
					lBytes = recv(lClientFd, &cInChar, sizeof(cInChar), 0);

					if(lBytes > 0L)
					{
						/* Echo the character back. */
						send(lClientFd, &cInChar, sizeof(cInChar), 0);

						if(cInChar == '\n')
						{
							/* The input string has been terminated.  Was the
							input a quit command? */
							if(strcmp( "quit", (const char *)cInputString) == 0)
							{
								/* Set lBytes to 0 to close the connection. */
								lBytes = 0L;
							}
							else 
							{
								/* The input string was help. Pass the string to the command interpreter of help. */
								do
								{
									/* Get the next output string from the command interpreter. */
									memset(cOutputString, 0, cmdMAX_OUTPUT_SIZE);
									xReturned = FreeRTOS_CLIProcessCommand(cInputString, cOutputString, cmdMAX_OUTPUT_SIZE);
    					    		send(lClientFd, cOutputString, strlen((const char *)cOutputString), 0);
								} while(xReturned != pdFALSE);

								/* All the strings generated by the input
								command have been sent.  Clear the input
								string ready to receive the next command. */

								lInputIndex = 0;
								memset(cInputString, 0x00, cmdMAX_INPUT_SIZE);
								send( lClientFd, "\r\n>", strlen( "\r\n>" ), 0 );
							}
						}
						else
						{
							if(cInChar == '\r')
							{
								/* Ignore the character. */
							}
							else if(cInChar == '\b')
							{
								/* Backspace was pressed.  Erase the last
								character in the string - if any. */
								if(lInputIndex > 0)
								{
									lInputIndex--;
									cInputString[ lInputIndex ] = '\0';
								}
							}
							else
							{
								/* A character was entered.  Add it to the string
								entered so far.  When a \n is entered the complete
								string will be passed to the command interpreter. */
								if(lInputIndex < cmdMAX_INPUT_SIZE)
								{
									cInputString[ lInputIndex ] = cInChar;
									lInputIndex++;
								}
							}
						}
					}
					else if (lBytes == 0)
					{
						printf("socket closed %ld\r\n", lBytes);
					}
					else // if (lBytes < 0)
					{
						printf("socket timeout %ld\r\n", lBytes);
					}
				} while( lBytes != 0L );

				close( lClientFd );
			}
		}
	}

}

int32_t cli_init(void)
{
	int32_t eRet = 0;

	if( false == sCli.bFlagInit )
	{
		struct mq_attr attr;
		attr.mq_maxmsg = 1;
		attr.mq_msgsize = 100;
		sCli.mqCmd = mq_open("/cli_mq", O_CREAT|O_RDWR|O_CLOEXEC, 0666, &attr);
		if(sCli.mqCmd == (mqd_t)-1)
		{
			printf("cli mq open error\n");
			eRet = -1;
		}
		else
		{
			if( 0 == pthread_mutex_init( &(sCli.mutex), NULL ) )
			{
				if( 0 == pthread_create(&(sCli.pid), NULL, vBasicSocketsCommandInterpreterTask, &sCli) )
				{
					sCli.bFlagInit = true;
					eRet = 0;
				}
				else
				{
					printf("cli creat pthread error\n");
					eRet = -1;
				}
			}
			else
			{
				printf("cli creat mutex error\n");
				eRet = -1;
			}
		}
	}

	return eRet;
}