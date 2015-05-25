#include <iostream>
#include <cstdlib> //for getenv

#include "ffrec.hpp"
#include "httpmodule.hpp"

#include <errno.h>
#include <signal.h>
#include <sys/resource.h>
#include <sys/file.h> //lock file

#define MOD_VIDEO 1
#define MOD_FRAME 0
#define HELP_MSG "Usage: usrdistd [options]\n\
Options:\n\
  -u {user}         Set {user}.conf file to use.\n\
  -m {mode}         Select frame mode: 0 -frame(default), 1 - video.\n\
  -t                Debug mode, (no http server).\n\
  -p {port}         Select custom port (9600 default) for transmission.\n\
  -h                Show this help message.\n\
\n\
  start             Start the program as daemon.\n\
  stop              Interrupt eventually running instance.\n"

using namespace std;

Httpmodule httpm;
FFrec* f;

void sig_handler(int signo);

SOCKET conn;
int port=9600;
int test=0; //switch to 1 to test
int user=0;
int mode=MOD_FRAME;
int daem=0;

int pid;

int main( int argc, const char** argv )
{
/******************** PID FILE LOCK AND WRITE *******************/
    int pid_file = open("pid.pid", O_CREAT | O_RDWR, 0666);
	int rc = flock(pid_file, LOCK_EX | LOCK_NB);
/***************************************************************/

/********************** GET ARGS ********************************/	
for(int i=0; i<argc; ++i){
	cout<<argv[i]<<endl;
	if(!strcmp(argv[i],"-u") && argv[i+1]!="")
		user=i;
	if(!strcmp(argv[i], "-t"))
		test=1;
	if(!strcmp(argv[i], "-p"))
		port=atoi(argv[i+1]);
	if(!strcmp(argv[i], "-m"))
		mode=atoi(argv[i+1]);
	if(!strcmp(argv[i], "-h")){
		cout<<HELP_MSG<<endl;
		exit(0);
	}
	if(!strcmp(argv[i], "start"))
		daem=1;
	if(!strcmp(argv[i], "stop")){
		//read the pid of the daemon and send SIGUSR2 to stop
		read(pid_file, &pid, sizeof(pid));
		kill(pid, SIGUSR2);
		sleep(1);
		kill(pid, SIGUSR2);
		exit(0);
	}
}
/***************************************************************/

/************** PID File to avoid multiple instances*************/
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
if(daem)
   	if(daemon(1,0)<0){
		cout<<errno<<endl;
		exit(errno);
	}

/****************************************************************/

/*************************** PID WRITE **************************/
 pid=getpid();
    write(pid_file, &pid, sizeof(pid));
/****************************************************************/

/********************** FFREC start ****************************/		
if(!user)
	f = new FFrec(getenv("USER"), mode);
else
	f = new FFrec(argv[user+1], mode);
/***************************************************************/

/********************* TEST MODE (-t) **************************/
if(test) f->show();
/***************************************************************/
   
/********************* HTTP SERVER *****************************/
 int token;
 httpm.startHttpServer(port);
 cout<<"waiting on port "<< port << endl;
 	while(1){
		conn = httpm.waitConnection();
		token=httpm.waitRequest(conn);
		cout<<"->[new req: "<<token<<"]"<<endl;
		cout<<"\t[analysis...]"<<endl;
		f->getFrame();

		if(f->find()){
			httpm.sendResponse(++token, f->getEyesDistance(), conn); 
		}else{
			httpm.sendResponse(++token, 0, conn); //no face detected
 			cout<<"\t[no face]"<<endl;
	     	}
     	cout<<"<-[send resp: "<< token <<"]"<<endl;
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