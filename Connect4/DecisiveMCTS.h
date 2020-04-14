#pragma once
#include "MCTS.h"

namespace MCTS
{
	class DecisiveMCTS : public MCTS
	{
	protected:
		int defaultPolicy(const std::shared_ptr<TreeNode> v0);

	};
}

