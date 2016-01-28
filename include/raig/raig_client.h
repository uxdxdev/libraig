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

#ifndef RAIG_RAIG_H
#define RAIG_RAIG_H

#include "export/raig_Export.h"

#include <memory>
#include <vector>
#include <iostream>

#include "base/vector3.h"

namespace raig{

class RaigClient
{
public:

	// Ai services available to clients
	enum AiService{
		ASTAR,
		FSM,
		BFS,
		DFS
	};

	raig_EXPORT RaigClient();

	int raig_EXPORT InitConnection(std::string hostname, std::string service);

	void raig_EXPORT CreateGameWorld(int size, AiService serviceType);

	void raig_EXPORT SetCellOpen(base::Vector3 cell);

	void raig_EXPORT SetCellBlocked(base::Vector3 cell);

	void raig_EXPORT FindPath(base::Vector3 *start, base::Vector3 *goal);

	std::vector<std::unique_ptr<base::Vector3> > raig_EXPORT &GetPath();

	void raig_EXPORT Update();

private:
	class RaigClientImpl;
	std::unique_ptr<RaigClientImpl> m_Impl;
};

} // namespace raig
#endif
