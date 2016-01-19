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

#include "net/net_manager.h"

extern "C" {
	#include "../external/libsocket/include/socket.h"
}

#include <cstring> // strlen()
#include <iostream>

namespace net{

NetManager::NetManager()
{

}

int NetManager::Init(std::string hostname, std::string service)
{
	printf("hostname: %s server: %s\n", hostname.c_str(), service.c_str());
	// Store hostname and service for reconnection attempts
	m_strHostname = hostname;
	m_strService = service;

	// Initialize connection to the raig server
	// TODO: give libsocket a namespace
	m_iSocketFileDescriptor = Connection(m_strHostname.c_str(), m_strService.c_str(), TYPE_CLIENT, SOCK_STREAM);

	if(m_iSocketFileDescriptor == -1)
	{
		m_eState = CONNECTION_FAILED;
		printf("InitConnection() Connection failed. Socketfd %d\n", m_iSocketFileDescriptor);
		return -1;
	}

	std::cout << "Init() connection successful" << std::endl;

	m_eState = CONNECTED;
	SetNonBlocking(m_iSocketFileDescriptor);

	return m_iSocketFileDescriptor;
}

int NetManager::SendData(char* buffer)
{
	m_SendBuffer = buffer;
	size_t size = strlen(buffer) + 1;
	int flags = 0;
	int bytesSents = 0;

	bytesSents = Send(m_iSocketFileDescriptor, buffer, size, flags);

	return bytesSents;
}

int NetManager::ReadData(char* buffer, int size)
{
	m_ReadBuffer = buffer;
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
		bytesRecv = Recv(m_iSocketFileDescriptor, buffer, size, flags);

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
			break;
		}

		if(bytesRecv > 0 && bytesRecv < MAX_BUFFER_SIZE && m_eState == CONNECTED)
		{
			size -= bytesRecv;

			if(size > 0)
			{
				strcat(temp, buffer);
				temp[bytesRecv] = '\0';
			}
			else
			{
				strcat(temp, buffer);
				strcpy(buffer, temp);
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

} // namespace net
