/* 
 * File:   utils.h
 * Author: Mascari Giovanni s206587
 *
 * Created on 15 giugno 2014, 18.00
 * for the course of Distributed Programming I 2013/2014
 */

#ifndef UTILS_H
#define	UTILS_H

#ifdef	__cplusplus
extern "C" {
#endif
#include "utils.h"
#include "socklib.h"
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <regex.h>
#include <rpc/xdr.h>
#include "sys/types.h"

#define SEARCH_AT_START 1
#define SEARCH_AT_END 2
#define SEARCH_GLOBAL 3

/******** LIST OF FUNCTIONS ********
    strClean();
    strAddHead();
    strRemHead();
    strAddTail();
    strRemTail();
    strAddTailCRLF();
    searchPatt();
    checkFileExistence();
    checkArgs();
    childHandling();
    xdr_FileOpen();
***********************************/

    /** strClean --- add a /0 char to a certain string
     *
     * @param str pointer to the string
     * @param buffsize string maximum size 
     */
    int strClean(char* str, ssize_t buffsize);
    
    /** strAddHead --- add a pattern to the head of a string
     *
     * @param str pointer to the base string
     * @param head the path to add
     * @param buffsize the string maximum size 
     */
    int strAddHead(char* str, const char* head, ssize_t buffsize);

    /** strRemHead --- remove (if exist) a certain pattern from the head of a string
     *
     * @param str pointer to the string
     * @param head string pattern to remove 
     */
    int strRemHead(char* str, const char* head);
    
    /** strAddTail --- add String at the end of a string
     *
     * @param str pointer to the string
     * @param tail string to add
     * @param buffsize the string maximum size 
     */
    int strAddTail(char* str, const char* tail, ssize_t buffsize);
    
    /** strRemTail --- remove (if exist) a certain pattern from the tail of the string
     *
     * @param str pointer to the string
     * @param buffsize string size 
     */
    int strRemTail(char* str, const char* tail);

    /** strAddTailCFLF --- add pattern the end of the string with CR LF
     *
     * @param str pointer to the string
     * @param tail string to add
     * @param buffsize the string maximum size 
     */
    int strAddTailCRLF(char* str, const char* tail, ssize_t buffsize);
    
    /** searchPatt --- searcg a pattern in a string (use regexp)
     *
     * @param str pointer to the string
     * @param patt  pattern to search
     * @param special position of the pattern you are loking for 
     */
    int searchPatt(char* str, const char* patt, int special);

    /** checkFileExistance --- check if a certain file exist
     *
     * @param path position of the file (e.g. /home/user/file.txt)
     */
    int checkFileExistence(const char* path);

    /** checkArgs --- check the numbers of arguments
     *
     * @param argc (the argc variable from main)
     * @param n the number of expected arguments
     */
    void checkArgs(int argc, int n);

    /** childHandling --- avoid zombie
     *
     *  call it when a new child routine start
     */
    void childHandling();

    /** xdr_FileOpen --- wrap for using xdrstdio_create
     *
     * @param xdr xdr handle (could be r or w)
     * @param sock current used socket
     * @param mode could be "w" or "r" depending of xdr
     * @param type 
     */
    FILE* xdr_FileOpen(XDR* xdr, SOCKET sock, const char* mode, int type);

    int readline(FILE* file, char* buff);

#ifdef	__cplusplus
}
#endif

#endif	/* UTILS_H */

