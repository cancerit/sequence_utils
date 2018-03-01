# sequence_utils

A collection of sequence related scripts.

## Scripts

### context_counter

Counts occurrences of of every possible context of a given length within a fasta sequence.

```
$ install/bin/context_counter
Reference fasta file (-f) not defined.
Usage: triplets -i index -o file.out -f reference.fa -s sections.bed [-h] [-v]

-i --index          Index of line in sections.bed to use.
-o --output         Output file.
-f --fasta          Reference fasta file e.g. genome.fa
-c --context-len    Length of contexts to count. Default [3]
-s --sections       Sections bed file. Index is used to select the line in this file as the region to process.

Other:
-h --help           Display this usage information.
-v --version        Prints the version number.
```
