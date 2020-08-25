/**   LICENSE
* Copyright (c) 2020 Genome Research Ltd.
*
* Author:  cgphelp@sanger.ac.uk
*
* This file is part of sequence_utils.
*
* sequence_utils is free software: you can redistribute it and/or modify it under
* the terms of the GNU Affero General Public License as published by the Free
* Software Foundation; either version 3 of the License, or (at your option) any
* later version.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
* FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more
* details.
*
* You should have received a copy of the GNU Affero General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*
*    1. The usage of a range of years within a copyright statement contained within
*    this distribution should be interpreted as being equivalent to a list of years
*    including the first and last year specified and all consecutive years between
*    them. For example, a copyright statement that reads ‘Copyright (c) 2005, 2007-
*    2009, 2011-2012’ should be interpreted as being identical to a statement that
*    reads ‘Copyright (c) 2005, 2007, 2008, 2009, 2011, 2012’ and a copyright
*    statement that reads ‘Copyright (c) 2005-2012’ should be interpreted as being
*    identical to a statement that reads ‘Copyright (c) 2005, 2006, 2007, 2008,
*    2009, 2010, 2011, 2012’."
*
*/

#ifndef _bed_access_h
#define _bed_access_h

#include <stdio.h>
#include <stdint.h>
#include "khash.h"

KHASH_MAP_INIT_STR(cod,uint64_t)

int bed_access_get_bed_range_from_file_by_index(char *file_loc, int index, char **chr, int *start, int *stop);

int bed_access_get_lines_in_file(char *file_loc);

#endif
