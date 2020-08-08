#include "cachelab.h"

#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct{
	int valid;
}cache_line;

typedef struct{
	int valid;
}cache_set;

int main()
{
    printSummary(0, 0, 0);
	return 1;
}
