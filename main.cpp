#include <iostream>
#include <cstdlib> //for getenv

#include "ffrec.hpp"
#include "httpmodule.hpp"

#include <errno.h>
#include <signal.h>
#include <sys/resource.h>
#include <sys/file.h> //lock file

using namespace std;

Httpmodule httpm;
FFrec* f;

void sig_handler(int signo);

SOCKET conn;
int port=9600;
int test=0; //switch to 1 to test
int user=0;

int main( int argc, const char** argv )
{

/************** PID File to avoid multiple instances*************/
    int pid_file = open("pid.pid", O_CREAT | O_RDWR, 0666);
	int rc = flock(pid_file, LOCK_EX | LOCK_NB);
	if(rc<0) {
    		cout<<"another instance is running... bye"<<endl;
    		close(pid_file);
    		exit(errno); // another instance is running
    }
/****************************************************************/

/********************* SIGNAL HANDLING **************************/
    signal(SIGUSR1, sig_handler);
    signal(SIGUSR2, sig_handler);
    signal(SIGINT, sig_handler);
/****************************************************************/

/**************************** DAEMON ****************************/
//   if(daemon(1,0)<0){
//		cout<<errno<<endl;
//		exit(errno);
//	}
/****************************************************************/

/********************** GET ARGS ********************************/	
for(int i=0; i<argc; ++i){
	cout<<argv[i]<<endl;
	if(!strcmp(argv[i],"-u") && argv[i+1]!=""){
		f = new FFrec(argv[i+1]);
		user=1;
	}
	if(!strcmp(argv[i], "-t"))
		test=1;
	if(!strcmp(argv[i], "-p"))
		port=atoi(argv[i+1]);
}
/***************************************************************/

/********************** FFREC start ****************************/
if(!user) f = new FFrec(getenv("USER"));
/***************************************************************/

/********************* TEST MODE (-t) **************************/
if(test) f->show();
/***************************************************************/
   
/********************* HTTP SERVER *****************************/
 httpm.startHttpServer(port);
 cout<<"waiting on port "<< port << endl;
	while(1){
		conn = httpm.waitConnection();
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

void sig_handler(int signo)
{
  if(signo==SIGUSR1){
  	cout<<"i'm alive!"<<endl;
  	errno = 0;
	struct rusage* memory = (struct rusage*)malloc(sizeof(struct rusage));
	getrusage(RUSAGE_SELF, memory);
	printf("Usage: %ld\n", memory->ru_ixrss);
	printf("Usage: %ld\n", memory->ru_isrss);
	printf("Usage: %ld\n", memory->ru_idrss);
	printf("Max: %ld\n", memory->ru_maxrss);
	}

  if (signo!=SIGUSR1){
  	cout<<"interrupted!"<<endl;
  	httpm.close();
  	delete f;
  	exit(0);
  }
}