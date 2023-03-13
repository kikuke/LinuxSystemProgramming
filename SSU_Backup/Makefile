UTILITY = ssu_backup_util.o
CC = gcc

init: create_backup

setting:
	sh ssu_backup_setting.sh

create_backup:
	make ssu_backup
	make ssu_backup_add

ssu_backup: ssu_backup.o $(UTILITY)
	$(CC) -o $@ $^

ssu_backup_add: ssu_backup_add.o $(UTILITY)
	$(CC) -o $@ $^

ssu_backup_util.o: ssu_backup_util.c
	$(CC) -c $^

ssu_backup.o: ssu_backup.c
	$(CC) -c $^

ssu_backup_add.o: ssu_backup_add.c
	$(CC) -c $^

clean: 
	rm ssu_backup_util.o
	rm ssu_backup
	rm ssu_backup.o
	rm ssu_backup_add
	rm ssu_backup_add.o
