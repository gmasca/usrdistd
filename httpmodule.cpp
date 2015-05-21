#include "httpmodule.hpp"

bool Httpmodule::startHttpServer(int port){
	sock = simple_TCPServer(port, NULL, 10);
	if(sock>0)
		return true;
	else
		return false;
}

SOCKET Httpmodule::waitConnection(){
	return Accept(sock, &nextcli, sizeof(nextcli));
}

void Httpmodule::waitRequest(SOCKET conn){
	char buf[MAXSTR];
	
	while(1){
		if(RecvString(conn, buf,MAXSTR)<=0){
			CloseSOCK(conn);
			return;
		}
		printf("%s", buf);
		
		if(buf[0]=='\r' || buf[0]=='\n' || buf[0]=='\0') break;
		
		bzero(buf, MAXSTR);
	}
}

void Httpmodule::sendResponse(long dist, SOCKET conn){
	char message[MAXSTR];
	sprintf(message, "%ld", dist);
	strAddHead(message, XML_TAG_O, MAXSTR);
	strAddTail(message, XML_TAG_C, MAXSTR);

	char lenghtheader[100];
	sprintf(lenghtheader, "Content-Length: %d\n", strlen(message));

    SendString(conn, "HTTP/1.1 200 OK\n");
	SendString(conn, "Server: usrdistd\n");
	SendString(conn,lenghtheader);
	SendString(conn, "Cache-Control: no-cache\n");
	SendString(conn, "Pragma: no-cache\n");
	SendString(conn, "Connection: keep-alive\n");
	SendString(conn, "Content-Type:application/xml; charset=utf-8\n");
	SendString(conn, "Access-Control-Allow-Credentials: true\n"); //for CORS Access-Control
	SendString(conn, "Access-Control-Allow-Origin: *\n");
    SendString(conn, "Set-Cookie: pageAccess=3;");
    SendString(conn, "Connection: close\n");
    SendString(conn,"\n"); //blank string
	SendString(conn, message);

	bzero(lenghtheader, 100);
	bzero(message, MAXSTR);
	CloseSOCK(conn);
}

void Httpmodule::close(){
	while(1)
	CloseSOCK(sock);
}