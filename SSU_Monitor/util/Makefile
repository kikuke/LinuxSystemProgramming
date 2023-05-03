INCLUDE = -I../define
LAYER0 =  ssu_backup_usage.o ssu_backup_path.o ssu_backup_hash.o
LAYER1 = ssu_backup_util.o
LAYER2 = ssu_backup_filetree_util.o
CC = gcc -g


#Init
init: create_utility


#Operation
create_utility:
	make $(LAYER0) $(LAYER1) $(LAYER2)

clean: 
	rm *.o


#Layer 0
ssu_backup_usage.o: ssu_backup_usage.c
	$(CC) $(INCLUDE) -c $^

ssu_backup_path.o: ssu_backup_path.c
	$(CC) $(INCLUDE) -c $^

ssu_backup_hash.o: ssu_backup_hash.c
	$(CC) $(INCLUDE) -c $^


#Layer 1
ssu_backup_util.o: ssu_backup_util.c
	$(CC) $(INCLUDE) -c $^

#Layer 2
ssu_backup_filetree_util.o: ssu_backup_filetree_util.c
	$(CC) $(INCLUDE) -c $^