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

#ifndef NET_NET_MANAGER_H_
#define NET_NET_MANAGER_H_

#include <string> // string

namespace net{

#define MAX_BUFFER_SIZE 13

class NetManager {
public:

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

	NetManager();

	int Init(std::string hostname, std::string service);

	State GetState(){ return m_eState; }

	// send buffer to the server
	int SendData(char* buffer);

	// read data from the network into the buffer
	int ReadData(char* buffer, int size = MAX_BUFFER_SIZE);

private:

	// Private members and functions
	void CleanUp();

	// Connection socket descriptor
	int m_iSocketFileDescriptor;

	State m_eState;

	// Game data used for re-connection attempts;
	std::string m_strHostname;
	std::string m_strService;

	char* m_SendBuffer;
	char* m_ReadBuffer;
};

}

#endif
