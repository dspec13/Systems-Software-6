compile:
	g++ -Werror -Wall -g -c Command.cpp
	g++ -Werror -Wall -g -c ExecutionPair.cpp
	g++ -Werror -Wall -g -c Piper.cpp
	g++ -Werror -Wall -g -o piper Command.o ExecutionPair.o Piper.cpp

test:
	./piper ~jloew/CSE109/prog6student/file1.con
	~jloew/CSE109/prog6student/piper ~jloew/CSE109/prog6student/file1.con

checkpoint:
	cp Command.h checkpoint6.c
	cat Command.cpp >> checkpoint6.c
	~jloew/CSE109/submitCheckpoint.pl 6
