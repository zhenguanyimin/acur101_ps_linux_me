
#ifndef RADAR_ERROR_H
#define RADAR_ERROR_H

#define RADAR_ERRORNO_BASE (-1000)
typedef enum radar_ret_code {
    RET_OK = 0,
    RET_GENERAL_ERR = RADAR_ERRORNO_BASE - 1,
    RET_SYSTEM_ERR = RADAR_ERRORNO_BASE - 2,
    RET_NOT_SUPPORT = RADAR_ERRORNO_BASE - 3,
    RET_NO_RESOURCE = RADAR_ERRORNO_BASE - 4,
    RET_INVALID_PARAM = RADAR_ERRORNO_BASE - 5,
} ret_code_t;

#endif /* RADAR_ERROR_H */