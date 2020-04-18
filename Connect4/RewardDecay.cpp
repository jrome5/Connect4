#include "RewardDecay.h"
#include <iostream>
#include <time.h>

namespace MCTS
{
	void RewardDecay::backPropagate(std::shared_ptr<TreeNode>& node, const float delta)
	{
		node->data.visited += 1;
		node->data.reward += delta;
		if (node->parent)
		{
			backPropagate(node->parent, -0.75f * delta);
		}
		return;
	}
}