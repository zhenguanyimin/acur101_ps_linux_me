table_2d组件用于将文件中的字符串列表转换为二维表
提供 struct table_2d* file_to_table_2d( char* strFileName ) 函数转换
转换需要输入文件绝对路径
转换完成输出二维表信息，输出NULL则表示转换失败
返回的二维表使用后
需使用 void table_2d_free( struct table_2d* psTable ) 函数释放

二维表信息为：
typedef struct table_2d
{
	uint32_t	uLineNum;
	uint32_t	uRowNum;
	int	aiTable[0];
}table_2d_t;

表中显示行和列的数量，如需访问第x行第y列，则
table[x][y] = aiTable[x*uRowNum + y];
需确认访问没有越界

该代码在文件较大时会消耗大块内存，存在优化空间