/*
CSE 109: Spring 2018
Dylan Spector
drs320
Contains Main() & is used to run and manage programs.
Program #6
*/
#include<stdio.h>
#include<stdlib.h>
#include<vector>
#include<sys/types.h>
#include<sys/wait.h>
#include<unistd.h>
#include<errno.h>
#include<string.h>
#include<signal.h>
#include<fcntl.h>
#include"Command.h"
#include"ExecutionPair.h"

using namespace std;

int main(int argc, char** argv)
{
	if(argc != 2) // looking for 1 argument
	{
		fprintf(stderr, "The wrong number of command line arguments were received.\n");
		return 1;
	}

	int fd = open(argv[1], O_RDONLY);
    if(fd == -1)
    {
        fprintf(stderr, "Open in of command line argument fd, failed. %d\n", errno);
        return 2;
    }

	unsigned short numCases = 0;
	ssize_t result = read(fd, &numCases, 2); // reads 1st 2 bytes, stores inunsigned short
    if(result != 2)
    {
        fprintf(stderr, "Read of Number Of Cases failed: %d\n", errno);
        close(fd);
        exit(1);
    }

	numCases = numCases / 2; // I WAS TOLD TO DO THIS FROM PIAZZA


	// create an Execution Pair for each ExecutionPair object (numCases)
	vector<ExecutionPair> execPairList; // Is this where I have problems ???
	while(numCases)
	{
		execPairList.push_back(ExecutionPair(fd));
		numCases--;
	}

	for(ExecutionPair i : execPairList) // Is this where I have a problem ???
	{
		i.execute();
	}

	// NEED TO WAIT FOR ALL CHILDREN PAIRS TO END
    pid_t whichChild = 0;
    int status = 0;
    while((whichChild = waitpid(-1, &status, WNOHANG | WUNTRACED | WCONTINUED)) > 0)
    {
        //A child has changed state. Let's see what happened:
        if(WIFEXITED(status))
        {
            fprintf(stdout, "Child %d terminated normally with value %d\n", whichChild, WEXITSTATUS(status));
        }
        else if(WIFSIGNALED(status))
        {
            fprintf(stdout, "Child %d terminated by signal %d\n", whichChild, WTERMSIG(status));
        }
        else if(WIFSTOPPED(status))
        {
            fprintf(stdout, "Child %d was stopped by signal %d\n", whichChild, WSTOPSIG(status));
        }
        else if(WIFCONTINUED(status))
        {
            fprintf(stdout, "Child %d was resumed\n", whichChild);
        }
        else
        {
            fprintf(stdout, "Unable to determine what happened to child %d\n", whichChild);
        }
    }

    if(errno != ECHILD)
    {
        perror("Wait errored but did not return ECHILD:");
		close(fd);
        exit(5);
    }

	close(fd);
	return 0;
}
