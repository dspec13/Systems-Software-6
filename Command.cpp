/*
CSE 109: Spring 2018
Dylan Spector
drs320
Used to store commands from the user
Program #6
*/

#include<stdio.h>
#include<stdlib.h>
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

using namespace std;

Command::Command(int fd, unsigned int size)
{
	// Check to make sure that fd is a real value (presumes fd is already open)
	if(fd == -1)
	{
		exit(1); // what to do here?
	}

	// Read in commands from cmdLine & store in string cmd
	char* cmdLine = (char*)malloc((size+1)*sizeof(char));
	ssize_t result = read(fd, cmdLine, size);
	if (result != size)
	{
		exit(1); // what to do here
	}

	cmdLine[size] = '\0';
	cmd.assign(cmdLine, size + 1); // cmd = string(cmdLine)
	free(cmdLine);
	cmdLine = NULL;

	// Set value of vector<string> args
	ssize_t numTokens = 0;
	istringstream checker(cmd); // create a stringstream from cmd
	while(checker)
	{
		string temp;
		checker >> temp;
		if(!checker.fail())
		{
			args.push_back(temp);
			numTokens++;
		}
	}

	// Set value of char const** argv
	argv = (char const**)malloc((numTokens+1) * sizeof(char*));
	for(int i = 0; i < numTokens; i++)
	{
		argv[i] = args[i].c_str();
	}
	argv[numTokens] = NULL;
}

Command::~Command()
{
	free(argv);
}

char const** Command::getArgs()
{
	return argv;
}
