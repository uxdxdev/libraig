#include "../include/raig.h" // API for developers

extern "C" {
	#include "../external/libsocket/include/socket.h"
}

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
	void sendData(Packet* packet);
	Packet* readData();
	void update();

	// Private members and functions
	void cleanUp();

	bool isComplete;
	bool alive;

	int iSocketFileDescriptor;
	char* strServerIPAddress;
	struct Address sAddress;
	Packet readPacket;
};

/*
 *  Raig class implementation
 */
Raig::Raig() : m_Impl(new RaigImpl())
{
	m_Impl->isComplete = false;
	m_Impl->alive = true;
}

Raig::~Raig()
{
	m_Impl->cleanUp();
}

// Libsocket wrapper functions to abstract the library and reduce coupling
// Wrapper for libsocket Connection() that creates a peer connection based on the
int Raig::InitConnection(char *hostname, char *service, int type /* Client or Server */, int protocol /* UDP or TCP */);

// Receive messages from the server using libsocket TODO: create wrapper in libsocket for revfrom()
int Raig::SendMessage(int socketFileDescriptor, char* buffer, size_t size, int flags);

// Receive data from the connected server using recvfrom()
int Raig::ReceiveMessage(int iListenSocketFileDescriptor, char* buffer, int bufferSize, int flags, struct sockaddr *sender, socklen_t *sendsize);

// Set the server to listen mode for incoming TCP client connections
// This wrapper function calls Listen() in libsocket
void Raig::ListenForConnections(int socketFileDescriptor, int maxListenQSize);

// Set up signal handler when forking processes on the server
// Fork() will signal the parent process when it has been terminated.
// Signal() in libsocket will create a signal handler for
// catching terminated processes and releasing resources
// used by them. This will prevent zombie processes.
void Raig::CreateSignalHandler();

// Accept all incoming TCP connections and return a file descriptor
// used to communicate with the client.
int Raig::AcceptConnection(int iListenSocketFileDescriptor, struct Address *address);

void Raig::connect(char* ipAddress)
{
	m_Impl->connect(ipAddress);
}

void Raig::sendData(char* dataString)
{
	m_Impl->sendData(dataString);
}

void Raig::sendData(int value)
{
	m_Impl->sendData(value);
}

void Raig::sendData(Packet* packet)
{
	m_Impl->sendData(packet);
}

Packet* Raig::readData()
{
	return (Packet*) m_Impl->readData();
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
	iSocketFileDescriptor = Connection(hostname, service, type, protocol);
	return iSocketFileDescriptor;
}

// Receive messages from the server using libsocket TODO: create wrapper in libsocket for revfrom()
int Raig::RaigImpl::SendMessage(int socketFileDescriptor, char* buffer, size_t size, int flags)
{
	return Send(socketFileDescriptor, buffer, size, flags);
}

// Receive data from the connected server using recvfrom()
int Raig::RaigImpl::ReceiveMessage(int iListenSocketFileDescriptor, char* buffer, int bufferSize, int flags, struct sockaddr *sender, socklen_t *sendsize)
{
	return ReceiveFrom(iListenSocketFileDescriptor, buffer, bufferSize, flags, sender, sendsize);
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
	Signal(SIGCHLD, SignalHandler);
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

void Raig::RaigImpl::connect(char* ipAddress)
{
	std::cout << "Raig::RaigImpl::connect()" << std::endl;

	strServerIPAddress = ipAddress;

	iSocketFileDescriptor = Socket(AF_INET, SOCK_STREAM, 0);

	Address(AF_INET, (struct Address*) &sAddress, strServerIPAddress, HANGMAN_TCP_PORT);

	Connect(iSocketFileDescriptor, (struct sockaddr*) &sAddress.m_sAddress, sizeof(sAddress.m_sAddress));

}

void Raig::RaigImpl::sendData(char* dataString)
{
	std::cout << "Raig::RaigImpl::sendData() : " << dataString << std::endl;
	//multiplexStdinFileDescriptor(stdin, iSocketFileDescriptor);

	//close(iSocketFileDescriptor);
}

void Raig::RaigImpl::sendData(int value)
{
	std::cout << "Raig::RaigImpl::sendData() : " << value << std::endl;
}

void Raig::RaigImpl::sendData(Packet* packet)
{
	Write(iSocketFileDescriptor, packet, sizeof(Packet));
}

Packet* Raig::RaigImpl::readData()
{
	Read(iSocketFileDescriptor, &readPacket, sizeof(Packet));
	return &readPacket;
}

void Raig::RaigImpl::update()
{
	std::cout << "Raig::RaigImpl::update()" << std::endl;
}

void Raig::RaigImpl::cleanUp()
{
	std::cout << "Raig::RaigImpl::cleanUp()" << std::endl;
	close(iSocketFileDescriptor);
}
