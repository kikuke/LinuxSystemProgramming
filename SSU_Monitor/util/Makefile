INCLUDE = -I../define
LAYER0 =  ssu_monitor_usage.o ssu_monitor_path.o ssu_monitor_system.o ssu_monitor_util.o ssu_monitor_monitlist_util.o ssu_monitor_monitree_util.o
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
ssu_monitor_usage.o: ssu_monitor_usage.c
	$(CC) $(INCLUDE) -c $^

ssu_monitor_path.o: ssu_monitor_path.c
	$(CC) $(INCLUDE) -c $^

ssu_monitor_system.o: ssu_monitor_system.c
	$(CC) $(INCLUDE) -c $^

ssu_monitor_util.o: ssu_monitor_util.c
	$(CC) $(INCLUDE) -c $^

ssu_monitor_monitlist_util.o: ssu_monitor_monitlist_util.c
	$(CC) $(INCLUDE) -c $^

ssu_monitor_monitree_util.o: ssu_monitor_monitree_util.c
	$(CC) $(INCLUDE) -c $^

#Layer 1

#Layer 2