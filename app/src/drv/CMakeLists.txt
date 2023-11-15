#
#
#aux_source_directory(. DIR_SCRS)

add_subdirectory(chc2442_drv)
add_subdirectory(lmx2492_drv)
add_subdirectory(gyro)
add_subdirectory(lsm303ah)
add_subdirectory(gps)
add_subdirectory(awmf_0165)
add_subdirectory(uart)

#aux_source_directory(. DIR_LIB_SRCS)
#add_library(main ${DIR_LIB_SRCS})

set(DRV_LIBS 
    ${CHC2442_DRV_LIBS}
    ${LMX2492_DRV_LIBS}
    ${AWMF_0165_LIBS}
    ${GYRO_LIBS} 
    ${LSM303AH_LIBS} 
    ${MYGPS_LIBS} 
    ${UART_LIBS} 
    PARENT_SCOPE)


