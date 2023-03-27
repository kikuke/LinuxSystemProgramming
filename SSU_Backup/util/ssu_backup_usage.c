#include <stdio.h>

#include "ssu_backup_usage.h"

const char* usage_message[USAGEIDX_MAX] = {
"add  <FILENAME> [OPTION]\n\
    -d : add directory recursive",
"remove <FILENAME> [OPTION]\n\
    -a : remove all file(recursive)\n\
    -c : clear backup directory",
"recover <FILENAME> [OPTION]\n\
    -d : recover directory recursive\n\
    -n <NEWNAME> : recover file with new name",
"ls",
"vi",
"vim",
"help",
"exit"
};

void Usage(SSU_BACKUP_IDX idx)
{
	if(idx != USAGEIDX_MAX){
		fputs("Usage : ", stdout);
		puts(usage_message[idx]);
		return;
	}

	puts("Usage:");
	for(int i=0; i<USAGEIDX_MAX; i++){
		fputs("  > ", stdout);
		puts(usage_message[i]);
	}
}
