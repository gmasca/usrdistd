/* 
 * File:   utils.c
 * Author: Mascari Giovanni s206587
 *
 * Created on 15 giugno 2014, 18.00
 * for the course of Distributed Programming 2013/2014
 */
#include "utils.h"
#include <rpc/xdr.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <regex.h>


int strClean(char* str, ssize_t buffsize){
  int i=0;
  while(1){
    if(str[i]=='\0'){
      return 0;
    }
    if(str[i]=='\n' || str[i]=='\r'){
      str[i]='\0';
      break;
    }
    i++;
    if(i > buffsize){
      return -1;
    }
  }
return 0;
}

int strAddHead(char* str, const char* head, ssize_t buffsize){
  int totsize = strlen (str) + strlen (head) + 1;
  if (totsize > buffsize)
      return -1;
  
  char* temp = (char*) malloc (totsize);
  bzero (temp, totsize);
  strncat (temp, head, strlen (head));
  strncat (temp, str, strlen (str));
  bzero (str, buffsize);
  strncpy (str, temp, strlen (temp) + 1);
  free (temp);
  return 0;
}

int strRemHead(char* str, const char* head){  
  if (strlen (str) < strlen (head))
     return -1;
  if(!searchPatt(str, head, SEARCH_AT_START)){
        int rbyte = strlen (head);
        int cbyte = strlen (str) - rbyte;
        char* temp = (char*) malloc (cbyte + 1);
        strncpy (temp, &str[rbyte], cbyte + 1);
        bzero (str, strlen (str) + 1);
        strncat (str, temp, strlen (temp)); //to add also a \0 to the end
        free (temp);
        return 0;
  }
  return -1;
}

int strAddTail(char* str, const char* tail, ssize_t buffsize){
  int totsize = strlen (str) + strlen (tail) + 1;
  if (totsize > buffsize)
      return -1;  
  strncat (str, tail, strlen (tail));
  str[totsize] = '\0';
  return 0;
}

int strAddTailCRLF(char* str, const char* tail, ssize_t buffsize){
  strRemTail(str, "\n");
  strRemTail(str, "\r");
  int len = strlen(str);
  int totsize = len + strlen (tail) + 3;
  if (totsize > buffsize)
      return -1;  
  strncat (str, tail, strlen (tail));
  strncat (str, "\r\n", 2);
  str[totsize] = '\0';
  return 0;
}

int strRemTail(char* str, const char* tail){
   if (strlen (str) < strlen (tail))
      return -1;
   
   if(!searchPatt(str, tail, SEARCH_AT_END)){
        int rmbyte = strlen(str) - strlen (tail);
        str[rmbyte] ='\0';
        return 0;
   }
return -1; 
}

int searchPatt(char* str, const char* patt, int special){
  regex_t regex;
  int ret;
  char* modpatt;
  
  switch (special){
      case SEARCH_AT_START:
          modpatt = (char*)malloc(strlen(patt)+2);
          strcpy(modpatt, "^");
          strcat(modpatt, patt);
          regcomp (&regex, modpatt, REG_EXTENDED);
          ret = regexec(&regex, str, 0, NULL, 0);
          free(modpatt);
          break;
          
      case SEARCH_AT_END:
          modpatt = (char*)malloc(strlen(patt)+2);
          strcpy(modpatt, patt);
          strcat(modpatt, "$");
          regcomp (&regex, modpatt, REG_EXTENDED);
          ret = regexec(&regex, str, 0, NULL, 0);
          free(modpatt);
          break;
          
      default:
          regcomp (&regex, patt, REG_EXTENDED);
          ret = regexec(&regex, str, 0, NULL, 0);
          break;
  }
  regfree (&regex);
  return ret;
}

int checkFileExistence(const char* path){
    FILE* file = fopen(path, "r");
    
    if(file == NULL){
        printf("file not exist\n");
        return -1;
    }else{
		fclose(file);
        return 0;
    }
    printf("impossible to read the requested file\n");
    return -2;//not possible to read file
}

void checkArgs(int argc, int n){
  if (argc < (n+1)){
    printf("not enough arguments!!\n");
     exit(-1);
  }
}

void handle_sigchld(int sig) {
  while (waitpid((pid_t)(-1), 0, WNOHANG) > 0) {}
}

void childHandling(){
  struct sigaction sa;
  sa.sa_handler = &handle_sigchld;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
  if (sigaction(SIGCHLD, &sa, 0) == -1) {
    perror(0);
    exit(1);
  }
}

FILE* xdr_FileOpen(XDR* xdr, SOCKET sock, const char* mode, int type){

  FILE* sock_f = fdopen(sock, mode);
  if(sock_f==NULL){
    printf("impossible to use fd %d\n", errno);
    return NULL;
  }
  xdrstdio_create(xdr, sock_f, type);
return sock_f;
}

int readline(FILE* file, char* buff){
      int ret=0;
      
      if(fscanf(file, "%s\n", buff)==EOF)
         ret = -1;
      else
        strClean(buff, strlen(buff));
      
      return ret;
}
