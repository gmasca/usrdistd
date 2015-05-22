/* 
 * File:   tcpf.c
 * Author: Mascari Giovanni s206587
 *
 * Created on 14 giugno 2014, 21.01
 * for the course of Distributed Programming 2013/2014
 */

#include "socklib.h"

SOCKET CreateSocketTCP() {
    SOCKET res = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (res < 0) {
        fatalprint("socket creation error\n", res);
    }
    debugprint("new SOCKET created");
    return res;
}

int SendNumber(SOCKET sock, int num){
    int net_num = htonl(num);
    int res;
        if(sock==0)
            res = send(currsock, &net_num, sizeof(int32_t), 0);
        else
            res = send(sock, &net_num, sizeof(int32_t), 0);
        
        if (res < 0) {
            errorprint("impossible to send number", errno);
            return errno;
        } else {
            debugprint("string sended");
        }

    return res;
}

int RecvNumber(SOCKET sock, int* num){
    int net_num;
    int res;
    if(sock==0){
          res = read(currsock, &net_num, sizeof(int32_t));
    }else{
          res = read(sock, &net_num, sizeof(int32_t));
    }

    num[0] = ntohl(net_num);

    if (res < 0) {
        errorprint("impossible to send number", errno);
        return errno;
    } else {
        debugprint("string sended");
    }
    return res;
}

int SendString(SOCKET sock,const char* string) {
    int len = strlen(string);
    if (len != 0) {
        int res;
        if(sock==0)
            res = send(currsock, string, len, 0);
        else
            res = send(sock, string, len, 0);
        
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

int RecvString(SOCKET sock, char* string, ssize_t maxsize) {    
    int res;
    char c = ' ';
    int i=0;

    if(sock==0){
        do{
          res = read(currsock, &c, 1);
          if(c=='\0')
            break;
          string[i]=c;
          i++;
          
          if(i>= maxsize){
            res = -1;
            break;
          }

        }while(res > 0);

    }else{
        do{
          res = read(sock, &c, 1);
          if(c=='\0' || c=='\n')
            break;
          string[i]=c;
          i++;
          
          if(i>= maxsize){
            res = -1;
            break;
          }

        }while(res > 0);
  }
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

int TryRecvString(SOCKET sock,char* string, ssize_t maxsize, int ntry) {
    int i=1;
    if(_WAIT == 0)
        return -1;

    while(1){
        setMaxTryWaitOnFd(wtime, sock);
        if(select(FD_SETSIZE, &cset, NULL, NULL, &tv)){
           int res;
        if(sock==0)
            res = recv(currsock, string, maxsize, 0);
        else
            res = recv(sock, string, maxsize, 0);
        
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
    return -1;
}


size_t Send(SOCKET sock,void* buff, ssize_t size) {
    size_t res;
    if(sock == 0)
        res = send(currsock, buff, size, 0);
    else
        res = send(sock, buff, size, 0);

    if (res < 0) {
        errorprint("send error", res);
    } else {
      // debugprint("something sended");
    }
    return res;
}

size_t Recv(SOCKET sock,void* buff, ssize_t maxsize) {
    size_t res;
        if(sock==0)
            res = recv(currsock, buff, maxsize, 0);
        else
            res = recv(sock, buff, maxsize, 0);
    
    if (res < 0) {
        errorprint("impossible recv something", res);
    } else {
        //debugprint("something received");
    }
    return res;
}

size_t RecvDontWait(SOCKET sock,void* buff, ssize_t maxsize){
  size_t res;
    if(sock==0)
        res = recv(currsock, buff, maxsize, MSG_DONTWAIT);
    else
        res = recv(sock, buff, maxsize, MSG_DONTWAIT);

  if(errno != EAGAIN){ //means there are data but cannot read it
    if (res < 0) {
        errorprint("impossible recv something", res);
    } else {
        debugprint("something received");
    }
 }
    return EAGAIN;
}

size_t TryRecv(SOCKET sock, void* buff, ssize_t maxsize, int ntry) {
    int i=1;
    if(_WAIT == 0)
      return -1; //impossible to wait

    while(1) {
     setMaxTryWaitOnFd(tv.tv_sec, sock);
     if(select(FD_SETSIZE, &cset, NULL, NULL, &tv)){
        int res = recv(currsock, buff, maxsize, 0);
        
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


void handle_sigpipe(int sig){
    CloseSOCK(0);
    printf("broken PIPE\n Connection with client closed\n");
}

int SendFile(SOCKET sock, char* path, ssize_t chunksize) {
    FILE* file = fopen(path, "r");
    setCurrSOCK(sock);
    signal(SIGPIPE, handle_sigpipe);
    if (file != NULL) {
        fseek(file, 0L, SEEK_END);
        unsigned long size = ftell(file);
        fseek(file, 0L, SEEK_SET);
        printf("file size: %lu\n", size);

        unsigned long nsize = htonl(size);
   
        Send(sock, &nsize, sizeof(nsize));
        char buf[100];
        RecvString(sock, buf, 100);
    
    printf("start file transfer\n");
    unsigned long comp=0;

        char* chunk = (char*) malloc(chunksize+1);

        int readed=0;
        while (1) {
	         readed = read(file->_fileno, chunk, chunksize);
             comp+= readed;            
             
             if(Send(sock, chunk, readed) < 0)
                break;
			
            if (comp >= size) {
                if(!INTERRUPTED_BY_SIGNAL){
                  break;
               }
			}
        }
        
        //flush socket
   end: lseek(currsock, 0L, SEEK_END);
        free(chunk);
        fclose(file);
        
        return 1;
    }
    //file exist?
    errorprint("file don't exist or cannot be open", errno);
    return -1;
}

int RecvFile(SOCKET sock, char* path, ssize_t chunksize, int ntry) {
    FILE* file = fopen(path, "w");

    if (file != NULL) {
        char* chunk = (char*) malloc(chunksize+1);
        int rec;
        
        unsigned long nsize, size;
        Recv(sock, &nsize, sizeof(nsize));
        size = ntohl(nsize);
        printf("file size: %lu\n", size);

        SendString(sock, "R\r\n");
          
        unsigned long comp=0;
            double val;
        while (1) {
            rec = Recv(sock, chunk, chunksize);
		    write(file->_fileno, chunk, rec);
            
            comp+=rec;

            if(comp>=size){
	              break;
            }

            val = 100.0*(((double)comp)/((double)size));
            printf("%s-->%d%%\r",path,(int)val);
        }
        printf("%s-->file received!\n", path);
        fclose(file);
        free(chunk);
        return 1;
    }
    errorprint("impossible create file", errno);
    return -1;
}

int Listen(SOCKET sock, unsigned int queuelen) {
    int ret=-1;
    if(sock == 0)
         ret = listen(currsock, queuelen);
    else
        ret = listen(sock, queuelen);

    if (ret < 0) {
        errorprint("impossible to start listening on socket", errno);
        return errno;
    }
    debugprint("server start to listen");
    return ret;
}

int Bind(SOCKET sock, ADDRIN* addr) {
    int ret=-1;
    int opt=1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(int));

    if(sock==0)
       ret = bind(currsock, (ADDR*) addr, sizeof (ADDRIN));
    else
       ret = bind(sock, (ADDR*) addr, sizeof (ADDRIN));

    if (ret < 0)
        fatalprint("impossible to bind address", ret);
    debugprint("address correctly binded");
    return ret;
}

SOCKET Accept(SOCKET sock, ADDRIN* cliaddr, socklen_t size) {
    bzero(cliaddr, size);
    debugprint("waiting for new connection");
    SOCKET new;
    
    if (sock==0)
        new = accept(currsock, (ADDR*) cliaddr, &size);
    else
        new = accept(sock, (ADDR*) cliaddr, &size);
    
    if (new < 0) {
        errorprint("impossible to accept new connection", errno);
        return -1;
    }
    return new;
}

int Connect(SOCKET sock, ADDRIN* addr) {
    socklen_t len = sizeof (ADDRIN);
    int ret; 
    if(sock==0)
         ret = connect(currsock, (ADDR*) addr, len);
    else
         ret = connect(sock, (ADDR*) addr, len);

    if (ret < 0) {
        fatalprint("impossible to connect", errno);
    }
    debugprint("connected to server");
    return ret;
}



SOCKET simple_TCPServer(int port, char* addr, int queuelen){
    ADDRIN saddr;
    SOCKET fd = CreateSocketTCP();
    setCurrSOCK(fd);
    setADDRIN(&saddr, AF_INET, port, addr);    
    Bind(fd, &saddr);
    Listen(fd, queuelen);
    return fd;
}

SOCKET simple_TCPClient(int port, char* addr){
    ADDRIN saddr;
    SOCKET fd = CreateSocketTCP();
    setCurrSOCK(fd);
    setADDRIN(&saddr, AF_INET, port, addr);
    Connect(fd, &saddr);
    return fd;
}
