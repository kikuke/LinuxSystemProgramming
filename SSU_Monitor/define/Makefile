LAYER0 =  ssu_backup_filetree.o
CC = gcc -g


#Init
init: create_define


#Operation
create_define:
	make $(LAYER0)

clean: 
	rm *.o


#Layer 0
ssu_backup_filetree.o: ssu_backup_filetree.c
	$(CC) -c $^