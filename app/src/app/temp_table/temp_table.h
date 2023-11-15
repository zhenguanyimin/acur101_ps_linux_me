#ifndef __TEMP_TABLE_H__
#define __TEMP_TABLE_H__

unsigned long temp_table_lookup(float tempval);
int temp_table_load(void);
int temp_table_unload(void);

#endif

