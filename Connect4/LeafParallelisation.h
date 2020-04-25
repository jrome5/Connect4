#pragma once
#include "MCTS.h"

namespace MCTS
{
	class LeafParallelisation : public MCTS
	{
	private:
		/*
		* @brief Play out the default policy simulatenously with threads
		* @param initial_state	state of the game
		* @param v				node
		* @return the average reward for from the threads
		*/
		float defaultPolicy(const std::shared_ptr<TreeNode> v0) override;
	};
}
