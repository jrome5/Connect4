#pragma once
#include "MCTS.h"

namespace MCTS
{
	class DecisiveMCTS : public MCTS
	{
	protected:

		std::shared_ptr<TreeNode> expand(std::shared_ptr<TreeNode>& v);

		int defaultPolicy(const std::shared_ptr<TreeNode> v0);
	};
}

