#include "socklib.h"

SOCKET CreateSocketUDP(){
    SOCKET res = socket(AF_INET, SOCK_DGRAM, 0);
     if (res < 0) {
        fatalprint("socket creation error\n", res);
    }
    debugprint("new SOCKET created");
    return res;
}

int SendStringTo(SOCKET sock, const char* string, ADDRIN *addr, socklen_t addrl) {
    int len = strlen(string);
    int res;
    if (len != 0) {
        if(sock==0)
            res = sendto(currsock, string, len, 0, (ADDR*)addr, addrl);
        else
            res = sendto(sock, string, len, 0, (ADDR*)addr, addrl);

        if (res < 0) {
            errorprint("impossible to send string", res);
        } else {
            debugprint("string sended");
        }
        return res;
    }
    //void string
    return 0;
}

int RecvStringFrom(SOCKET sock, char* string, ssize_t maxsize, ADDRIN *addr, socklen_t* addrl) {
    int res;
    char c;
    if(sock==0)
      sock = currsock;

    res = recvfrom(sock, string, maxsize, MSG_PEEK, (ADDR*)addr, addrl);

    int i=0;
    while(1){
        c = string[i];
        i++;
        if(c == '\n')
            break;
    }

     read(sock, NULL, i);

    if (res < 0) {
        errorprint("impossible to recv string", res);
    } else {
        if (strlen(string) >= maxsize) {
            string[res] = '\0';
        }
        debugprint("string received");
    }
    return res;
}

void setMaxWait(int t){
    if(t==0)
       _WAIT = 0;
    else
        _WAIT = 1;
    wtime = t;
}

void setMaxTryWaitOnFd(int sec, SOCKET sock) {
    FD_ZERO(&cset);
    tv.tv_sec = sec;
    tv.tv_usec = 0;
    if(sock == 0)
      FD_SET(currsock, &cset);
    else
      FD_SET(sock, &cset);
}

int TryRecvStringFrom(SOCKET sock, char* string, ssize_t maxsize, int ntry, ADDRIN* addr, socklen_t* addrl) {
    int i=1;
    if(_WAIT == 0)
      return -1; //impossible to wait

    while(1) {
       setMaxTryWaitOnFd(wtime, sock);
        if(select(FD_SETSIZE, &cset, NULL, NULL, &tv)){
           int res;
           if(sock==0) 
             res = recvfrom(currsock, string, maxsize, 0, (ADDR*)addr, addrl);   
           else
             res = recvfrom(sock, string, maxsize, 0, (ADDR*)addr, addrl);
                printf("%d", errno);
           if (res < 0) {
               errorprint("impossible to recv string", res);
           } else {
            if (strlen(string) >= maxsize) {
               string[res] = '\0';
           }
         debugprint("string received");
         return res;      
        }
     }else{
            i++;
            errorprint("time elapsed", -1);
            if(i>ntry)
                break;
     }
    }
    return -3;
}

size_t SendTo(SOCKET sock, void* buff, ssize_t size, ADDRIN* addr, socklen_t addrl) {
    size_t res;
    if(sock==0)
        res = sendto(currsock, buff, size, 0, (ADDR*)addr, addrl);
    else
        res = sendto(sock, buff, size, 0, (ADDR*)addr, addrl);

    if (res < 0) {
        errorprint("send error", res);
    } else {
       // debugprint("something sended");
    }
    return res;
}

size_t RecvFrom(SOCKET sock, void* buff, ssize_t maxsize, ADDRIN* addr, socklen_t* addrl) {
    size_t res;

    if(sock==0)
        res =  recvfrom(currsock, buff, maxsize, 0, (ADDR*)addr, addrl);
    else
        res = recvfrom(sock, buff, maxsize, 0, (ADDR*)addr, addrl);
    
//    printf("received\n");

    if (res < 0) {
        errorprint("impossible recv something", res);
    } else {
      //  debugprint("something received");
    }
    return res;
}

size_t RecvFromDontWait(SOCKET sock, void* buff, ssize_t maxsize, ADDRIN* addr, socklen_t* addrl){
  size_t res;
  if(sock ==0)
      res = recvfrom(currsock, buff, maxsize, MSG_DONTWAIT, (ADDR*)addr, addrl);
   else
      res = recvfrom(sock, buff, maxsize, MSG_DONTWAIT, (ADDR*)addr, addrl);

 if(errno != EAGAIN){ //means there are data but cannot read it
    if (res < 0) {
        errorprint("impossible recv something", res);
    } else {
       // debugprint("something received");
    }
 }
    return EAGAIN;
}	


size_t TryRecvFrom(SOCKET sock, void* buff, ssize_t maxsize, int ntry, ADDRIN* addr, socklen_t* addrl) {
    int i=1;
    if(_WAIT == 0)
      return -1; //impossible to wait

    while(1) {
     setMaxTryWaitOnFd(tv.tv_sec, sock);
     if(select(FD_SETSIZE, &cset, NULL, NULL, &tv)){
        int res;
        if(sock==0)
          res = recvfrom(currsock, buff, maxsize, 0, (ADDR*)addr, addrl);
        else
          res = recvfrom(currsock, buff, maxsize, 0, (ADDR*)addr, addrl);

        if (res < 0) {
            errorprint("impossible recv something", res);
            return res;
        } else {
            debugprint("something received");
        }
        return res;
    } else {
        i++;
        errorprint("no message received during wait", -2);
        if(i > ntry)
            break;
    }
   }
  return -1;
}

int SendFileUDP(SOCKET sock, char* path, ssize_t chunksize, ADDRIN* addr, socklen_t addrl){
   if(sock==0)
    sock = currsock;

    FILE* file = fopen(path, "r");

    fseek(file, 0L, SEEK_END);
    unsigned long size = ftell(file);
    fseek(file, 0L, SEEK_SET);
    printf("file size: %lu\n", size);

    unsigned long nsize = htonl(size);
   
    SendTo(sock, &nsize, sizeof(nsize), addr, addrl);
    char buf[100];
    RecvStringFrom(sock, buf, 100, addr, &addrl);
    printf("start file transfer\n");
    unsigned long comp=0;

    if (file != NULL) {
        char* chunk = (char*) malloc(chunksize+1);

        int readed=0;
        while (1) {

	         readed = read(file->_fileno, chunk, chunksize);
            
             if(SendTo(sock, chunk, readed, addr, addrl) < 0){
               printf("error sending\n");
                break;
			         }

            comp+=readed;

            if (comp >= size) {
                  printf("file sended!\n");
                  break;
          }
        }
     free(chunk);
     fclose(file);
        
      return 1;
    }
    //file exist?
    errorprint("file don't exist or cannot be open", errno);
    return -1;
}

int RecvFileUDP(SOCKET sock, char* path, ssize_t chunksize, int ntry, ADDRIN* addr, socklen_t addrl){
    if(sock==0)
      sock = currsock;

    unsigned long nsize, size;
    RecvFromDontWait(sock, &nsize, sizeof(nsize), addr, &addrl);
    size = ntohl(nsize);
    printf("file size: %lu\n", size);

    SendStringTo(sock, "R\r\n", addr, addrl);
    
    FILE* file = fopen(path, "w");
    if (file != NULL) {
        char* chunk = (char*) malloc(chunksize+1);
        int rec;

      unsigned long comp=0;

      while (1) {
  	       rec = read(sock, chunk, chunksize);
	         write(file->_fileno, chunk, rec);
           comp+=rec; 
            printf("%d%%\r", (int)((((double)comp)/((double)size))*100));

            if(comp >= size){
              break;
			    }
        }

        debugprint("file received!");
        fclose(file);
        free(chunk);
        return 1;
    }
    errorprint("impossible create file", errno);
    return -1;
}

SOCKET simple_UDPServer(int port){
    ADDRIN saddr;
    SOCKET fd = CreateSocketUDP();
    setCurrSOCK(fd);
    setADDRIN(&saddr, AF_INET, port, INADDR_ANY);
    Bind(fd, &saddr);
    return fd;
}

//use saddr fpr 
SOCKET simple_UDPClient(ADDRIN* saddr, int port, char* addr){
    SOCKET fd = CreateSocketUDP();
    setCurrSOCK(fd);
    setADDRIN(saddr, AF_INET, port, addr);
    return fd;
}