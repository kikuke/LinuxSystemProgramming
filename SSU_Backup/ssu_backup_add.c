#include <unistd.h>

#include "ssu_backup_define.h"
#include "ssu_backup_usage.h"

int main(int argc, char* argv[])
{
	char opt;

	if(argc != 2){
		Usage(USAGEIDX_SSU_BACKUP);
	}

	while((opt = getopt(argc, argv, ""))){
	}
}
