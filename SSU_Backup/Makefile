INCLUDE = -Idefine/ -Iutil/
SUBDIRS = define util
UTILITY =  util/ssu_backup_usage.o util/ssu_backup_path.o util/ssu_backup_util.o
HASH =  util/ssu_backup_hash.o
FILETREE = define/ssu_backup_filetree.o util/ssu_backup_filetree_util.o
CRYPTO_MACRO = -lcrypto
CC = gcc -g


#Init
init: create_backup


#Operation
create_backup:
	for DIR in $(SUBDIRS); do \
		$(MAKE) -C $$DIR; \
	done
	make ssu_backup
	make ssu_backup_add
	make ssu_backup_remove
	make ssu_backup_recover
	make ssu_backup_help

clean: 
	for DIR in $(SUBDIRS); do \
		$(MAKE) -C $$DIR clean; \
	done
	rm *.o
	rm ssu_backup
	rm ssu_backup_add
	rm ssu_backup_remove
	rm ssu_backup_recover
	rm ssu_backup_help


#Main File
ssu_backup: ssu_backup.o $(UTILITY)
	$(CC) -o $@ $^

ssu_backup_add: ssu_backup_add.o $(UTILITY) $(HASH) $(FILETREE)
	$(CC) -o $@ $^ $(CRYPTO_MACRO)

ssu_backup_remove: ssu_backup_remove.o $(UTILITY) $(HASH) $(FILETREE)
	$(CC) -o $@ $^ $(CRYPTO_MACRO)

ssu_backup_recover: ssu_backup_recover.o $(UTILITY) $(HASH) $(FILETREE)
	$(CC) -o $@ $^ $(CRYPTO_MACRO)

ssu_backup_help: ssu_backup_help.o $(UTILITY)
	$(CC) -o $@ $^


#Application
ssu_backup.o: ssu_backup.c
	$(CC) $(INCLUDE) -c $^

ssu_backup_add.o: ssu_backup_add.c
	$(CC) $(INCLUDE) -c $^

ssu_backup_remove.o: ssu_backup_remove.c
	$(CC) $(INCLUDE) -c $^

ssu_backup_recover.o: ssu_backup_recover.c
	$(CC) $(INCLUDE) -c $^

ssu_backup_help.o: ssu_backup_help.c
	$(CC) $(INCLUDE) -c $^
