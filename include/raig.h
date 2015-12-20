#ifndef _RAIG_RAIG_H
#define _RAIG_RAIG_H

#include <memory>
#include <iostream>

#define LINESIZE 80
#define HANGMAN_TCP_PORT 1071

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
	int ReceiveMessage(int iListenSocketFileDescriptor, char* buffer, int bufferSize, int flags, struct sockaddr *sender, socklen_t *sendsize);

	// Set the server to listen mode for incoming TCP client connections
	// This wrapper function calls Listen() in libsocket
	void ListenForConnections(int socketFileDescriptor, int maxListenQSize);

	// Set up signal handler when forking processes on the server
	// Fork() will signal the parent process when it has been terminated.
	// Signal() in libsocket will create a signal handler for
	// catching terminated processes and releasing resources
	// used by them. This will prevent zombie processes.
	void CreateSignalHandler();

	// Accept all incoming TCP connections and return a file descriptor
	// used to communicate with the client.
	int AcceptConnection(int iListenSocketFileDescriptor, struct Address *address);

	void connect(char* ipAddress);
	void sendData(char* dataString);
	void sendData(int value);
	void sendData(struct Packet* packet);
	Packet* readData();
	void update();
private:
	class RaigImpl; // Forward declaration
	std::unique_ptr<RaigImpl> m_Impl; // Raig implementation using auto_ptr
};

} // namespace raig
#endif
