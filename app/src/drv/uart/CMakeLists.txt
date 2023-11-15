#
#
#aux_source_directory(. DIR_SCRS)

#add_subdirectory(lmx2492_hal)

aux_source_directory(. DIR_LIB_SRCS)
add_library(uart ${DIR_LIB_SRCS})
set(UART_LIBS uart PARENT_SCOPE)


