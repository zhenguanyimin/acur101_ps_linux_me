
#include <stdio.h>

#include "ut_version.h"

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* FreeRTOS+CLI includes. */
#include "../../srv/cli/FreeRTOS_CLI.h"
#include "../../srv/version/version.h"


static BaseType_t version_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);

static const CLI_Command_Definition_t version_cmd =
{
	"version",
	"\r\nversion:\r\n Show version info\r\n",
	version_cmd_handler,
	0
};

static BaseType_t version_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{

	uint32_t plVersion = 0;
	const char *embed_software_ps_version_string = NULL;
	const char *alg_detect_version_string = NULL;
	const char *alg_track_version_string = NULL;

	get_embed_software_pL_version_string(&plVersion);
	embed_software_ps_version_string = get_embed_software_ps_version_string();
	alg_detect_version_string = get_algo_detect_version_string();
	alg_track_version_string = get_algo_track_version_string();

	snprintf(pcWriteBuffer, xWriteBufferLen, "(%s %s)\r\nPS: %s\r\nPL: %x\r\nDetect: %s\r\nTrack: %s", \
				__DATE__, __TIME__, embed_software_ps_version_string, plVersion, alg_detect_version_string, alg_track_version_string);

	return pdFALSE;
}

void ut_version_init(void)
{
	FreeRTOS_CLIRegisterCommand(&version_cmd);
}
