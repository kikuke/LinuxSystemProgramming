#ifndef SSU_MONITOR_USAGE_H
#define SSU_MONITOR_USAGE_H

typedef enum SSU_Monitor_Idx {
	USAGEIDX_ADD,
	USAGEIDX_DELETE,
	USAGEIDX_TREE,
	USAGEIDX_HELP,
	USAGEIDX_EXIT,
	USAGEIDX_MAX
} SSU_MONITOR_IDX;

//Comment: 사용법을 출력한다. USAGEIDX_MAX가 인자로 들어오면 모든 사용법을 출력한다.
void Usage(SSU_MONITOR_IDX idx);

#endif