#include "libs/socklib.h"
#include "libs/utils.h"

#define MAXSTR 600
#define CHUNKSIZE 512

#define XML_TAG_O "<distance>"
#define XML_TAG_C "</distance>"
#define DEFAULT_PORT 9600

class Httpmodule{
	private:
		SOCKET sock;
		ADDRIN nextcli;

	public:
		bool startHttpServer(int port);
		SOCKET waitConnection();
		void waitRequest(SOCKET conn);
		void sendResponse(long dist, SOCKET conn);
		void close();
};