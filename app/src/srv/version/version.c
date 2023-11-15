#include <stdint.h>
#include "version.h"

#include "../../bsp/reg/pl_reg.h"
#include "../../app/Detection/detection_interface.h"

#define VERSION_WRITE_REG(addr,data)	PL_REG_WRITE(0xFFFF&addr,data)
#define VERSION_READ_REG(addr)			PL_REG_READ(0xFFFF&addr)

const char *get_embed_software_ps_version_string(void)
{
	return EMBED_SOFTWARE_PS_VERSION_STR;
}


const char *get_embed_alink_version_string(void)
{
	return EMBED_ALINK_VERSION_STR;
}


void get_embed_software_pL_version_string(uint32_t *version)
{
	*version = VERSION_READ_REG(0x80020000);
}

const char *get_algo_detect_version_string(void)
{
	return GetDetectAlgVersion();
}

const char *get_algo_track_version_string(void)
{
	return GetTrackAlgVersion();
}


