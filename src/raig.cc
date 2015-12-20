#include "../include/raig.h" // API for developers

using namespace raig;

// RaigImpl class declaration
class Raig::RaigImpl
{
public:
	// API
	// Libsocket wrapper functions to abstract the library and reduce coupling
	// Wrapper for libsocket Connection() that creates a peer connection based on the
	int InitConnection(char *hostname, char *service, int type /* Client or Server */, int protocol /* UDP or TCP */);

	// Receive messages from the server using libsocket TODO: create wrapper in libsocket for revfrom()
	int SendMessage(int socketFileDescriptor, char* buffer, size_t size, int flags);

	// Receive data from the connected server using recvfrom()
	int ReceiveMessage(int socketFileDescriptor, char* buffer, int bufferSize, int flags, struct sockaddr *sender, socklen_t *sendsize);

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
	int AcceptConnection(int listenSocketFileDescriptor, struct Address *address);

	void sendData(struct Packet* packet);

	void readData(struct Packet* packet);

	void update();

	// Private members and functions
	void cleanUp();

	int m_iSocketFileDescriptor;
	char* strServerIPAddress;
	struct Address sAddress;
	Packet readPacket;
};

/*
 *  Raig class implementation
 */
Raig::Raig() : m_Impl(new RaigImpl())
{
}

Raig::~Raig()
{
	m_Impl->cleanUp();
}

// Libsocket wrapper functions to abstract the library and reduce coupling
// Wrapper for libsocket Connection() that creates a peer connection based on the
int Raig::InitConnection(char *hostname, char *service, int type /* Client or Server */, int protocol /* UDP or TCP */)
{
	return m_Impl->InitConnection(hostname, service, type, protocol);
}

// Receive messages from the server using libsocket TODO: create wrapper in libsocket for revfrom()
int Raig::SendMessage(int socketFileDescriptor, char* buffer, size_t size, int flags)
{
	m_Impl->SendMessage(socketFileDescriptor, buffer, size, flags);
}

// Receive data from the connected server using recvfrom()
int Raig::ReceiveMessage(int socketFileDescriptor, char* buffer, int bufferSize, int flags, struct sockaddr *sender, socklen_t *sendsize)
{
	m_Impl->ReceiveMessage(socketFileDescriptor, buffer, bufferSize, flags, sender, sendsize);
}

// Set the server to listen mode for incoming TCP client connections
// This wrapper function calls Listen() in libsocket
void Raig::ListenForConnections(int listenSocketFileDescriptor, int maxListenQSize)
{
	m_Impl->ListenForConnections(listenSocketFileDescriptor, maxListenQSize);
}

// Set up signal handler when forking processes on the server
// Fork() will signal the parent process when it has been terminated.
// Signal() in libsocket will create a signal handler for
// catching terminated processes and releasing resources
// used by them. This will prevent zombie processes.
void Raig::CreateSignalHandler()
{
	m_Impl->CreateSignalHandler();
}

// Accept all incoming TCP connections and return a file descriptor
// used to communicate with the client.
int Raig::AcceptConnection(int listenSocketFileDescriptor, struct Address *address)
{
	return m_Impl->AcceptConnection(listenSocketFileDescriptor, address);
}

void Raig::sendData(struct Packet* packet)
{
	m_Impl->sendData(packet);
}

void Raig::readData(struct Packet* packet)
{
	m_Impl->readData(packet);
}

void Raig::update()
{
	m_Impl->update();
}




/*
 * RaigImpl implementation
 */

// Libsocket wrapper functions to abstract the library and reduce coupling
// Wrapper for libsocket Connection() that creates a peer connection based on the
int Raig::RaigImpl::InitConnection(char *hostname, char *service, int type /* Client or Server */, int protocol /* UDP or TCP */)
{
	m_iSocketFileDescriptor = Connection(hostname, service, type, protocol);
	return m_iSocketFileDescriptor;
}

// Receive messages from the server using libsocket TODO: create wrapper in libsocket for revfrom()
int Raig::RaigImpl::SendMessage(int socketFileDescriptor, char* buffer, size_t size, int flags)
{
	return Send(socketFileDescriptor, buffer, size, flags);
}

// Receive data from the connected server using recvfrom()
int Raig::RaigImpl::ReceiveMessage(int listenSocketFileDescriptor, char* buffer, int bufferSize, int flags, struct sockaddr *sender, socklen_t *sendsize)
{
	return ReceiveFrom(listenSocketFileDescriptor, buffer, bufferSize, flags, sender, sendsize);
}

// Set the server to listen mode for incoming TCP client connections
// This wrapper function calls Listen() in libsocket
void Raig::RaigImpl::ListenForConnections(int socketFileDescriptor, int maxListenQSize)
{
	Listen(socketFileDescriptor, maxListenQSize);
}

// Set up signal handler when forking processes on the server
// Fork() will signal the parent process when it has been terminated.
// Signal() in libsocket will create a signal handler for
// catching terminated processes and releasing resources
// used by them. This will prevent zombie processes.
void Raig::RaigImpl::CreateSignalHandler()
{
	// Signal() in libsocket will create a signal handler for
	// catching terminated processes and releasing resources
	// used by them. This will prevent zombie processes.
	Signal(SIGCHLD, (void*)SignalHandler);
}

// Accept all incoming TCP connections and return a file descriptor
// used to communicate with the client.
int Raig::RaigImpl::AcceptConnection(int iListenSocketFileDescriptor, struct Address *address)
{
	int connfd;
	socklen_t client_len = sizeof(address->m_sAddress);

	// Accept connections from clients
	connfd = accept(iListenSocketFileDescriptor, (struct sockaddr *) &address->m_sAddress, &client_len);

	if (connfd < 0)
	{
		// There was an error (interrupt)
		if( errno == EINTR )
		{
			// Try another Accept() in the event of a system interrupt
			//continue;
			perror("AcceptConnections() system interrupt");
			exit(1); // Exit failaure
		}
		else
		{
			// There was an error other than an interrupt so close the Parent process
			perror("Accept error");
			exit(3);
		}
	}
	return connfd;
}

void Raig::RaigImpl::sendData(struct Packet* packet)
{
	printf("sendData()\n");
	Write(m_iSocketFileDescriptor, packet, sizeof(Packet));
	printf("sendData() OK\n");
}

void Raig::RaigImpl::readData(struct Packet *packet)
{
	Read(m_iSocketFileDescriptor, packet, sizeof(Packet));
}

void Raig::RaigImpl::update()
{
	std::cout << "Raig::RaigImpl::update()" << std::endl;
}

void Raig::RaigImpl::cleanUp()
{
	std::cout << "Raig::RaigImpl::cleanUp()" << std::endl;
	close(m_iSocketFileDescriptor);
}
