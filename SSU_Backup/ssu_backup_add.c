#include <unistd.h>
#include <stdlib.h>

#include "ssu_backup_define.h"
#include "ssu_backup_usage.h"
#include "ssu_backup_add.h"

int main(int argc, char* argv[])
{
	char opt;

	if(argc != 2){
		Usage(USAGEIDX_ADD);
		exit(1);
	}

	while((opt = getopt(argc, argv, ""))){
	}
}
