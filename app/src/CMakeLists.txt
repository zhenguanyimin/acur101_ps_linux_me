#
#
#aux_source_directory(. DIR_SCRS)

add_subdirectory(common)
add_subdirectory(app)
add_subdirectory(drv)
add_subdirectory(hal)
add_subdirectory(srv)
add_subdirectory(test)
add_subdirectory(bsp)
add_subdirectory(service)

aux_source_directory(. DIR_LIB_SRCS)
add_library(main ${DIR_LIB_SRCS})
set(SRC_LIBS main 
	${COMMON_LIBS} 
	${APP_LIBS} 
	${DRV_LIBS}
	${HAL_LIBS}
	${SRV_LIBS}
	${TEST_LIBS}
	${BSP_LIBS}
	${SERVICE_LIBS}
	PARENT_SCOPE)

