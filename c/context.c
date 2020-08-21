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

#include <getopt.h>
#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>
#include <ctype.h>
#include "htslib/faidx.h"
#include "bed_access.h"
#include "dbg.h"

int idx = 0;
char *output_file = NULL;
char *ref_file = NULL;
char *sections_bed = NULL;
int context_size = 3;

int check_exist(char *fname){
	FILE *fp;
	if((fp = fopen(fname,"r"))){
		fclose(fp);
		return 1;
	}
	return 0;
}

void print_version (int exit_code){
  printf ("%s\n",VERSION);
	exit(exit_code);
}

void print_usage (int exit_code){

	printf ("Usage: triplets -i index -o file.out -f reference.fa -s sections.bed [-h] [-v]\n\n");
	printf ("-i --index          Index of line in sections.bed to use.\n");
	printf ("-o --output         Output file.\n");
  printf ("-f --fasta          Reference fasta file e.g. genome.fa\n");
	printf ("-c --context-len    Length of contexts to count. Default [%d]\n",context_size);
  printf ("-s --sections       Sections bed file. Index is used to select the line in this file as the region to process.\n\n");
  printf ("Other:\n");
  printf ("-h --help           Display this usage information.\n");
	printf ("-v --version        Prints the version number.\n\n");
  exit(exit_code);
}

void options(int argc, char *argv[]){

  ref_file = NULL;

	const struct option long_opts[] =
	  {
             	{"version", no_argument, 0, 'v'},
             	{"help",no_argument,0,'h'},
              {"index",required_argument,0,'i'},
              {"fasta",required_argument,0,'f'},
              {"output",required_argument,0,'o'},
							{"context-len",required_argument,0, 'c'},
              {"sections",required_argument,0,'s'},
              { NULL, 0, NULL, 0}

   }; //End of declaring opts

   int index = 0;
   int iarg = 0;

   //Iterate through options
   while((iarg = getopt_long(argc, argv, "i:o:f:s:c:vh", long_opts, &index)) != -1){
   	switch(iarg){
   		case 'i':
        if(sscanf(optarg, "%i", &idx) != 1 || idx < 1){
      		fprintf(stderr,"Error parsing -i argument '%s'. Should be an integer > 0",optarg);
      		print_usage(1);
      	}
        break;

   		case 'o':
				output_file = optarg;
   			break;

   		case 'f':
   		  ref_file = optarg;
   		  break;

   		case 's':
        sections_bed = optarg;
        break;

			case 'c':
				context_size = atoi(optarg);
				break;

   		case 'h':
        print_usage(0);
        break;

      case 'v':
        print_version(0);
        break;

			case '?':
        print_usage (1);
        break;

      default:
      	print_usage (1);

   	}; // End of args switch statement

   }//End of iteration through options

   //Do some checking to ensure required arguments were passed and are accessible files
    if(ref_file != NULL){
      if(check_exist(ref_file) != 1){
        fprintf(stderr,"Reference fasta file (-f) %s does not exist.\n",ref_file);
        print_usage(1);
      }
    }else{
      fprintf(stderr,"Reference fasta file (-f) not defined.\n");
      print_usage(1);
    }

    if(sections_bed != NULL){
      if(check_exist(sections_bed) != 1){
        fprintf(stderr,"Sections bed file (-s) %s does not exist.\n",sections_bed);
        print_usage(1);
      }
    }else{
      fprintf(stderr,"Sections bed file (-s) not defined.\n");
      print_usage(1);
    }

    if(idx<1){
      fprintf(stderr,"Invalid index %i to lookup section in sections.bed\n",idx);
      print_usage(1);
    }

    if (output_file==NULL || strcmp(output_file,"/dev/stdout")==0) {
      output_file = "-";   // we recognise this as a special case
    }

   return;
}

int main(int argc, char *argv[]){
	options(argc, argv);
	char *chr = NULL;
	faidx_t *fai = NULL;
	char *seq = NULL;
	char *region = NULL;
	int start = 0;
	int stop = 0;
	FILE *output = NULL;
	khash_t(cod) *cod_hash = NULL;
	//Use index and sections bed to get the bed file section we want
	int res = bed_access_get_bed_range_from_file_by_index(sections_bed, idx, &chr, &start, &stop);
	check(res==1,"Error retrieving line from bed file.");
  //Retrieve reference sequence for the region
  fai = fai_load(ref_file);
  check(fai,"Error loading reference sequence file %s.",ref_file);
  region = malloc(sizeof(char *) * (strlen(chr)+60));
  check_mem(region);
  int out = sprintf(region,"%s:%d-%d",chr,start,stop);
  check(out>0,"Error writing region string.");
  int length_of_seq;
  seq = fai_fetch(fai,region,&length_of_seq);
  check(seq != NULL,"Error fetching sequence for region %s.",region);
  //Iterate through and add each codon to the counts
  cod_hash = kh_init(cod);
  int i=0;
  for(i=0;i<length_of_seq-(context_size-1);i++){
    //Build codon
    char *codon = malloc(sizeof(char)*(context_size+1));
    codon[context_size] = '\0';
    int s=0;
    int isn = 0;
    for(s=0;s<context_size;s++){
      codon[s] = toupper(seq[i+s]);
      if(codon[s] != 'A' && codon[s] != 'C' &&  codon[s] != 'G' && codon[s] != 'T'){
        isn = 1;
      }
    }
    if(isn==1){
      free(codon);
      continue;
    }
    int res;
    khiter_t k = kh_get(cod, cod_hash, codon);          // query the hash table
    if(k == kh_end(cod_hash)){
      k = kh_put(cod,cod_hash,codon,&res);
      kh_value(cod_hash,k) = 1;
    }else{
      kh_value(cod_hash,k) = kh_value(cod_hash,k)+1;
      free(codon);
    }
  }

  if (strcmp(output_file,"-")==0) {
    output = stdout;
  } else {
    output = fopen(output_file,"w");
  }
  check(output,"Error opening output file for write.");

  uint64_t val;
  khiter_t k;
  for (k = kh_begin(cod_hash); k != kh_end(cod_hash); ++k) {
    if (kh_exist(cod_hash, k)) {
       const char *key = kh_key(cod_hash,k);
       val = kh_value(cod_hash, k);
       fprintf(output,"%s\t%"PRIu64"\n", key, val);
    }
  }
  check(fflush(output)==0,"Error flushing output.");
  check(fclose(output)==0,"Error closing output file.");

  free(chr);
  fai_destroy(fai);
  free(seq);
  free(region);
  kh_destroy(cod,cod_hash);
  return 0;
error:
  if(chr) free(chr);
  if(seq) free(seq);
  if(fai) fai_destroy(fai);
  if(region) free(region);
  if(cod_hash) kh_destroy(cod,cod_hash);
  return 1;

}
