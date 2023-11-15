提供sflog宏接口，用于输出log信息
依赖syslog组件
SFLOG_OUTPUT_FUNC 宏用于选择是否添加函数信息
添加的函数信息有线程ID，文件名称，行号
SFLOG_OUTPUT_TIME 宏用于输出时间信息
输出时间信息包含UTC时间，ns计数
SFLOG_OUTPUT_TIME = 0 表示不添加时间信息
SFLOG_OUTPUT_TIME = 1 表示添加UTC时间
SFLOG_OUTPUT_TIME > 1 表示添加UTC时间和ns计数