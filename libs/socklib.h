/* 
 * File:   socklib.h
 * Author: Mascari Giovanni s206587
 *
 * Created on 14 giugno 2014, 21.01
 * for the course of Distributed Programming 2013/2014
 */

#ifndef SOCKLIB_H
#define	SOCKLIB_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "socklib.h"
#include "unistd.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "sys/socket.h"
#include "arpa/inet.h"
#include "netinet/in.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "sys/select.h"
#include "errno.h"
#include "sys/time.h"
#include <signal.h>
#define STATIC_WAIT_INITIALIZED 1
#define STATIC_WAIT_NOTINITIALIZED 0    
    
#define INTERRUPTED_BY_SIGNAL (errno == EINTR || errno == ECHILD)

    typedef int SOCKET;
    typedef struct sockaddr_in ADDRIN;
    typedef struct sockaddr ADDR;
    
    //global variables
    static SOCKET currsock;
    
    static int _DEBUG = 1; //call setDEBUG(true); to turn ON debug msg
    static int _WAIT = STATIC_WAIT_NOTINITIALIZED;
    static fd_set cset;
    static long wtime;
    static struct timeval tv;

/**************************** LIST OF FUNCTIONS ***************************  
    SOCKET CreateSocketTCP();
    SOCKET CreateSocketUDP();
    void setCurrSOCK(SOCKET newsock);
//send & rcv TCP --> refering to GLOBAL CurrSOCK
    int SendNumber(SOCKET sock, int num);
    int RecvNumber(SOCKET sock, int* num);
    int SendString(SOCKET sock, const char* string);
    int RecvString(SOCKET sock, char* string, ssize_t maxsize);
    int TryRecvString(SOCKET sock, char* string, ssize_t maxsize, int ntry);
    size_t Send(SOCKET sock, void* buff, ssize_t size);
    size_t Recv(SOCKET sock,void* buff, ssize_t maxsize);
    size_t RecvDontWait(SOCKET sock,void* buff, ssize_t maxsize);
    size_t TryRecv(SOCKET sock,void* buff, ssize_t maxsize, int ntry);
    int SendFile(SOCKET sock, char* path,ssize_t chunksize);
    int RecvFile(SOCKET sock, char* path, ssize_t chunksize, int ntry);
//send & rcv UDP --> refering to a GLOBAL SOCK
    int SendStringTo(SOCKET sock, const char* string, ADDRIN *addr, socklen_t addrl);
    int RecvStringFrom(SOCKET sock, char* string, ssize_t maxsize, ADDRIN *addr, socklen_t* addrl);
    int TryRecvStringFrom(SOCKET sock, char* string, ssize_t maxsize, int ntry, ADDRIN* addr, socklen_t* addrl);
    size_t SendTo(SOCKET sock, void* buff, ssize_t size, ADDRIN* addr, socklen_t addrl);
    size_t RecvFrom(SOCKET sock, void* buff, ssize_t maxsize, ADDRIN* addr, socklen_t* addrl);
    size_t RecvFromDontWait(SOCKET sock, void* buff, ssize_t maxsize, ADDRIN* addr, socklen_t* addrl);
    size_t TryRecvFrom(SOCKET sock, void* buff, ssize_t maxsize, int ntry, ADDRIN* addr, socklen_t* addrl);
    int SendFileUDP(SOCKET sock, char* path, ssize_t chunksize, ADDRIN* addr, socklen_t addrl);
    int RecvFileUDP(SOCKET sock, char* path, ssize_t chunksize, int ntry, ADDRIN* addr, socklen_t addrl);
//close socket
    int CloseSOCK(SOCKET sock);
    int shutdownWriteSOCK(SOCKET sock);
    int shutdownReadSOCK(SOCKET sock);
    void setMaxTryWaitOnFd(int sec, SOCKET sock);
    void setADDRIN(ADDRIN* addr, int family, int port, char* naddr);
//SERVER SIDE FUNCTIONS
    int Bind(SOCKET sock, ADDRIN* addr); //1
    int Listen(SOCKET sock, unsigned int queuelen); //2
    SOCKET Accept(SOCKET sock, ADDRIN* clientaddr, socklen_t size); //3
//CLIENT SIDE FUNCTIONS
    int Connect(SOCKET sock, ADDRIN* addr);
//UTILS
    int errorprint(const char*, int err);
    void fatalprint(const char*, int err);
    void setDEBUG(int val);
    void debugprint(const char* str);
//easy function
    SOCKET simple_TCPServer(int port, char* addr, int queuelen);
    SOCKET simple_TCPClient(int port, char* addr);
    SOCKET simple_UDPServer(int port);
    SOCKET simple_UDPClient(ADDRIN* saddr, int port, char* addr);
//from stevenson
    void Writen(int fd, void* ptr, size_t nbytes);
****************************************************************/
 
    //COMMON FUNCTIONS
    //create sock
    SOCKET CreateSocketTCP();
    SOCKET CreateSocketUDP();
   
    void setCurrSOCK(SOCKET newsock);
    
    //send & rcv TCP --> refering to GLOBAL CurrSOCK
    int SendNumber(SOCKET sock, int num);
    int RecvNumber(SOCKET sock, int* num);
    int SendString(SOCKET sock, const char* string);
    int RecvString(SOCKET sock, char* string, ssize_t maxsize);
    int TryRecvString(SOCKET sock, char* string, ssize_t maxsize, int ntry);
    size_t Send(SOCKET sock, void* buff, ssize_t size);
    size_t Recv(SOCKET sock,void* buff, ssize_t maxsize);
    size_t RecvDontWait(SOCKET sock,void* buff, ssize_t maxsize);
    size_t TryRecv(SOCKET sock,void* buff, ssize_t maxsize, int ntry);
    int SendFile(SOCKET sock, char* path,ssize_t chunksize);
    int RecvFile(SOCKET sock, char* path, ssize_t chunksize, int ntry);
    
    //send & rcv UDP --> refering to a GLOBAL SOCK
    int SendStringTo(SOCKET sock, const char* string, ADDRIN *addr, socklen_t addrl);
    int RecvStringFrom(SOCKET sock, char* string, ssize_t maxsize, ADDRIN *addr, socklen_t* addrl);
    int TryRecvStringFrom(SOCKET sock, char* string, ssize_t maxsize, int ntry, ADDRIN* addr, socklen_t* addrl);
    size_t SendTo(SOCKET sock, void* buff, ssize_t size, ADDRIN* addr, socklen_t addrl);
    size_t RecvFrom(SOCKET sock, void* buff, ssize_t maxsize, ADDRIN* addr, socklen_t* addrl);
    size_t RecvFromDontWait(SOCKET sock, void* buff, ssize_t maxsize, ADDRIN* addr, socklen_t* addrl);
    size_t TryRecvFrom(SOCKET sock, void* buff, ssize_t maxsize, int ntry, ADDRIN* addr, socklen_t* addrl);
    int SendFileUDP(SOCKET sock, char* path, ssize_t chunksize, ADDRIN* addr, socklen_t addrl);
    int RecvFileUDP(SOCKET sock, char* path, ssize_t chunksize, int ntry, ADDRIN* addr, socklen_t addrl);
    
    //close socket
    int CloseSOCK(SOCKET sock);
    int shutdownWriteSOCK(SOCKET sock);
    int shutdownReadSOCK(SOCKET sock);
    
    void setMaxTryWaitOnFd(int sec, SOCKET sock);
    
    void setADDRIN(ADDRIN* addr, int family, int port, char* naddr);
    
    //SERVER SIDE FUNCTIONS
    int Bind(SOCKET sock, ADDRIN* addr); //1
    int Listen(SOCKET sock, unsigned int queuelen); //2
    SOCKET Accept(SOCKET sock, ADDRIN* clientaddr, socklen_t size); //3

    //CLIENT SIDE FUNCTIONS
    int Connect(SOCKET sock, ADDRIN* addr);
    
    //UTILS
    int errorprint(const char*, int err);
    void fatalprint(const char*, int err);
    void setDEBUG(int val);
    void debugprint(const char* str);

    //easy function
    SOCKET simple_TCPServer(int port, char* addr, int queuelen);
    SOCKET simple_TCPClient(int port, char* addr);
    SOCKET simple_UDPServer(int port);
    SOCKET simple_UDPClient(ADDRIN* saddr, int port, char* addr);

    //from stevenson
    void Writen(int fd, void* ptr, size_t nbytes);
#ifdef	__cplusplus
}
#endif

#endif	/* SOCKLIB_H */

