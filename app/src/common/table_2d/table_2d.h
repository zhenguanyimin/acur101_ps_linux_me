#ifndef __TABLE_2D_H__
#define __TABLE_2D_H__

typedef struct table_2d
{
	uint32_t	uLineNum;
	uint32_t	uRowNum;
	int			aiTable[0];
}table_2d_t;

struct table_2d* file_to_table_2d( char* strFileName );

inline static
void table_2d_free( struct table_2d* psTable )
{
	free( psTable );
}

#endif	/* __TABLE_2D_H__ */

