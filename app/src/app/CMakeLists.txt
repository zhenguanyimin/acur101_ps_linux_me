
add_subdirectory(c2_network)
add_subdirectory(DataPath)
add_subdirectory(Wave_Configuration)
add_subdirectory(beam_calculate)
add_subdirectory(Detection)
add_subdirectory(Tracking)
add_subdirectory(C2_order)
add_subdirectory(DevInfoProcess)
add_subdirectory(sys_data)
add_subdirectory(temp_table)
add_subdirectory(pil)

set(APP_LIBS 
    ${C2_NETWORK_LIBS} 
    ${DATAPATH_LIBS} 
    ${WAVE_CONFIGURATION_LIBS} 
    ${BEAM_CALCULATE_LIBS} 
    ${DETECTION_LIBS} 
    ${TRACKING_LIBS} 
    ${C2_ORDER_LIBS} 
    ${DEVINFOPROCESS_LIBS} 
    ${SYS_DATA_LIBS} 
    ${TEMP_TABLE_LIBS} 
    ${PIL_LIBS}
    PARENT_SCOPE)

