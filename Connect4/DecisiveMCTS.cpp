#include "DecisiveMCTS.h"
#include <boost/optional.hpp>

namespace MCTS
{
	boost::optional<int> findDecisiveMove(const std::vector<int> available_moves, const TreeNode v)
	{
		auto tmp_node = v;
		for (const auto move : available_moves)
		{
			auto child = tmp_node.addChild(move, false);
			if (child->data.terminal)
				return move;
		}
		return boost::none;
	}

	boost::optional<int> findAntiDecisiveMove(const std::vector<int> available_moves, const TreeNode v)
	{
		auto tmp_node = v;
		tmp_node.data.players_turn = !tmp_node.data.players_turn;
		return findDecisiveMove(available_moves, tmp_node);
	}

	int DecisiveMCTS::defaultPolicy(const std::shared_ptr<TreeNode> v0)
	{
		auto v = *v0;
		std::vector<int> moves;
		while (not v.data.terminal)
		{
			std::vector<int> available_actions = connect4::getAvailableMoves(v.data.current_state);
			if (available_actions.size() == 0)
			{
				return 0;
			}
			auto action = findDecisiveMove(available_actions, v);
			if (!action) //return to default policy if no decisive or antidecisive move found
			{
				action = findAntiDecisiveMove(available_actions, v);
				if (!action)
				{
					action = chooseRandomAction(available_actions);
				}
			}
			v = *v.addChild(*action, false);
			v.data.terminal = (v.data.terminal or v.depth == turns_remaining);
		}
		const bool node_win = (v0->data.players_turn == v.data.players_turn);
		return calculateReward(v, node_win);
	}
};