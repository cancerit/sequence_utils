#include "dbg.h"
#include "bed_access.h"

int bed_access_get_bed_range_from_file_by_index(char *file_loc, int index, char **chr, int *start, int *stop){
  FILE *bedf = NULL;
  bedf = fopen(file_loc,"r");
  check(bedf>0,"Error opening bed file for reading '%s'.",file_loc);
  int line_no = 0;
  char rd[4096];
  int found = 0;
	while(fgets(rd, 4096, bedf) != NULL){
		check(rd != NULL,"Invalid line read in ignored region file.");
		line_no++;
		if (line_no != index) continue;
		char *chr_nom = malloc(sizeof(char *));
		check_mem(chr_nom);
		int beg,end;
		int chk = sscanf(rd,"%s\t%d\t%d",chr_nom,&beg,&end);
		check(chk == 3,"Incorrect bed line '%s' could not be parsed from bed file.\n",rd);
		*chr = malloc(sizeof(char) * (strlen(chr_nom)+1));
		check_mem(chr);
		char *cpy_chk = strcpy(*chr,chr_nom);
		check((cpy_chk != NULL && strcmp(cpy_chk,chr_nom)== 0 && strcmp(cpy_chk,*chr)== 0),"Error copying chr name to returned string.");
    *start = beg + 1; //+1 as bed is half open
    *stop = end;
    found = 1;
    break;
	}
	if(found==0){
	  sentinel("Index %d not found in bed file %s. Is your index too high?\n",index,file_loc,1);
	}
	int chk = fclose(bedf);
  check(chk==0,"Error closing bed file '%s'.",file_loc);
  return 1;
error:
	if(bedf) fclose(bedf);
	return 0;
}
