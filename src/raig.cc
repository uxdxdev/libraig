#include <string>

#include "../include/raig.h" // API for developers

extern "C" {
	#include "../external/libsocket/include/socket.h"
}

using namespace raig;

enum State{
	IDLE,
	PROCESSING
};

// RaigImpl class declaration
class Raig::RaigImpl
{
public:

	RaigImpl();
	~RaigImpl();

	int InitConnection(char *hostname, char *service);

	void CreateGameWorld(int size);

	// Find a path using A* from source to destination
	void findPath(int sourceX, int sourceY, int destinationX, int destinationY);

	// Read the path data received by the server
	std::vector<Location> getPath();

	bool IsPathfindingComplete();

	// send buffer to the server
	int sendBuffer();

	// read data from the network into the buffer
	int readBuffer();

	void ClearBuffer();

	// Send packet data
	void sendData(struct Packet* packet);

	// Read packet data
	void readData(struct Packet* packet);

	// Update the raig engine
	void update();

	// Private members and functions
	void cleanUp();

	// Connection socket descriptor
	int m_iSocketFileDescriptor;

	// Network buffer
	char m_cBuffer[MAX_BUF_SIZE];

	// vector of locations
	std::vector<Location> m_vPath;
	std::vector<Location> m_vCompletePath;

	int m_iSentSequence;

	bool m_bIsPathfindingComplete;

	State m_eState;

};

/*
 *  Raig class implementation
 */
Raig::Raig() : m_Impl(new RaigImpl())
{
}

Raig::~Raig()
{
}

int Raig::InitConnection(char *hostname, char *service)
{
	return m_Impl->InitConnection(hostname, service);
}

void Raig::CreateGameWorld(int size)
{
	m_Impl->CreateGameWorld(size);
}

void Raig::FindPath(int sourceX, int sourceY, int destinationX, int destinationY)
{
	m_Impl->findPath(sourceX, sourceY, destinationX, destinationY);
}

std::vector<Location> Raig::GetPath()
{
	return m_Impl->getPath();
}

bool Raig::IsPathfindingComplete()
{
	return m_Impl->IsPathfindingComplete();
}

void Raig::SendData(struct Packet* packet)
{
	m_Impl->sendData(packet);
}

void Raig::ReadData(struct Packet* packet)
{
	m_Impl->readData(packet);
}

void Raig::Update()
{
	m_Impl->update();
}




/*
 * RaigImpl implementation
 */

Raig::RaigImpl::RaigImpl()
{
	m_eState = IDLE;
	m_iSocketFileDescriptor = -1;
	m_iSentSequence = 0;
	m_bIsPathfindingComplete = true;
}

Raig::RaigImpl::~RaigImpl()
{
	cleanUp();
}

// Libsocket wrapper functions to abstract the library and reduce coupling
// Wrapper for libsocket Connection() that creates a peer connection based on the
int Raig::RaigImpl::InitConnection(char *hostname, char *service)
{
	// Initialize connection to the raig server
	m_iSocketFileDescriptor = Connection(hostname, service, TYPE_CLIENT, SOCK_STREAM);

	return m_iSocketFileDescriptor;
}

void Raig::RaigImpl::CreateGameWorld(int size)
{
	sprintf(m_cBuffer, "gameworld_%d", size);
	sendBuffer();
}

void Raig::RaigImpl::findPath(int sourceX, int sourceY, int destinationX, int destinationY)
{
	// Store path query data in the system buffer so
	// it can be sent on the next update
	m_iSentSequence++;
	m_bIsPathfindingComplete = false;
	sprintf(m_cBuffer, "path_%d_%d_%d_%d_%d", m_iSentSequence, sourceX, sourceY, destinationX, destinationY);
	m_vCompletePath.clear();
}

std::vector<Location> Raig::RaigImpl::getPath()
{
	std::reverse(m_vCompletePath.begin(), m_vCompletePath.end());
	return m_vCompletePath;
}

bool Raig::RaigImpl::IsPathfindingComplete()
{
	return m_bIsPathfindingComplete;
}

// Receive messages from the server using libsocket TODO: create wrapper in libsocket for revfrom()
int Raig::RaigImpl::sendBuffer()
{
	size_t size = strlen(m_cBuffer) + 1;
	int flags = 0;
	int bytesSents = 0;

	//printf("Buffer: %s\n", m_cBuffer);
	bytesSents = Send(m_iSocketFileDescriptor, m_cBuffer, size, flags);

	return bytesSents;
}

// Receive data from the connected server using recvfrom()
int Raig::RaigImpl::readBuffer()
{
	int flags = 0;
	int receivedBytes = 0;

	// Store network data in buffer and return pointer
	receivedBytes = Recv(m_iSocketFileDescriptor, m_cBuffer, MAX_BUF_SIZE, flags);

	//printf("Processing packet...\n");
	char *statusFlag = strtok((char*)m_cBuffer, "_");

	if(strcmp(statusFlag, "node") == 0)
	{
		// Parse the buffer and construct the path vector
		char *nodeId = strtok((char*)NULL, "_"); // Tokenize the string using '_' as delimiter
		char *nodeX = strtok((char*)NULL, "_"); // X coordinate
		char *nodeY = strtok((char*)NULL, "_"); // Y coordinate

		std::string locationId(nodeId); // char array to string
		int locationX = std::atoi(nodeX); // char array to int
		int locationY = std::atoi(nodeY); // char array to int

		Location location = {
				locationId,
				locationX,
				locationY
		};

		// Add a location to the path vector
		m_vPath.push_back(location);
		ClearBuffer();
	}
	else if(strcmp(statusFlag, "done") == 0)
	{
		// Parse the buffer and add the final location to the path vector
		char *nodeId = strtok((char*)NULL, "_"); // Tokenize the string using '_' as delimiter
		char *nodeX = strtok((char*)NULL, "_"); // X coordinate
		char *nodeZ = strtok((char*)NULL, "_"); // Z coordinate

		std::string locationId(nodeId); // char array to string
		int locationX = std::atoi(nodeX); // char array to int
		int locationZ = std::atoi(nodeZ); // char array to int

		Location location = {
				locationId,
				locationX,
				locationZ
		};

		// Add a location to the path vector
		m_vPath.push_back(location);
		m_vCompletePath.clear();
		m_vCompletePath = m_vPath;
		m_vPath.clear();

		m_bIsPathfindingComplete = true;

		m_eState = IDLE;
		ClearBuffer();
	}

	return receivedBytes;
}

void Raig::RaigImpl::ClearBuffer()
{
	sprintf(m_cBuffer, "null_");
}

void Raig::RaigImpl::sendData(struct Packet* packet)
{
	// Send struct packet on the network
	Write(m_iSocketFileDescriptor, packet, sizeof(Packet));
}

void Raig::RaigImpl::readData(struct Packet *packet)
{
	// Read network data into packet structure
	Read(m_iSocketFileDescriptor, packet, sizeof(Packet));
}

void Raig::RaigImpl::update()
{
	//std::cout << "Raig::RaigImpl::update()" << std::endl;

	// Send contents of buffer to the server
	sendBuffer();

	// Read messages from the server
	readBuffer();
}

void Raig::RaigImpl::cleanUp()
{
	std::cout << "Raig::RaigImpl::cleanUp()" << std::endl;

	m_vPath.clear();

	close(m_iSocketFileDescriptor);
}
