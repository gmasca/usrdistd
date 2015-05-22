#include "libs/socklib.h"
#include "libs/utils.h"

#define MAXSTR 600
#define CHUNKSIZE 512

#define XML_MSG "\
<Resp token='%d'>\
<distance>%ld\
</distance>\
</Resp>"

#define DEFAULT_PORT 9600

class Httpmodule{
	private:
		SOCKET sock;
		ADDRIN nextcli;

	public:
		bool startHttpServer(int port);
		SOCKET waitConnection();
		int waitRequest(SOCKET conn);
		void sendResponse(int token, long dist, SOCKET conn);
		int stripMSG(char* msg);
		void close();
};