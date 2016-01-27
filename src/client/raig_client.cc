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

#include "../external/libsocket/include/socket.h"


#include <algorithm> // std::reverse()
#include <cstring> // strlen(), strcat(), strtok(), strcpy()
#include <memory>

#include "net/net_manager.h"

namespace raig {

#define MAX_BUFFER_SIZE 13

class RaigClient::RaigClientImpl
{
public:

	RaigClientImpl();

	~RaigClientImpl();

	int InitConnection(std::string hostname, std::string service);

	void CreateGameWorld(int size, AiService serviceType);

	void SetCellOpen(base::Vector3 cell);

	void SetCellBlocked(base::Vector3 cell);

	void ReSendBlockedList();

	// Find a path using A* from source to destination
	void FindPath(base::Vector3 *start, base::Vector3 *goal);

	// Read the path data received by the server
	std::vector<std::unique_ptr<base::Vector3> > &GetPath();

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

	void ClearBuffer();

	// Private members and functions
	void CleanUp();

	std::unique_ptr<net::NetManager> m_NetManager;

	// Connection socket descriptor
	int m_iSocketFileDescriptor;

	// Network buffer
	char m_cSendBuffer[MAX_BUFFER_SIZE];

	char m_cRecvBuffer[MAX_BUFFER_SIZE];

	// vector of locations
	std::vector<std::unique_ptr<base::Vector3> > m_vPath;
	std::vector<std::unique_ptr<base::Vector3> > m_vCompletedPath;

	int m_iRecvSequence;

	bool m_bIsReqestComplete;

	std::vector<std::unique_ptr<base::Vector3> > m_vBlockedCells;

	// Game data used for re-connection attempts;
	std::string m_strHostname;
	std::string m_strService;
	int m_iGameWorldSize;
	AiService m_ServiceType;
};

/*
 *  Raig class implementation
 */
RaigClient::RaigClient()
	: m_Impl(new RaigClientImpl())
{
}

int RaigClient::InitConnection(std::string hostname, std::string service)
{
	return m_Impl->InitConnection(hostname, service);
}

void RaigClient::CreateGameWorld(int size, AiService serviceType)
{
	m_Impl->CreateGameWorld(size, serviceType);
}

void RaigClient::SetCellOpen(base::Vector3 cell)
{
	m_Impl->SetCellOpen(cell);
}

void RaigClient::SetCellBlocked(base::Vector3 cell)
{
	m_Impl->SetCellBlocked(cell);
}

void RaigClient::FindPath(base::Vector3 *start, base::Vector3 *goal)
{
	m_Impl->FindPath(start, goal);
}

std::vector<std::unique_ptr<base::Vector3> > &RaigClient::GetPath()
{
	return m_Impl->GetPath();
}

void RaigClient::Update()
{
	m_Impl->Update();
}

/*
 * RaigImpl implementation
 */
RaigClient::RaigClientImpl::RaigClientImpl()
{
	m_NetManager = std::unique_ptr<net::NetManager>(new net::NetManager());
	m_ServiceType = AiService::ASTAR; // default AStar
	m_iGameWorldSize = 0;
	m_iSocketFileDescriptor = -1;
	m_iRecvSequence = -1; // Start counting from -1
	m_bIsReqestComplete = true; // Server is ready for first request
}

RaigClient::RaigClientImpl::~RaigClientImpl()
{
	CleanUp();
}

int RaigClient::RaigClientImpl::InitConnection(std::string hostname, std::string service)
{
	// Store hostname and service for reconnection attempts
	m_strHostname = hostname;
	m_strService = service;

	return m_NetManager->Init(m_strHostname, m_strService);
}

void RaigClient::RaigClientImpl::CreateGameWorld(int size, AiService serviceType)
{
	std::cout << "CreateGameWorld()" << std::endl;
	// Store initial game world size and service type for re-connection attempts
	m_iGameWorldSize = size;
	m_ServiceType = serviceType;

	sprintf(m_cSendBuffer, "%02d_%03d_%02d_000000000", RaigClientImpl::GAMEWORLD, size, serviceType);
	m_NetManager->SendData(m_cSendBuffer);
}

void RaigClient::RaigClientImpl::SetCellOpen(base::Vector3 openCell)
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

	sprintf(m_cSendBuffer, "%02d_%02d_%02d_%02d_0000000", RaigClientImpl::CELL_OPEN, openCell.m_iX, openCell.m_iY, openCell.m_iZ);
	m_NetManager->SendData(m_cSendBuffer);
}

void RaigClient::RaigClientImpl::SetCellBlocked(base::Vector3 cell)
{
	// Add blocked cell to vector
	m_vBlockedCells.push_back(std::unique_ptr<base::Vector3>(new base::Vector3(cell)));

	sprintf(m_cSendBuffer, "%02d_%02d_%02d_%02d_0000000", RaigClientImpl::CELL_BLOCKED, cell.m_iX, cell.m_iY, cell.m_iZ);
	m_NetManager->SendData(m_cSendBuffer);
}

void RaigClient::RaigClientImpl::ReSendBlockedList()
{
	if(m_NetManager->GetState() == net::NetManager::CONNECTED)
	{
		// Time complexity O(N) to send all cells in vector to RAIG server
		for(int i = 0; i < m_vBlockedCells.size(); i++)
		{
			if(!m_vBlockedCells.empty())
			{
				sprintf(m_cSendBuffer, "%02d_%02d_%02d_%02d_0000000", RaigClientImpl::CELL_BLOCKED, m_vBlockedCells[i]->m_iX, m_vBlockedCells[i]->m_iY, m_vBlockedCells[i]->m_iZ);
				m_NetManager->SendData(m_cSendBuffer);
			}
		}
	}
}

void RaigClient::RaigClientImpl::FindPath(base::Vector3 *start, base::Vector3 *goal)
{
	if(m_NetManager->GetState() == net::NetManager::CONNECTED)
	{
		if(m_bIsReqestComplete == false)
		{
			// Connected but the server is busy processing a request
			// client must wait before sending another request
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

		sprintf(m_cSendBuffer, "%02d_%02d_%02d_%02d_%02d_0000", RaigClientImpl::PATH, start->m_iX, start->m_iZ, goal->m_iX, goal->m_iZ);
		m_NetManager->SendData(m_cSendBuffer);
		m_vPath.clear(); // Clear path storage

		// Send message to web application
		//m_NetManager->GetDao()->Create("raig_client", "true");

		// Path request sent. Set the request complete flag to prevent more requests until this one is complete
		m_bIsReqestComplete = false;
	}
}

std::vector<std::unique_ptr<base::Vector3> > &RaigClient::RaigClientImpl::GetPath()
{
	return m_vCompletedPath;
}

void RaigClient::RaigClientImpl::ClearBuffer()
{
	sprintf(m_cSendBuffer, "%d", RaigClientImpl::EMPTY);
	sprintf(m_cRecvBuffer, "%d", RaigClientImpl::EMPTY);
}

void RaigClient::RaigClientImpl::Update()
{
	int result = m_NetManager->ReadData(m_cRecvBuffer, MAX_BUFFER_SIZE);
	//std::cout << "Read bytes: " << result << " Buffer: " << m_cRecvBuffer << std::endl;

	// Re-connect to server
	if(m_NetManager->GetState() == net::NetManager::CONNECTION_FAILED)
	{
		std::cout << "Reconnecting to server" << std::endl;

		//printf("Trying to reconnect to server\n");
		InitConnection(m_strHostname, m_strService);

		if(m_NetManager->GetState() == net::NetManager::CONNECTED)
		{
			// Re-connection successful, send RAIG game world size and service
			// type used initially
			printf("Re-connection successful\n");
			CreateGameWorld(m_iGameWorldSize, m_ServiceType);
			ReSendBlockedList();
			m_bIsReqestComplete = true; // Game client has reconnected to the server, allow first request to be sent
			return;
		}

		std::cout << "Raig Update()" << std::endl;

		return;
	}

	if(result < 0)
	{
		//std::cout << "Read " << result << " skip processing" << std::endl;
		return;
	}

	// If the clients request is not yet complete continue to
	// process packets until an END packet is received signifying the
	// final Vector3 in the path
	if(m_bIsReqestComplete == false)
	{
		char *statusFlag = strtok((char*)m_cRecvBuffer, "_");
		int statusCode = atoi(statusFlag);

		if(statusCode == RaigClientImpl::NODE)
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
			m_vPath.push_back(std::unique_ptr<base::Vector3>(new base::Vector3(locationId, locationX, 0, locationZ)));
			ClearBuffer();
		}
		else if(statusCode == RaigClientImpl::END)
		{
			// Parse the buffer and add the final location to the path vector
			char *nodeId = strtok((char*)NULL, "_");
			char *nodeX = strtok((char*)NULL, "_");
			char *nodeZ = strtok((char*)NULL, "_");

			int locationId = std::atoi(nodeId);
			int locationX = std::atoi(nodeX);
			int locationZ = std::atoi(nodeZ);

			// Add vector to the path
			m_vPath.push_back(std::unique_ptr<base::Vector3>(new base::Vector3(locationId, locationX, 0, locationZ)));
			std::reverse(m_vPath.begin(), m_vPath.end()); // Reverse path before client game uses it
			m_vCompletedPath = std::move(m_vPath);
			m_bIsReqestComplete = true; // Received an END packet, allow more requests
			ClearBuffer();
		}
	}
}

void RaigClient::RaigClientImpl::CleanUp()
{
	m_vPath.clear();
	m_vBlockedCells.clear();
	m_vCompletedPath.clear();
	close(m_iSocketFileDescriptor);
}

} // namespace raig
