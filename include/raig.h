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
#ifndef _INCLUDE_RAIG_H
#define _INCLUDE_RAIG_H

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
