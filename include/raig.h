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
