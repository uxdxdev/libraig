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
#include "raig/raig_client.h" // API
#include <algorithm> // std::reverse()
#include <cstring> // strlen(), strcat(), strtok(), strcpy()

extern "C" {
	#include "../external/libsocket/include/socket.h"
}

using namespace raig;

#define MAX_BUFFER_SIZE 13

class Raig::RaigImpl
{
public:

	RaigImpl();

	~RaigImpl();

	int InitConnection(std::string hostname, std::string service);

	void CreateGameWorld(int size, AiService serviceType);

	void SetCellOpen(Vector3 cell);

	void SetCellBlocked(Vector3 cell);

	void ReSendBlockedList();

	// Find a path using A* from source to destination
	void FindPath(Vector3 *start, Vector3 *goal);

	// Read the path data received by the server
	std::vector<std::shared_ptr<Vector3> > GetPath();

	bool IsPathfindingComplete();

	// Update the raig engine
	void Update();

private:
	enum State{
		CONNECTED,
		CONNECTION_FAILED,
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

	// send buffer to the server
	int SendBuffer();

	// read data from the network into the buffer
	int ReadBuffer();

	void ClearBuffer();

	// Private members and functions
	void CleanUp();

	// Connection socket descriptor
	int m_iSocketFileDescriptor;

	// Network buffer
	char m_cSendBuffer[MAX_BUFFER_SIZE];

	char m_cRecvBuffer[MAX_BUFFER_SIZE];

	// vector of locations
	std::vector<std::shared_ptr<Vector3> > m_vPath;

	int m_iRecvSequence;

	bool m_bIsPathfindingComplete;

	int m_iProtocolId;

	State m_eState;

	std::vector<std::unique_ptr<Vector3> > m_vBlockedCells;

	// Game data used for re-connection attempts;
	std::string m_strHostname;
	std::string m_strService;
	int m_iGameWorldSize;
	AiService m_ServiceType;
};

/*
 *  Raig class implementation
 */
Raig::Raig() : m_Impl(new RaigImpl())
{
}

int Raig::InitConnection(std::string hostname, std::string service)
{
	return m_Impl->InitConnection(hostname, service);
}

void Raig::CreateGameWorld(int size, AiService serviceType)
{
	m_Impl->CreateGameWorld(size, serviceType);
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

void Raig::Update()
{
	m_Impl->Update();
}

/*
 * RaigImpl implementation
 */
Raig::RaigImpl::RaigImpl()
{
	m_iProtocolId = 23061912; // Turing
	m_eState = CONNECTION_FAILED;
	m_iSocketFileDescriptor = -1;
	m_iRecvSequence = -1; // Start counting from -1
	m_bIsPathfindingComplete = true;
}

Raig::RaigImpl::~RaigImpl()
{
	CleanUp();
}

int Raig::RaigImpl::InitConnection(std::string hostname, std::string service)
{
	// Store hostname and service for reconnection attempts
	m_strHostname = hostname;
	m_strService = service;

	// Initialize connection to the raig server
	m_iSocketFileDescriptor = Connection(m_strHostname.c_str(), m_strService.c_str(), TYPE_CLIENT, SOCK_STREAM);

	if(m_iSocketFileDescriptor == -1)
	{
		m_eState = CONNECTION_FAILED;
		//printf("InitConnection() Connection failed. Socketfd %d\n", m_iSocketFileDescriptor);
		return -1;
	}
	m_eState = CONNECTED;
	SetNonBlocking(m_iSocketFileDescriptor);

	return m_iSocketFileDescriptor;
}

void Raig::RaigImpl::CreateGameWorld(int size, AiService serviceType)
{
	// Store initial game world size and service type for re-connection attempts
	m_iGameWorldSize = size;
	m_ServiceType = serviceType;

	if(m_eState == CONNECTED)
	{
		sprintf(m_cSendBuffer, "%02d_%03d_%02d_000000000", RaigImpl::GAMEWORLD, size, serviceType);
		SendBuffer();
	}
}

void Raig::RaigImpl::SetCellOpen(Vector3 openCell)
{
	// Search blocked cells vector and remove the openCell if it was found
	// Time complexity O(N)
	for(int i = 0; i <= m_vBlockedCells.size(); i++)
	{
		if(!m_vBlockedCells.empty())
		{
			if(m_vBlockedCells[i]->Compare(&openCell))
			{
				//printf("Cell X:%d Z:%d == Cell X:%d Z:%d\n", m_vBlockedCells[i]->m_iX, m_vBlockedCells[i]->m_iZ, openCell.m_iX, openCell.m_iZ);
				m_vBlockedCells.erase(m_vBlockedCells.begin() + i);
				if(m_vBlockedCells.empty())
				{
					//printf("Blocked cells list empty\n");
				}
			}
		}
		else
		{
			//printf("Blocked cells list empty\n");
		}
	}

	if(m_eState == CONNECTED)
	{
		sprintf(m_cSendBuffer, "%02d_%02d_%02d_%02d_0000000", RaigImpl::CELL_OPEN, openCell.m_iX, openCell.m_iY, openCell.m_iZ);
		//printf("Cell X:%d Y:%d Z:%d opened - buffer : %s\n", openCell.m_iX, openCell.m_iY, openCell.m_iZ, m_cSendBuffer);
		SendBuffer();
	}
}

void Raig::RaigImpl::SetCellBlocked(Vector3 cell)
{
	// Add blocked cell to vector
	m_vBlockedCells.push_back(std::unique_ptr<Vector3>(new Vector3(cell)));

	if(m_eState == CONNECTED)
	{
		sprintf(m_cSendBuffer, "%02d_%02d_%02d_%02d_0000000", RaigImpl::CELL_BLOCKED, cell.m_iX, cell.m_iY, cell.m_iZ);
		SendBuffer();
	}
}

void Raig::RaigImpl::ReSendBlockedList()
{
	if(m_eState == CONNECTED)
	{
		// Time complexity O(N) to send all cells in vector to RAIG server
		for(int i = 0; i < m_vBlockedCells.size(); i++)
		{
			if(!m_vBlockedCells.empty())
			{
				sprintf(m_cSendBuffer, "%02d_%02d_%02d_%02d_0000000", RaigImpl::CELL_BLOCKED, m_vBlockedCells[i]->m_iX, m_vBlockedCells[i]->m_iY, m_vBlockedCells[i]->m_iZ);
				SendBuffer();
			}
		}
	}
}

void Raig::RaigImpl::FindPath(Vector3 *start, Vector3 *goal)
{
	if(m_eState == CONNECTED)
	{
		if(!m_bIsPathfindingComplete)
		{
			// If the previous path request has not been completed
			return;
		}

		// Check if the start of goal cell is a blocked cell
		// Time complexity O(N)
		if(!m_vBlockedCells.empty())
		{
			for(int i = 0; i < m_vBlockedCells.size(); i++)
			{
				if(m_vBlockedCells[i]->Compare(start) || m_vBlockedCells[i]->Compare(goal))
				{
					//printf("Invalid path, start or end goal is blocked\n");
					return;
				}
			}
		}

		// Set the path complete flag to prevent more requests until this one is complete
		m_bIsPathfindingComplete = false;

		sprintf(m_cSendBuffer, "%02d_%02d_%02d_%02d_%02d_0000", RaigImpl::PATH, start->m_iX, start->m_iZ, goal->m_iX, goal->m_iZ);
		SendBuffer();

		m_vPath.clear();

		//printf("Path query sent OK\n");
	}
}

std::vector<std::shared_ptr<Vector3> > Raig::RaigImpl::GetPath()
{
	// Return a copy of the completed path. Path vectors must be reversed because
	// RAIG sends them from Goal to Start order
	std::reverse(m_vPath.begin(), m_vPath.end());
	return m_vPath;
}

bool Raig::RaigImpl::IsPathfindingComplete()
{
	return m_bIsPathfindingComplete;
}

int Raig::RaigImpl::SendBuffer()
{
	size_t size = strlen(m_cSendBuffer) + 1;
	int flags = 0;
	int bytesSents = 0;

	bytesSents = Send(m_iSocketFileDescriptor, m_cSendBuffer, size, flags);
	ClearBuffer();
	return bytesSents;
}

int Raig::RaigImpl::ReadBuffer()
{
	int size = sizeof(m_cRecvBuffer);
	int bufferSpace = size;
	int flags = 0;
	int bytesRecv = 0;
	char temp[MAX_BUFFER_SIZE] = "\0";
	int continueReading = 1;

	// Continue to loop until full TCP segment has been received and parsed. The issue here is that
	// a segment can contain the end of one vector packet and the beginning of another which causes an
	// error when reading partial data. The solution was to only read the full size of a packet each time
	// from the kernel buffer. If a partial packet was received at the end of a TCP segment, reduce the number
	// of bytes read from the kernel buffer corresponding to the end of the previous segment packet data.
	// Example:
	//
	//		20 Byte packet
	//
	//						TCP Segment 1				 | 				TCP Segment 2
	//		| 		20 Bytes 	|		10 Bytes		~|~		10 Bytes	|		20 Bytes 	|
	//
	//
	do{
		bytesRecv = Recv(m_iSocketFileDescriptor, m_cRecvBuffer, size, flags);

		// Normal non-blocking receive when connected
		if(bytesRecv == -1 && m_eState == CONNECTED)
		{
			// Non blocking socket returns -1 if there is no data to read
			// in the buffer. Returns 0 on shutdown.
			break;
		}

		// Server shutdown connection
		if(bytesRecv == 0 && m_eState == CONNECTED)
		{
			m_eState = CONNECTION_FAILED;
		}

		if(m_eState == CONNECTION_FAILED)
		{
			// Re-connection attempt failed
			//printf("Trying to reconnect to server\n");
			InitConnection(m_strHostname, m_strService);

			if(m_eState == CONNECTED)
			{
				// Re-connection successful, send RAIG game world size and service
				// type used initially
				printf("Re-connection successful\n");
				CreateGameWorld(m_iGameWorldSize, m_ServiceType);
				ReSendBlockedList();
				m_bIsPathfindingComplete = true;
				break;
			}

			break;
		}

		if(bytesRecv > 0 && bytesRecv < MAX_BUFFER_SIZE)
		{
			size -= bytesRecv;

			if(size > 0)
			{
				strcat(temp, m_cRecvBuffer);
				temp[bytesRecv] = '\0';
			}
			else
			{
				strcat(temp, m_cRecvBuffer);
				strcpy(m_cRecvBuffer, temp);
			}
		}
		else if(bytesRecv > 0)
		{
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

void Raig::RaigImpl::Update()
{
	if(ReadBuffer() <= 0)
	{
		return;
	}

	if(!m_bIsPathfindingComplete)
	{
		char *statusFlag = strtok((char*)m_cRecvBuffer, "_");
		int statusCode = atoi(statusFlag);

		if(statusCode == RaigImpl::NODE)
		{
			// Parse the buffer and construct the path vector
			char *nodeId = strtok((char*)NULL, "_");
			char *nodeX = strtok((char*)NULL, "_");
			char *nodeZ = strtok((char*)NULL, "_");

			int locationId = std::atoi(nodeId);
			int locationX = std::atoi(nodeX);
			int locationZ = std::atoi(nodeZ);

			if(locationId == m_iRecvSequence)
			{
				// Already processed the node
				return;
			}
			m_iRecvSequence = locationId;

			// Add vector to the path
			m_vPath.push_back(std::shared_ptr<Vector3>(new Vector3(locationId, locationX, 0, locationZ)));
			ClearBuffer();
		}
		else if(statusCode == RaigImpl::END)
		{
			// Parse the buffer and add the final location to the path vector
			char *nodeId = strtok((char*)NULL, "_");
			char *nodeX = strtok((char*)NULL, "_");
			char *nodeZ = strtok((char*)NULL, "_");

			int locationId = std::atoi(nodeId);
			int locationX = std::atoi(nodeX);
			int locationZ = std::atoi(nodeZ);

			// Add vector to the path
			m_vPath.push_back(std::shared_ptr<Vector3>(new Vector3(locationId, locationX, 0, locationZ)));
			m_bIsPathfindingComplete = true;
			ClearBuffer();
		}
	}
}

void Raig::RaigImpl::CleanUp()
{
	m_vPath.clear();
	close(m_iSocketFileDescriptor);
}
