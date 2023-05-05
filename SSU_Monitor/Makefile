INCLUDE = -Idefine/ -Iutil/
SUBDIRS = util
MODULE = ssu_monitor_add.o
UTILITY = util/ssu_monitor_usage.o util/ssu_monitor_system.o util/ssu_monitor_util.o util/ssu_monitor_monitlist_util.o
CC = gcc -g


#Init
init: create_monitor


#Operation
create_monitor:
	for DIR in $(SUBDIRS); do \
		$(MAKE) -C $$DIR; \
	done
	make ssu_monitor

clean: 
	for DIR in $(SUBDIRS); do \
		$(MAKE) -C $$DIR clean; \
	done
	rm *.o
	rm ssu_monitor


#Main File
ssu_monitor: ssu_monitor.o $(MODULE) $(UTILITY)
	$(CC) -o $@ $^


#Application
ssu_monitor.o: ssu_monitor.c
	$(CC) $(INCLUDE) -c $^

ssu_monitor_add.o: ssu_monitor_add.c
	$(CC) $(INCLUDE) -c $^