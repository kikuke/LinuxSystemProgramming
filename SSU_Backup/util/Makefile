INCLUDE = -Idefnie/ -Iutil/
UTILITY =  ssu_backup_usage.o ssu_backup_path.o ssu_backup_util.o
FILETREE = ssu_backup_filetree_util.o
CRYPTO_MACRO = -lssl -lcrypto -DOPENSSL_API_COMPAT=10101
CC = gcc -g


#Init
init: create_backup


#Operation
create_backup:
	make ssu_backup
	make ssu_backup_add
	make ssu_backup_help

clean: 
	rm *.o
	rm ssu_backup
	rm ssu_backup_add
	rm ssu_backup_help


#Main File
ssu_backup: ssu_backup.o $(UTILITY)
	$(CC) -o $@ $^

ssu_backup_add: ssu_backup_add.o $(UTILITY)
	$(CC) -o $@ $^ $(CRYPTO_MACRO)

ssu_backup_help: ssu_backup_help.o $(UTILITY)
	$(CC) -o $@ $^ $(CRYPTO_MACRO)


#Layer 0
ssu_backup_usage.o: util/ssu_backup_usage.c
	$(CC) $(INCLUDE)-c $^

ssu_backup_path.o: util/ssu_backup_path.c
	$(CC) $(INCLUDE)-c $^

ssu_backup_filetree.o: define/ssu_backup_filetree.c
	$(CC) $(INCLUDE)-c $^

ssu_backup_hash.o: util/ssu_backup_hash.c
	$(CC) $(INCLUDE)-c $^ $(CRYPTO_MACRO)


#Layer 1
ssu_backup_util.o: util/ssu_backup_util.c
	$(CC) $(INCLUDE)-c $^

ssu_backup_filetree_util.o: util/ssu_backup_filetree_util.c
	$(CC) $(INCLUDE)-c $^


#Application
ssu_backup.o: ssu_backup.c
	$(CC) $(INCLUDE)-c $^

ssu_backup_add.o: ssu_backup_add.c
	$(CC) $(INCLUDE)-c $^

ssu_backup_help.o: ssu_backup_help.c
	$(CC) $(INCLUDE)-c $^