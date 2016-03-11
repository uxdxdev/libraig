// Copyright (c) 2016 David Morton
// Use of this source code is governed by a license that can be
// found in the LICENSE file.
// https://github.com/damorton/libraig.git

#ifndef NET_NET_MANAGER_H_
#define NET_NET_MANAGER_H_

#include <string> // string

#include "http/http_client.h"

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

	int Init(std::shared_ptr<std::string> hostname, std::shared_ptr<std::string> service);

	State GetState(){ return m_eState; }

	// send buffer to the server
	int SendData(char* buffer);

	// read data from the network into the buffer
	int ReadData(char* buffer, int size = MAX_BUFFER_SIZE);

	http::HttpDao *GetDao(){ return m_HttpDao.get(); }

private:

	// Private members and functions
	void CleanUp();

	// Connection socket descriptor
	int m_iSocketFileDescriptor;

	State m_eState;

	// Game data used for re-connection attempts;
	std::shared_ptr<std::string> m_strHostname;
	std::shared_ptr<std::string> m_strService;

	char* m_SendBuffer;
	char* m_ReadBuffer;

	std::unique_ptr<http::HttpDao> m_HttpDao;
};

}

#endif
