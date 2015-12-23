#ifndef _RAIG_RAIG_H
#define _RAIG_RAIG_H

#include <memory>
#include <iostream>

extern "C" {
	#include "../external/libsocket/include/socket.h"
}

namespace raig{

struct Packet
{
	char stringData[255];
	int x;
	int y;
	int completeFlag;
};

class Raig
{
public:
	Raig();
	~Raig();
	// Libsocket wrapper functions to abstract the library and reduce coupling
	// Wrapper for libsocket Connection() that creates a peer connection based on the
	int InitConnection(char *hostname, char *service, int type /* Client or Server */, int protocol /* UDP or TCP */);

	// Receive messages from the server using libsocket TODO: create wrapper in libsocket for revfrom()
	int SendMessage(int socketFileDescriptor, char* buffer, size_t size, int flags);

	// Receive data from the connected server using recvfrom()
	int ReceiveMessage(int socketFileDescriptor, char* buffer, int bufferSize, int flags, struct sockaddr *sender, socklen_t *sendsize);

	void sendData(struct Packet* packet);

	void readData(struct Packet* packet);

	void update();

private:
	class RaigImpl; // Forward declaration
	std::unique_ptr<RaigImpl> m_Impl; // Raig implementation using auto_ptr
};

} // namespace raig
#endif
