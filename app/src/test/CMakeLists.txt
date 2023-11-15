#
#
#aux_source_directory(. DIR_SCRS)

add_subdirectory(ut)
#add_subdirectory(rpc_api)
#add_subdirectory(xen_vchan)

#aux_source_directory(. DIR_LIB_SRCS)
#add_library(main ${DIR_LIB_SRCS})
set(TEST_LIBS ${UT_LIBS} PARENT_SCOPE)
