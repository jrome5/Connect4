#pragma once
#include "MCTS.h"

namespace MCTS
{
	class RewardDecay : public MCTS
	{

	protected:
		void backPropagate(std::shared_ptr<TreeNode>& node, const float delta) override;
	};
}

