#ifndef COMMAND_H
#define COMMAND_H

#include<string>
#include<vector>
#include<sstream>

using namespace std;

class Command
{
public:

	// File constructor:
	// fd: open file descriptor (for reading) that points to a file containing our command.
	// size: The amount of data we need to read from the file.
	// cmd is initialized from the file contents.
	// args is the tokenized version of cmd.
	// argv is a dynamically allocated set of pointers to the contents of args, with a NULL (for execvp purposes)
	Command(int fd, unsigned int size);
	~Command();

	// Returns argv
	// The user is not expected to free this, you are.
	char const** getArgs();

private:
	// The complete command line
	string cmd;

	// Tokenized version of cmd
	vector<string> args;


	// Pointers to the const char* elements with args.
	char const** argv;
};


#endif
