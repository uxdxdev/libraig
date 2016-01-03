#ifndef _RAIG_RAIG_H
#define _RAIG_RAIG_H

#include <memory>
#include <vector>
#include <iostream>
#include "vector3.h"

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

	int InitConnection(char *hostname, char *service);

	void CreateGameWorld(int size);

	// Store the path in a vector of x, y coordinate locations
	void FindPath(int sourceX, int sourceY, int destinationX, int destinationY);

	std::vector<std::shared_ptr<Vector3> > *GetPath();

	bool IsPathfindingComplete();

	void SendData(struct Packet* packet);

	void ReadData(struct Packet* packet);

	void Update();

private:
	class RaigImpl;
	std::unique_ptr<RaigImpl> m_Impl;
};

} // namespace raig
#endif
