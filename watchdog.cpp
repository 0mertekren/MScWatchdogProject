/** @file watchdog.cpp
@author Ekren:Mert:2020700063
@version Revision 1.1
@brief Watchdog program, creating processes & detecting killed
processes
@details
@date Sunday, January 03, 2021
 */

/**
@mainpage
This project includes three different programs which include
a watchdog procedure for N processes. Processes can handle
signals coming from executor program and terminate gracefully.
 */

//Include section
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <wait.h>
#include <sys/stat.h>
#include <iostream>
#include <csignal>
#include <sstream>
#include <fstream>
#include <fcntl.h>
#include <string>
using namespace std;


//Global variables section
struct timespec delta = {0, 300000000}; //0.3 sec
ofstream wd_out; // File outstream
char *dir; // File directory

void static sigHandlerTerm
(
		int sig //sig
);


int main(
		int argc,
		char **argv
)
{
	//Local Variables
	pid_t pid; // pid value used in fork
	int num_of_processes; // recieved in command line
	int i; // indexing
	pid_t *process_ids; // child pids
	int unnamedPipe; // fifo
	char * myfifo = (char*) "/tmp/myfifo"; // ptr to fifo file
	char temp[30]; // temp array
	char *arg[2]; // argument to pass to childs

	if(argc != 4)
	{
		cout << "Wrong number of inputs!\n";
		return -1;
	}
	/* Removes the files beforehand in order to avoid
	 * probems with existing file
	 * */
	remove(argv[2]);
	remove(argv[3]);

	num_of_processes = stoi(argv[1]);
	dir = argv[3];

	process_ids = new pid_t[num_of_processes+1]; // child pids

	mkfifo(myfifo, 666);
	unnamedPipe = open(myfifo, O_RDWR);

	if (signal(SIGTERM, sigHandlerTerm) == SIG_ERR)
		wd_out << "signal";

	sprintf(temp, "P%d %d", 0, getpid());
	write(unnamedPipe, temp, 30);
	process_ids[0] = getpid();

	for(i=1; i<= num_of_processes; i++)
	{
		pid = fork();
		nanosleep(&delta, &delta);
		if(pid == 0)
		{
			nanosleep(&delta, &delta);
			wd_out.open(dir, ios::app);
			wd_out << "P" << i << " is started and it has a pid of " << getpid() << "\n";
			wd_out.close();
			sprintf(temp, "P%d %d", i, getpid());
			write(unnamedPipe, temp, 30);
			arg[0] = (char *) &i;
			execl("./process", arg[0], argv[2], NULL);
		}
		process_ids[i] = pid;
	}

	/*
	 * Handle signals if there's zombie child.
	 */
	while(1)
	{
		pid_t dead_child = wait(NULL);

		for(i=1; i<=num_of_processes; i++)
		{
			if(dead_child == process_ids[i])
				break;
		}
		if(i > num_of_processes)
		{
			continue;
		}

		if(i==1)
		{
			wd_out.open(dir, ios::app);
			wd_out << "P1 is killed, all processes must be killed\n";
			wd_out << "Restarting all processes\n";
			wd_out.close();
			for(int i=1; i<=num_of_processes; i++)
			{
				kill(process_ids[i], SIGTERM);
				nanosleep(&delta, &delta);
			}
			for(int i=1; i<=num_of_processes; i++)
			{
				pid = fork();
				nanosleep(&delta, &delta);
				if(pid == 0)
				{
					nanosleep(&delta, &delta);
					sprintf(temp, "P%d %d", i, getpid());
					wd_out.open(dir, ios::app);
					wd_out << "P" << i << " is started and it has a pid of " << getpid() << "\n";
					wd_out.close();
					write(unnamedPipe, temp, 30);
					arg[0] = (char *) &i;
					execl("./process", arg[0], argv[2], NULL);
				}
				process_ids[i] = pid;
			}
		}
		else
		{
			kill(process_ids[i], SIGTERM);
			wd_out.open(dir, ios::app);
			wd_out << "P" << i << " is killed\n";
			wd_out.close();
			pid = fork();
			if(pid == 0)
			{
				wd_out.open(dir, ios::app);
				wd_out << "Restarting P" << i << "\n";
				wd_out.close();
				nanosleep(&delta, &delta);
				sprintf(temp, "P%d %d", i, getpid());
				write(unnamedPipe, temp, 30);
				arg[0] = (char *) &i;
				wd_out.open(dir, ios::app);
				wd_out << "P" << i << " is started and it has a pid of " << getpid() << "\n";
				wd_out.close();
				execl("./process", arg[0], argv[2], NULL);
			}
			process_ids[i] = pid;
		}
	}
	return 0;
}

//Function definition section
//Definitions must be in the same order as the prototypes.
void static sigHandlerTerm
(
		int sig // here
)
{
	wd_out.open(dir, ios::app); //opens
	wd_out << "Watchdog is terminating gracefully\n"; //here
	wd_out.close(); //closes
	nanosleep(&delta, &delta);
	exit(0);
}
