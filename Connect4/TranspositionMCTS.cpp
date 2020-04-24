#include "TranspositionMCTS.h"

namespace MCTS
{
	int TranspositionMCTS::search(const state& s, const int remaining)
	{
		int chosen_action = MCTS::search(s, remaining);
		transposition_table_.clear();
		return chosen_action;
	}

	std::shared_ptr<TreeNode> TranspositionMCTS::expand(std::shared_ptr<TreeNode>& v)
	{
		//choose action 'a' from untried actions
		std::vector<int> actions = v->data.current_state.getAvailableMoves();
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
			//create new leaf node v' of v with action 'a'
			//add node to tree and return new node
			std::shared_ptr<TreeNode> n = v->addChildTransposition(transposition_table_, chooseRandomAction(available_actions));
			n->data.terminal = (n->data.terminal or n->depth == turns_remaining);
			v->data.fully_expanded = v->children.size() == actions.size();
			return n;
		}
	}
}