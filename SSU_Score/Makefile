INCLUDE = -Iutil/
SUBDIRS = util
UTILITY = util/ssu_util_path.o
SCORE = blank.o ssu_score.o
CC = gcc -g


#Init
init: create_score


#Operation
create_score:
	for DIR in $(SUBDIRS); do \
		$(MAKE) -C $$DIR; \
	done
	make ssu_score

clean:
	for DIR in $(SUBDIRS); do \
		$(MAKE) -C $$DIR clean; \
	done
	rm *.o
	rm ssu_score


#Main File
ssu_score: main.o $(SCORE) $(UTILITY)
	$(CC) -o $@ $^


#Application
blank.o: blank.c
	$(CC) -c $^

ssu_score.o: ssu_score.c
	$(CC) $(INCLUDE) -c $^

main.o: main.c
	$(CC) -c $^