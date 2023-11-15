
#ifndef VERSION_H
#define VERSION_H

#include "../../inc/radar_common.h"

const char *get_embed_software_ps_version_string(void);

const char *get_embed_alink_version_string(void);

void get_embed_software_pL_version_string(uint32_t *version);

const char *get_algo_detect_version_string(void);

const char *get_algo_track_version_string(void);

#endif /* VERSION_H */
