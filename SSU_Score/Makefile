SCORE = blank.o ssu_score.o
CC = gcc -g


#Init
init: create_score


#Operation
create_score:
	make ssu_score

clean:
	rm *.o
	rm ssu_score


#Main File
ssu_score: main.o $(SCORE)
	$(CC) -o $@ $^


#Application
blank.o: blank.c
	$(CC) -c $^

ssu_score.o: ssu_score.c
	$(CC) -c $^

main.o: main.c
	$(CC) -c $^