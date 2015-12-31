#ifndef _RAIG_RAIG_H
#define _RAIG_RAIG_H

#include <memory>
#include <vector>
#include <iostream>

namespace raig{

struct Packet
{
	char stringData[255];
	int x;
	int y;
	int completeFlag;
};

struct Location{
	std::string id;
	int x;
	int y;
};

class Raig
{
public:
	Raig();
	~Raig();

	int InitConnection(char *hostname, char *service);

	// Store the path in a vector of x, y coordinate locations
	void findPath(int sourceX, int sourceY, int destinationX, int destinationY);

	std::vector<Location> getPath();

	bool IsPathfindingComplete();

	void sendData(struct Packet* packet);

	void readData(struct Packet* packet);

	void update();

private:
	class RaigImpl;
	std::unique_ptr<RaigImpl> m_Impl;
};

} // namespace raig
#endif
