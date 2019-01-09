/*
CSE 109: Spring 2018
Dylan Spector
drs320
Finds, stores, and executes Execution Pairs
Program #6
*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<string>
#include<vector>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<errno.h>
#include<unistd.h>
#include<sstream>
#include<iostream>
#include"Command.h"
#include"ExecutionPair.h"

using namespace std;

ExecutionPair::ExecutionPair(int fd)
{
	// Check to make sure that fd is a real value (presumes fd is already open)
    if(fd == -1)
    {
        exit(1);
    }

	// Read in the Comm. Model from fd & assign to in1, out1, in2, out2.
	char commModel = '\0';
    ssize_t result = read(fd, &commModel, 1); // reads 1st 2 bytes, stores inunsigned short
    if(result != 1)
    {
        fprintf(stderr, "Read of Comm. Model Failed, %d\n", errno);
        close(fd);
        exit(1);
    }

	in1 = commModel & 3;
	commModel = commModel >> 2;
	out1 = commModel & 3;
	commModel = commModel >> 2;
	in2 = commModel & 3;
	commModel = commModel >> 2;
	out2 = commModel & 3;

	// Store values sizeA & sizeB
	unsigned int sizeA = 0;
	result = read(fd, &sizeA, 4);
    if(result != 4)
    {
        fprintf(stderr, "Read of sizeA Failed. %d \n", errno);
        close(fd);
        exit(1);
    }

	unsigned int sizeB = 0;
    result = read(fd, &sizeB, 4);
    if(result != 4)
    {
        fprintf(stderr, "Read of sizeB Failed. %d \n", errno);
        close(fd);
        exit(1);
    }

	// Store Command1 & Command2 pointers
	cmd1 = new Command(fd, sizeA);
	cmd2 = new Command(fd, sizeB);
}

ExecutionPair::~ExecutionPair()
{
	delete(cmd1);
	delete(cmd2);
}

void ExecutionPair::execute()
{
	// deterine the current Communication Model
	int makePipe1 = 0; // 1out -> 2in
	int makePipe2 = 0; // 2out -> 1n

	if(in1 == 3 && out1 == 2 && in2 == 1 && out2 == 0) // Loop
    {
		makePipe1 = 1;
        makePipe2 = 1;
    }
    else if(in1 == 0 && out1 == 2 && in2 == 1 && out2 == 3) // c1->c2
    {
		makePipe1 = 1;
    }
    else if(in1 == 3 && out1 == 1 && in2 == 2 && out2 == 0) // c2->c1
    {
		makePipe2 = 1;
    }

	// Make Pipes (both require duping after children are made)
	int fd1[2];
	int fd2[2];
	if(makePipe1)
	{
		pipe(fd1);
	}
	if(makePipe2)
	{
        pipe(fd2);
	}

	int returnVal = fork();
	if(returnVal == -1)
	{
		perror("first fork failed");
		exit(1);
	}
	else if(!returnVal) // currently in child1
	{
		if(makePipe1)
	    {
			dup2(fd1[1], STDOUT_FILENO);
    	}
    	if(makePipe2)
    	{
			dup2(fd2[0], STDOUT_FILENO);
    	}

		int result = execvp(cmd1->getArgs()[0], (char* const*)cmd1->getArgs());
		if(result == -1)
		{
			fprintf(stderr, "execvp failed %d\n", errno);
        	if(makePipe1) // Won't this cause issues?
            {
                close(fd1[0]);
                close(fd1[1]);
            }
            if(makePipe2)
            {
                close(fd2[0]);
                close(fd2[1]);
            }
        	exit(3);
		}
		exit(0);
	}
	else // currently in parent
	{
		returnVal = fork();
		if(returnVal == -1)
    	{
        	perror("second fork failed");
        	exit(1);
    	}
    	else if(!returnVal) // currently in child2
    	{
			if(makePipe1)
			{
				dup2(fd1[0], STDIN_FILENO);
			}
			if(makePipe2)
			{
				dup2(fd2[1], STDIN_FILENO);
			}

			// Receiving result == -1
			int result = execvp(cmd2->getArgs()[0], (char* const*)cmd1->getArgs());
			if(result == -1)
			{
				fprintf(stderr, "execvp failed. %d\n", errno);
				if(makePipe1) // Won't this cause issues?
            	{
                	close(fd1[0]);
                	close(fd1[1]);
            	}
            	if(makePipe2)
            	{
                	close(fd2[0]);
                	close(fd2[1]);
            	}
				exit(3);
			}
			exit(0);
    	}
    	else // currently back in parent
    	{
			if(makePipe1)
			{
				close(fd1[0]);
				close(fd1[1]);
			}
			if(makePipe2)
			{
				close(fd2[0]);
				close(fd2[1]);
			}
			return; // parent does not wait
    	}
	}
}
