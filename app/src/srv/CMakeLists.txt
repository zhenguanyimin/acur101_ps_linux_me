#
#
#aux_source_directory(. DIR_SCRS)

add_subdirectory(alink)
add_subdirectory(cli)
add_subdirectory(flash_nv)
add_subdirectory(protocol)
add_subdirectory(pl_test)
add_subdirectory(log)
add_subdirectory(version)
add_subdirectory(record)
add_subdirectory(eth_link)
add_subdirectory(wit)
add_subdirectory(net_cfg)

#aux_source_directory(. DIR_LIB_SRCS)
#add_library(main ${DIR_LIB_SRCS})
set(SRV_LIBS 
    ${ALINK_LIBS} 
    ${CLI_LIBS} 
    ${FLASH_NV_LIBS} 
    ${PROTOCOL_LIBS} 
    ${PL_TEST_LIBS} 
    ${VERSION_LIBS} 
    ${RECORD_LIBS} 
    ${ETH_LINK_LIBS} 
    ${LOG_LIBS}
    ${WIT_LIBS}
    ${NET_CFG_LIBS}
    PARENT_SCOPE)
