/*

The MIT License (MIT)

Copyright (c) 2016 David Morton

https://github.com/damorton/libraig.git

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/
#include <string>

#include "../include/raig.h" // API for developers
#include <algorithm>
#include <cstring>
extern "C" {
	#include "../external/libsocket/include/socket.h"
}

using namespace raig;

#define MAX_BUFFER_SIZE 13

// RaigImpl class declaration
class Raig::RaigImpl
{
public:

	RaigImpl();
	~RaigImpl();

	int InitConnection(char *hostname, char *service);

	void CreateGameWorld(int size);

	void SetCellOpen(Vector3 cell);
	void SetCellBlocked(Vector3 cell);

	// Find a path using A* from source to destination
	void FindPath(Vector3 *start, Vector3 *goal);

	// Read the path data received by the server
	std::vector<std::shared_ptr<Vector3> > GetPath();

	bool IsPathfindingComplete();

	// send buffer to the server
	int sendBuffer();

	// read data from the network into the buffer
	int ReadBuffer();

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
	char m_cSendBuffer[MAX_BUFFER_SIZE];

	char m_cRecvBuffer[MAX_BUFFER_SIZE];

	// vector of locations
	std::vector<std::shared_ptr<Vector3> > m_vPath;
	std::vector<std::shared_ptr<Vector3> > m_vCompletePath;

	int m_iSentSequence;
	int m_iRecvSequence;

	bool m_bIsPathfindingComplete;

	enum State{
		IDLE,
		PROCESSING
	};

	enum PacketCode{
		GAMEWORLD,
		PATH,
		NODE,
		END,
		EMPTY,
		CELL_BLOCKED,
		CELL_OPEN
	};

	int m_iProtocolId;

	State m_eState;

	std::vector<std::unique_ptr<Vector3> > m_vBlockedCells;

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

void Raig::SetCellOpen(Vector3 cell)
{
	m_Impl->SetCellOpen(cell);
}

void Raig::SetCellBlocked(Vector3 cell)
{
	m_Impl->SetCellBlocked(cell);
}


void Raig::FindPath(Vector3 *start, Vector3 *goal)
{
	m_Impl->FindPath(start, goal);
}

std::vector<std::shared_ptr<Vector3> > Raig::GetPath()
{
	return m_Impl->GetPath();
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
	m_iProtocolId = 23061912; // Turing
	m_eState = IDLE;
	m_iSocketFileDescriptor = -1;
	m_iSentSequence = 0;
	m_iRecvSequence = -1; // Start counting from -1
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

	SetNonBlocking(m_iSocketFileDescriptor);

	return m_iSocketFileDescriptor;
}

void Raig::RaigImpl::CreateGameWorld(int size)
{
	sprintf(m_cSendBuffer, "%02d_%03d_000000000000", RaigImpl::GAMEWORLD, size);
	sendBuffer();
}

void Raig::RaigImpl::SetCellOpen(Vector3 openCell)
{
	for(int i = 0; i <= m_vBlockedCells.size(); i++)
	{
		if(!m_vBlockedCells.empty())
		{
			printf("Cell blocked list: X:%d Z:%d\n", m_vBlockedCells[i]->m_iX, m_vBlockedCells[i]->m_iZ);
			if(m_vBlockedCells[i]->Compare(&openCell))
			{
				printf("Cell X:%d Z:%d == Cell X:%d Z:%d\n", m_vBlockedCells[i]->m_iX, m_vBlockedCells[i]->m_iZ, openCell.m_iX, openCell.m_iZ);
				m_vBlockedCells.erase(m_vBlockedCells.begin() + i);
				if(m_vBlockedCells.empty())
				{
					printf("Blocked cells list empty\n");
				}
			}
		}
		else
		{
			printf("Blocked cells list empty\n");
		}
	}

	sprintf(m_cSendBuffer, "%02d_%02d_%02d_%02d_0000000", RaigImpl::CELL_OPEN, openCell.m_iX, openCell.m_iY, openCell.m_iZ);
	printf("Cell X:%d Y:%d Z:%d opened - buffer : %s\n", openCell.m_iX, openCell.m_iY, openCell.m_iZ, m_cSendBuffer);
	sendBuffer();
}

void Raig::RaigImpl::SetCellBlocked(Vector3 cell)
{
	// Add blocked cell to vector
	m_vBlockedCells.push_back(std::unique_ptr<Vector3>(new Vector3(cell)));
	sprintf(m_cSendBuffer, "%02d_%02d_%02d_%02d_0000000", RaigImpl::CELL_BLOCKED, cell.m_iX, cell.m_iY, cell.m_iZ);
	printf("Cell X:%d Y:%d Z:%d blocked - buffer : %s\n", cell.m_iX, cell.m_iY, cell.m_iZ, m_cSendBuffer);
	sendBuffer();
}

void Raig::RaigImpl::FindPath(Vector3 *start, Vector3 *goal)
{
	if(!IsPathfindingComplete())
	{
		return;
	}

	printf("Find path start (%d, %d, %d) : goal (%d, %d, %d)\n", start->m_iX, start->m_iY, start->m_iZ, goal->m_iX, goal->m_iY, goal->m_iZ);

	if(!m_vBlockedCells.empty())
	{
		for(int i = 0; i < m_vBlockedCells.size(); i++)
		{
			if(m_vBlockedCells[i]->Compare(start) || m_vBlockedCells[i]->Compare(goal))
			{
				printf("Invalid path, start or end goal is blocked\n");
				return;
			}
		}
	}

	printf("Blocked cells vector checked, sending request\n");

	// Store path query data in the system buffer so
	// it can be sent on the next update
	m_iSentSequence++;
	m_bIsPathfindingComplete = false;
	sprintf(m_cSendBuffer, "%02d_%02d_%02d_%02d_%02d_0000", RaigImpl::PATH, start->m_iX, start->m_iZ, goal->m_iX, goal->m_iZ);
	sendBuffer();
	//m_vCompletePath.clear();
	m_vPath.clear();

	printf("Path query sent OK\n");
}

std::vector<std::shared_ptr<Vector3> > Raig::RaigImpl::GetPath()
{
	// Return a copy of the completed path
	//std::reverse(m_vCompletePath.begin(), m_vCompletePath.end());
	std::reverse(m_vPath.begin(), m_vPath.end());
	std::vector<std::shared_ptr<Vector3> > temp(m_vPath);
	m_vPath.clear();
	return temp;
}

bool Raig::RaigImpl::IsPathfindingComplete()
{
	return m_bIsPathfindingComplete;
}

// Receive messages from the server using libsocket TODO: create wrapper in libsocket for revfrom()
int Raig::RaigImpl::sendBuffer()
{
	size_t size = strlen(m_cSendBuffer) + 1;
	int flags = 0;
	int bytesSents = 0;

	//printf("Buffer: %s\n", m_cBuffer);
	bytesSents = Send(m_iSocketFileDescriptor, m_cSendBuffer, size, flags);
	printf("Buffer: %s\n", m_cSendBuffer);
	ClearBuffer();
	return bytesSents;
}

// Receive data from the connected server using recvfrom()
int Raig::RaigImpl::ReadBuffer()
{
	//printf("Called ReadBuffer()\n");
	int size = sizeof(m_cRecvBuffer);
	int bufferSpace = size;

	int flags = 0;
	int bytesRecv = 0;
	char temp[MAX_BUFFER_SIZE] = "\0";
	int continueReading = 1;

	do{
		bytesRecv = Recv(m_iSocketFileDescriptor, m_cRecvBuffer, size, flags);

		if(bytesRecv > 0 && bytesRecv < MAX_BUFFER_SIZE)
		{
			printf("bytes received : %d buffer : %s\n\n", bytesRecv, m_cRecvBuffer);
			size -= bytesRecv;

			if(size > 0)
			{
				strcat(temp, m_cRecvBuffer);
				temp[bytesRecv] = '\0';
				printf("strcat() temp : %s m_cRecvBuffer : %s\n", temp, m_cRecvBuffer);
			}
			else
			{
				strcat(temp, m_cRecvBuffer);
				strcpy(m_cRecvBuffer, temp);
				printf("strcpy() m_cRecvBuffer : %s temp : %s \n", m_cRecvBuffer, temp);
			}
			printf("bytes left to read: %d temp buffer: %s\n", size, temp);
		}
		else if(bytesRecv > 0)
		{
			printf("bytes received : %d buffer : %s\n\n", bytesRecv, m_cRecvBuffer);
			size -= bytesRecv;
		}

	}while(size > 0);

	temp[0] = '\0';

	return bytesRecv;
}

void Raig::RaigImpl::ClearBuffer()
{
	sprintf(m_cSendBuffer, "%d", RaigImpl::EMPTY);
	sprintf(m_cRecvBuffer, "%d", RaigImpl::EMPTY);
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
	if(!m_bIsPathfindingComplete)
	{
		// Read messages from the server
		ReadBuffer();

		char *statusFlag = strtok((char*)m_cRecvBuffer, "_");
		int statusCode = atoi(statusFlag); // Convert to integer

		if(statusCode == RaigImpl::NODE)
		{
			//printf("NODE: %s\n", m_cRecvBuffer);
			// Parse the buffer and construct the path vector
			char *nodeId = strtok((char*)NULL, "_"); // Tokenize the string using '_' as delimiter
			char *nodeX = strtok((char*)NULL, "_"); // X coordinate
			char *nodeZ = strtok((char*)NULL, "_"); // Y coordinate

			//std::string locationId(nodeId); // char array to string
			int locationId = std::atoi(nodeId); // char array to int
			int locationX = std::atoi(nodeX); // char array to int
			int locationZ = std::atoi(nodeZ); // char array to int

			if(locationId == m_iRecvSequence)
			{
				// Already processed the node
				return;
			}
			m_iRecvSequence = locationId;

			// Add a location to the path vector
			//m_vPath.push_back(std::shared_ptr<Vector3>(new Vector3(locationId, locationX, 0, locationZ)));
			m_vPath.push_back(std::shared_ptr<Vector3>(new Vector3(locationId, locationX, 0, locationZ)));
			ClearBuffer();
		}
		else if(statusCode == RaigImpl::END)
		{
			//printf("END: %s\n", m_cRecvBuffer);
			// Parse the buffer and add the final location to the path vector
			char *nodeId = strtok((char*)NULL, "_"); // Tokenize the string using '_' as delimiter
			char *nodeX = strtok((char*)NULL, "_"); // X coordinate
			char *nodeZ = strtok((char*)NULL, "_"); // Z coordinate

			//std::string locationId(nodeId); // char array to string
			int locationId = std::atoi(nodeId); // char array to int
			int locationX = std::atoi(nodeX); // char array to int
			int locationZ = std::atoi(nodeZ); // char array to int

			// Add a location to the path vector
			//m_vPath.push_back(std::shared_ptr<Vector3>(new Vector3(locationId, locationX, 0, locationZ)));
			m_vPath.push_back(std::shared_ptr<Vector3>(new Vector3(locationId, locationX, 0, locationZ)));
			//m_vCompletePath.clear();
			//m_vCompletePath = m_vPath;
			//m_vPath.clear();

			m_bIsPathfindingComplete = true;

			m_eState = IDLE;
			ClearBuffer();
		}
	}
	/*
	if(strcmp(statusFlag, "node") == 0)
	{
		// Parse the buffer and construct the path vector
		char *nodeId = strtok((char*)NULL, "_"); // Tokenize the string using '_' as delimiter
		char *nodeX = strtok((char*)NULL, "_"); // X coordinate
		char *nodeZ = strtok((char*)NULL, "_"); // Y coordinate

		//std::string locationId(nodeId); // char array to string
		int locationId = std::atoi(nodeId); // char array to int
		int locationX = std::atoi(nodeX); // char array to int
		int locationZ = std::atoi(nodeZ); // char array to int


		// Add a location to the path vector
		//m_vPath.push_back(std::shared_ptr<Vector3>(new Vector3(locationId, locationX, 0, locationZ)));
		m_vPath.push_back(std::shared_ptr<Vector3>(new Vector3(locationId, locationX, 0, locationZ)));
		ClearBuffer();
	}
	else if(strcmp(statusFlag, "done") == 0)
	{
		// Parse the buffer and add the final location to the path vector
		char *nodeId = strtok((char*)NULL, "_"); // Tokenize the string using '_' as delimiter
		char *nodeX = strtok((char*)NULL, "_"); // X coordinate
		char *nodeZ = strtok((char*)NULL, "_"); // Z coordinate

		//std::string locationId(nodeId); // char array to string
		int locationId = std::atoi(nodeId); // char array to int
		int locationX = std::atoi(nodeX); // char array to int
		int locationZ = std::atoi(nodeZ); // char array to int

		// Add a location to the path vector
		//m_vPath.push_back(std::shared_ptr<Vector3>(new Vector3(locationId, locationX, 0, locationZ)));
		m_vPath.push_back(std::shared_ptr<Vector3>(new Vector3(locationId, locationX, 0, locationZ)));
		m_vCompletePath.clear();
		m_vCompletePath = m_vPath;
		m_vPath.clear();

		m_bIsPathfindingComplete = true;

		m_eState = IDLE;
		ClearBuffer();
	}
	*/
}

void Raig::RaigImpl::cleanUp()
{
	std::cout << "Raig::RaigImpl::cleanUp()" << std::endl;

	m_vPath.clear();

	close(m_iSocketFileDescriptor);
}
