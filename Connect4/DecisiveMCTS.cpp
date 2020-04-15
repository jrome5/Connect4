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

	std::shared_ptr<TreeNode> DecisiveMCTS::expand(std::shared_ptr<TreeNode>& v)
	{
		//choose action 'a' from untried actions
		std::vector<int> actions = connect4::getAvailableMoves(v->data.current_state);
		std::vector<int> available_actions;
		for (auto i = 0; i < actions.size(); i++)
		{
			const auto itr = std::find_if(v->children.cbegin(), v->children.cend(), [&](const std::shared_ptr<TreeNode>& obj)
				{ return obj->data.action == actions[i]; });
			const auto found = itr != v->children.cend();
			if (!found)
				available_actions.push_back(actions[i]);
		}
		if (available_actions.size() == 0)
		{
			v->data.fully_expanded = true;
			return v;
		}
		else
		{
			auto action = findDecisiveMove(available_actions, *v);
			if (!action)
			{
				action = findAntiDecisiveMove(available_actions, *v);
				if (!action)
				{
					chooseRandomAction(available_actions);
				}
			}
			//create new leaf node v' of v with action 'a'
			//add node to tree and return new node
			std::shared_ptr<TreeNode> n = v->addChild(*action);
			n->data.terminal = (n->data.terminal or n->depth == turns_remaining);
			v->data.fully_expanded = v->children.size() == actions.size();
			return n;
		}
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