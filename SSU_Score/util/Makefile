LAYER0 = ssu_util_path.o ssu_util_scoretree.o
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

ssu_util_scoretree.o: ssu_util_scoretree.c
	$(CC) -c $^