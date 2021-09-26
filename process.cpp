/* @file process.cpp
@author Ekren:Mert:2020700063
@brief Process program for handling signals from executor.
@date Sunday, January 03, 2021
*/

//Include section
#include <iostream>
#include <signal.h>
#include <unistd.h>
#include <fstream>

struct timespec delta = {0 /*secs*/, 300000000 /*nanosecs*/}; //0.3 sec
using namespace std;

ofstream myfile;  /*Object to File*/
int p_index = -1; /*Process Number/Index*/
char *dir; /*Directory for Process Output*/
/*
Signal handler for non-terminal signals. It writes to process_out
file the index of process and the signal number.
@return void.
@param signal integer number
*/
static void sigHandler(int sig)
{
	myfile.open(dir,ios::app);
	myfile << "P" << p_index << " received signal " << sig << "\n"; //P3 received signal 1
	myfile.close();
    nanosleep(&delta, &delta);
}
/*
Signal handler for SIGTERM signal which exits from the process.
 It writes to process_out file the index of process and the signal number.
@return void.
@param signal integer number
*/
static void sigHandlerTerm(int sig)
{
	myfile.open(dir,ios::app);
	myfile << "P" << p_index << " received signal " << sig << ", terminating gracefully" << "\n";
	myfile.close();
    exit(p_index);
}

int main(int argc, char **argv)
{
	dir = argv[1];
	p_index = (int)*argv[0];
	myfile.open(dir, ios::app);
	myfile << "P" << p_index <<" is waiting for a signal\n"; /*TODO: P1 bastırılacak pid degil*/
	myfile.close();
    int j;

    while(1)
    {
    	if     (signal(SIGHUP,  sigHandler) == SIG_ERR ||
    			signal(SIGINT,  sigHandler) == SIG_ERR ||
				signal(SIGILL,  sigHandler) == SIG_ERR  ||
				signal(SIGTRAP, sigHandler) == SIG_ERR ||
				signal(SIGFPE,  sigHandler) == SIG_ERR ||
				signal(SIGSEGV, sigHandler) == SIG_ERR ||
				signal(SIGXCPU, sigHandler) == SIG_ERR)
    		myfile << "can't signal";

    	if (signal(SIGTERM, sigHandlerTerm) == SIG_ERR)
    		myfile << "can't signal";

    }
    return 0;
}
