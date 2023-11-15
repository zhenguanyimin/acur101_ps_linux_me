#
#
#aux_source_directory(. DIR_SCRS)

#add_subdirectory(rpc)
#add_subdirectory(rpc_api)
#add_subdirectory(xen_vchan)

add_subdirectory(lmx2492_hal)
add_subdirectory(chc2442_hal)
add_subdirectory(led)

#aux_source_directory(. DIR_LIB_SRCS)
#add_library(main ${DIR_LIB_SRCS})
set(HAL_LIBS 
	${LMX2492_HAL_LIBS} 
	${CHC2442_HAL_LIBS} 
	${LED_LIBS} 
	PARENT_SCOPE)


