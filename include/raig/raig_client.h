// Copyright (c) 2016 David Morton
// Use of this source code is governed by a license that can be
// found in the LICENSE file.
// https://github.com/damorton/libraig.git

#ifndef RAIG_RAIG_H
#define RAIG_RAIG_H

#include <memory>
#include <vector>

#include "export/raig_Export.h"
#include "base/vector3.h"

namespace raig{

class RaigClient
{
public:

	// Ai services available to clients
	enum AiService{
		ASTAR, // A* pathfinding
		FSM, // Finite state machine
		BFS, // Breadth first search
		DFS // Depth first search
	};

	raig_EXPORT RaigClient();

	raig_EXPORT ~RaigClient();

	int raig_EXPORT InitConnection(std::shared_ptr<std::string> hostname, std::shared_ptr<std::string> service);

	void raig_EXPORT CreateGameWorld(int width, int height, AiService serviceType);

	void raig_EXPORT ResetGameWorld();

	void raig_EXPORT SetCellOpen(base::Vector3 cell);

	void raig_EXPORT SetCellBlocked(base::Vector3 cell);

	int raig_EXPORT FindPath(base::Vector3 *start, base::Vector3 *goal);

	std::vector<std::unique_ptr<base::Vector3> > raig_EXPORT &GetPath();

	void raig_EXPORT Update();

private:
	class RaigClientImpl;
	std::unique_ptr<RaigClientImpl> m_Impl;
};

} // namespace raig
#endif
