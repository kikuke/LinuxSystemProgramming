#include <stdio.h>

#include "ssu_monitor_usage.h"

const char* usage_message[USAGEIDX_MAX] = {
"add  <DIRPATH> [OPTION]\n\
    -t <TIME> : set add daemon loop time",
"delete <DAEMON_PID>",
"tree <DIRPATH>",
"help",
"exit"
};

void Usage(SSU_MONITOR_IDX idx)
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