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
	// Libsocket wrapper functions to abstract the library and reduce coupling
	// Wrapper for libsocket Connection() that creates a peer connection based on the
	int InitConnection(char *hostname, char *service);

	// Store the path in a vector of x, y coordinate locations
	void findPath(int sourceX, int sourceY, int destinationX, int destinationY);

	std::vector<Location> getPath();

	bool IsPathfindingComplete();

	void sendData(struct Packet* packet);

	void readData(struct Packet* packet);

	void update();

private:
	class RaigImpl; // Forward declaration
	std::unique_ptr<RaigImpl> m_Impl; // Raig implementation using auto_ptr
};

} // namespace raig
#endif
