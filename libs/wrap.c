/* 
 * File:   wrap.c
 * Author: Mascari Giovanni s206587
 *
 * Created on 14 giugno 2014, 21.01
 * for the course of Distributed Programming 2013/2014
 */

#include "socklib.h"

void setCurrSOCK(SOCKET newsock) {
    debugprint("current SOCKET changed");
    currsock = newsock;
}

void setADDRIN(ADDRIN* addr, int family, int port, char* naddr){  
  bzero (addr, sizeof (ADDRIN)); //fill to zero
  if(family)
      addr->sin_family = family;
  else
      addr->sin_family = AF_INET;
  
  addr->sin_port = htons (port);
  
  if (naddr!=NULL)
        addr->sin_addr.s_addr = inet_addr (naddr);
  else      
        addr->sin_addr.s_addr = htonl (INADDR_ANY);
  
  debugprint ("sockaddr filled");
}

int CloseSOCK(SOCKET sock) {
    int res;
    if(sock == 0)
      sock = currsock;
    
    if ((res = close(sock)) == 0)
        debugprint("currSock Closed");
    else
        errorprint("impossible to close currSock", res);
    
    return res;
}

int shutdownWriteSOCK(SOCKET sock) {
    int res;
    if(sock==0){
      sock = currsock;
    }
    if ((res = shutdown(sock, SHUT_WR)) < 0)
        debugprint("shutdown write on currSock");
    else
        errorprint("impossible to shutdown write on currSock", res);
    return res;
}

int shutdownReadSOCK(SOCKET sock) {
    int res;
    if(sock==0){
      sock = currsock;
    }
    if ((res = shutdown(currsock, SHUT_RD)) < 0)
        debugprint("shutdown read on currSock");
    else
        errorprint("impossible to shutdown read on currSock", res);
    return res;
}

int errorprint(const char* str, int err) {
    printf("!-%s :%d\n", str, err);
    return (err);
}

void fatalprint(const char* str, int err) {
    printf("!!-%s :%d\n", str, err);
    exit(err);
}

void setDEBUG(int val) {
    if (val > 0)
        _DEBUG = 1;
    else
        _DEBUG = 0;
}

void debugprint(const char* str) {
    if (_DEBUG)
        printf("%s\n", str);
}

//STEVENS VERY UTILS FUNCTIONS!!!

/* Write "n" bytes to a descriptor. */
ssize_t
writen(int fd, const void *vptr, size_t n)
{
  size_t        nleft;
  ssize_t       nwritten;
  const char    *ptr;

  ptr = vptr;
  nleft = n;
  while (nleft > 0) {
    if ( (nwritten = write(fd, ptr, nleft)) <= 0) {
      if (nwritten < 0 && errno == EINTR)
    nwritten = 0;       /* and call write() again */
      else
    return(-1);         /* error */
    }

    nleft -= nwritten;
    ptr   += nwritten;
  }
  return(n);
}
/* end writen */

void
Writen(int fd, void *ptr, size_t nbytes)
{
  if (writen(fd, ptr, nbytes) != nbytes)
    errorprint("writen error", errno);
}