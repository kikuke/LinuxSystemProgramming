INCLUDE = -Idefine/ -Iutil/
SUBDIRS = define util
UTILITY =  util/ssu_monitor_system.o
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
ssu_monitor: ssu_monitor.o $(UTILITY)
	$(CC) -o $@ $^


#Application
ssu_monitor.o: ssu_monitor.c
	$(CC) $(INCLUDE) -c $^