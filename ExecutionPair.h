#ifndef EXECUTIONPAIR_H
#define EXECUTIONPAIR_H

#include"Command.h"
#include<memory>
#include<string>

class ExecutionPair
{
public:
	// File constructor:
	// fd is an open file descriptor that we read from to create the object:
	// cmd1 and cmd2 are command objects that we create from the file input
	// in1/2, out1/2 reflect the communication model specified in the file input
	ExecutionPair(int fd);
	~ExecutionPair();

	// Executes cmd1 and cmd2, setting up communication as specified
	void execute();

private:
	Command* cmd1;
	Command* cmd2;
	int in1, out1, in2, out2;
};


#endif
