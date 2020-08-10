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
	int time;
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

// init_cache(num of sets, num of lines, block size -> cache) : Build a new cache
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


//exit_cache(cache, num of sets -> void) : Destruct a cache
void exit_cache(cache sim_cache, mem_addr num_sets){
	int set_idx;
	
	for(set_idx = 0; set_idx < num_sets; set_idx++){
		free(sim_cache.sets[set_idx].lines);
	}
	
	free(sim_cache.sets);
}


cache_param run(cache sim_cache, cache_param param, mem_addr addr){
	int line_idx;
	int evict_idx = 0;
	int num_lines = param.E;
	int full = 1;
	
	// Tag bits
	mem_addr tag = addr >> (param.s + param.b);
	// Set index bits
	mem_addr num = (addr << (64 - (param.s + param.b))) >> (64 - param.s);
	
	cache_set set = sim_cache.sets[num];
	cache_line line;
	
	// Look around the determined set
	for(line_idx = 0; line_idx < num_lines; line_idx++){
		line = set.lines[line_idx];
		// Valid bit okay & Tag coincide 
		if(line.valid && line.tag == tag){
			param.hit++;
			set.lines[line_idx].timestamp = param.time;
			param.time++;
			return param;
		}
	}
	// All hitting cases returned
	param.miss++;
	
	// Check if the cache set is full to determine eviction
	for(line_idx = 0; line_idx < num_lines; line_idx++){
		line = set.lines[line_idx];
		
		if(!line.valid){
			full = 0;
			break;
		}
	}
	
	// Eviction not required : Cache is not full
	if(!full){
		set.lines[line_idx].tag = tag;
		set.lines[line_idx].valid = 1;
		set.lines[line_idx].timestamp = param.time;
		param.time++;
	}
	// Eviction required : Cache is full now
	else{
		// Finding a last recently used line
		line = set.lines[0];
		for(line_idx = 0; line_idx < num_lines; line_idx++){
			if(set.lines[line_idx].timestamp < line.timestamp){
				line = set.lines[line_idx];
				evict_idx = line_idx;
			}
		}
		set.lines[evict_idx].tag = tag;
		set.lines[evict_idx].timestamp = param.time;
		param.time++;
		param.evict++;		
	}
	
	return param;
}


int main(int argc, char *argv[])
{
	char parse;				// input variable for getopt
	char *trace = NULL;		// string variable for filename 
	
	FILE *fp = NULL;		// file pointer for read
	char inst;				// char variable for instruction
	mem_addr addr;			// address variable for instruction
	int size;				// size int variable for instruction
	
	
	
	//int verbosity = 0;
	
	cache_param param;		// parameter storage for cache
	cache sim_cache;		// cache declaration for sim

	
	memset(&param, 0, sizeof(cache_param));
	
	// Get arguments from the command line through getopt
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
	
	// Invalid argument input / Missing required arguments
	if (param.s == 0 || param.E == 0 || param.b == 0 || trace == NULL){
		printf("%s : Missing required command line\n", argv[0]);
	}
	
	param.S = (1 << param.s);
	param.B = (1 << param.b);
	
	sim_cache = init_cache(param.S, param.E, param.B);
	
	
	fp = fopen(trace, "r");
	if(fp != NULL){
		while(fscanf(fp, " %c %llx,%d", &inst, &addr, &size) == 3){
			switch(inst){
				case 'I' :
					break;
				
				case 'L' :
					param = run(sim_cache, param, addr);
					break;
				
				case 'S' :
					param = run(sim_cache, param, addr);
					break;
				
				case 'M' :
					param = run(sim_cache, param, addr);
					param = run(sim_cache, param, addr);
					break;
				
				default :
					break;
			}
		}
	}
	else{
		printf("%s : Invalid file name\n", trace);
	}
	
	
	exit_cache(sim_cache,param.S);
	fclose(fp);
	
    printSummary(param.hit, param.miss, param.evict);
	return 0;
}
