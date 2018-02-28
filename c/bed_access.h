#ifndef _bed_access_h
#define _bed_access_h

#include <stdio.h>
#include <stdint.h>
#include "khash.h"

KHASH_MAP_INIT_STR(cod,uint64_t)

int bed_access_get_bed_range_from_file_by_index(char *file_loc, int index, char **chr, int *start, int *stop);

#endif
