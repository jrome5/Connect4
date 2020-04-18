#pragma once
#include "MCTS.h"

namespace MCTS
{
	class ScoreBonus : public MCTS
	{
	protected:
		float calculateReward(const TreeNode leaf, const bool node_win) override;
	};
}
