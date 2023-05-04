INCLUDE = -I../define
LAYER0 =  ssu_monitor_daemon.o
LAYER1 = 
LAYER2 = 
CC = gcc -g


#Init
init: create_utility


#Operation
create_utility:
	make $(LAYER0) $(LAYER1) $(LAYER2)

clean: 
	rm *.o


#Layer 0
ssu_monitor_daemon.o: ssu_monitor_daemon.c
	$(CC) $(INCLUDE) -c $^


#Layer 1

#Layer 2