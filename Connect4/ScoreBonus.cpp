#include "ScoreBonus.h"

constexpr float GAME_LENGTH = 42.0f;

namespace MCTS
{
	float ScoreBonus::calculateReward(const TreeNode leaf, const bool node_win)
	{
		const bool draw = (turns_remaining == leaf.depth);
		if (draw)
			return 0;
		else
		{
			//return reward proportional to game size
			const float win_strength = (GAME_LENGTH - (turns_remaining + leaf.depth)) / GAME_LENGTH;
			return node_win ? win_strength : -win_strength;
		}
	}
}