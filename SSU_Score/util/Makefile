LAYER0 = ssu_util_path.o
CC = gcc -g


#Init
init: create_utility


#Operation
create_utility:
	make $(LAYER0)

clean: 
	rm *.o


#Layer 0
ssu_util_path.o: ssu_util_path.c
	$(CC) -c $^