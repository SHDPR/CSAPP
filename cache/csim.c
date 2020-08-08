// Writer Info
// Student ID : 20150310 / KAIST
// Name : Sangmin Park


#include "cachelab.h"

#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

// Data type for 64bit address
typedef unsigned long long int mem_addr;

// Cache Parameter 
typedef struct{
	int s;	// number of set bit
	int b;	// number of block bit
	int E;	// number of lines in a set, or Associativity
	mem_addr S;	// number of sets = 2^s
	mem_addr B; // number of bytes in a block = 2^b
	
	int hit;
	int miss;
	int evict;
}cache_param;

// Cache Line
typedef struct{
	int timestamp;	// flag for LRU eviction rule
	int valid;		// flag for validity check
	mem_addr tag;	// tag bits
}cache_line;

// Cache Set = Array of pointers to 'Cache Line'
typedef struct{
	cache_line *lines;
}cache_set;

// Cache = Array of pointers to 'Cache Set'
typedef struct{
	cache_set *sets;
}cache;

cache init_cache(mem_addr num_sets, int num_lines, mem_addr block_size){
	cache new_cache;
	int set_idx, line_idx;
	
	new_cache.sets = (cache_set *)malloc(sizeof(cache_set) * num_sets);
	
	for(set_idx = 0; set_idx < num_sets; set_idx++){
		new_cache.sets[set_idx].lines = (cache_line *)malloc(sizeof(cache_line) * num_lines);
		
		for(line_idx = 0; line_idx < num_lines; line_idx++){
			new_cache.sets[set_idx].lines[line_idx].timestamp = 0;
			new_cache.sets[set_idx].lines[line_idx].valid = 0;
			new_cache.sets[set_idx].lines[line_idx].tag = 0;
		}
		
	}
	
	return new_cache;
}



void exit_cache(cache sim_cache, mem_addr num_sets){
	int set_idx;
	
	for(set_idx = 0; set_idx < num_sets; set_idx++){
		free(sim_cache.sets[set_idx].lines);
	}
	
	free(sim_cache.sets);
}



int main(int argc, char *argv[])
{
	char parse;
	//int verbosity = 0;
	
	cache_param param;
	cache sim_cache;
	
	char *trace = NULL;
	
	memset(&param, 0, sizeof(cache_param));
	
	while ((parse = getopt(argc, argv, "s:E:b:t:vh")) != -1){
		switch(parse){
			case 's' : 
				param.s = atoi(optarg);
				break;
			case 'E' : 
				param.E = atoi(optarg);
				break;
			case 'b' : 
				param.b = atoi(optarg);
				break;
			case 't' : 
				trace = optarg;
				break;
			case 'v' :
				//verbosity = 1;
				break;
			case 'h' : 

				exit(0);
			default : 

				exit(1);

		}			
	}
	
	
	if (param.s == 0 || param.E == 0 || param.b == 0 || trace == NULL){
		printf("%s : Missing required command line\n", argv[0]);
	}
	
	param.S = (1 << param.s);
	param.B = (1 << param.b);
	
	sim_cache = init_cache(param.S, param.E, param.B);
	
	
	exit_cache(sim_cache,param.S);
	
	
	
	
    printSummary(0, 0, 0);
	return 0;
}
