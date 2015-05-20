#include <iostream>

#include "ffrec.hpp"
#include "httpmodule.hpp"

#include "stdlib.h" //for getenv
#include "sys/file.h" //lock file
#include "errno.h"
#include "signal.h"

using namespace std;

Httpmodule httpm;
FFrec* f;

void sig_handler(int signo)
{
  if (signo == SIGUSR1){
    httpm.close();
	}

  if (signo == SIGUSR2){
  	httpm.close();
  	delete f;
  	exit(0);
  }
}


int test=0; //switch to 1 to test

int main( int argc, const char** argv )
{
/************** PID File to avoid multiple instances****/
    int pid_file = open("pid.pid", O_CREAT | O_RDWR, 0666);
	int rc = flock(pid_file, LOCK_EX | LOCK_NB);
	if(rc<0) {
    		cout<<"another instance is running... bye"<<endl;
    		exit(errno); // another instance is running
    }
/********************************************************/
/*************** SIGNAL HANDLING ************************/
    signal(SIGUSR1, sig_handler);
    signal(SIGUSR2, sig_handler);
/********************************************************/
/******************** DAEMON ****************************/
//   if(daemon(1,0)<0){
//		cout<<errno<<endl;
//		exit(errno);
//	}
/*******************************************************/
	
if(argc>1){
	for(int i=0; i<argc-1; i++){
		if(!strcmp(argv[i],"-u") && argv[i+1]!="")
			f = new FFrec(argv[i+1]);
		else
			f = new FFrec(getenv("USER"));
		
		if(!strcmp(argv[i], "-t"))
			test=1;
	}
}
/********** TEST *********************************************/
 //	while(1)
//		f->show();
/*************************************************************/
   
/********* HTTP SERVER ****************************************/
 httpm.startHttpServer(0);

	while(1){
		SOCKET conn = httpm.waitConnection();
		httpm.waitRequest(conn);

		f->getFrame();

		if(f->find()){
			httpm.sendResponse(f->getEyesDistance(), conn); 
		}else{
     			httpm.sendResponse(0, conn); //no face detected
     			cout<<"no face detected"<<endl;
	     	}
     	}
/**************************************************************/
     return 0;
}

