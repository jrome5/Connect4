#include "MCTS.h"
#include <cmath>
#include <iostream>
#include <time.h>
#include <algorithm>
#include <limits>

namespace MCTS
{
	constexpr int infinite = std::numeric_limits<int>::max();

	int MCTS::search(const state& s, const int remaining)
	{
		turns_remaining = remaining;
		NodeData node_data;
		connect4::copyState(node_data.current_state, s);
		std::shared_ptr<TreeNode> root = std::make_shared<TreeNode>(node_data);
		time_t start = time(0);
		double seconds_since_start = 0.0;
		while(seconds_since_start < 5)
		{
			std::shared_ptr<TreeNode> v0 = treePolicy(root);
			const int delta = defaultPolicy(*v0);
			backPropagate(v0, delta);
			seconds_since_start = difftime(time(0), start);
		}
		int chosen_action = bestChild(root, 0.0)->data.action;
		return chosen_action;
	}

	std::shared_ptr<TreeNode> MCTS::bestChild(const std::shared_ptr<TreeNode>& root_node, const double c)
	{
		const int root_visited = root_node->data.visited;
		auto best = root_node->children.front();
		double best_score = 0;
		for (const auto& leaf: root_node->children)
		{
			double score = double(leaf->data.reward) / double(leaf->data.visited);
			if (c > 0.0)
			{
				const auto q = 2.0 * (std::log1p(double(root_visited)));
				score += c * std::sqrt(q / double(leaf->data.visited));
			}
			if (score > best_score)
			{
				best = leaf;
				best_score = score;
			}
		}
		return best;
	}

	std::shared_ptr<TreeNode> MCTS::treePolicy(std::shared_ptr<TreeNode>& root)
	{
		std::shared_ptr<TreeNode> v = root;
		while (not isNodeTerminal(*v))
		{
			if (not v->data.fully_expanded)
				return expand(v);
			else
			{
				v = bestChild(v, 1.0 / sqrt(2.0));
			}
		}
		return v;
	}

	int MCTS::chooseRandomAction(const std::vector<int>& available_actions)
	{
		if (available_actions.size() == 0)
		{
			std::cout << "Something's wrong. 0 actions available to choose";
			std::abort();
		}
		if (available_actions.size() == 1)
			return available_actions[0];
		else
		{
			const int limit = available_actions.size();
			const int random_choice = std::rand() % limit; //from std::rand docs
			return available_actions[random_choice];
		}
	}

	std::shared_ptr<TreeNode> MCTS::expand(std::shared_ptr<TreeNode>& v)
	{
		//choose action 'a' from untried actions
		std::vector<int> actions = connect4::getAvailableMoves(v->data.current_state);
		std::vector<int> available_actions;
		for (unsigned int i = 0; i < actions.size(); i++)
		{
			const auto itr = std::find_if(std::begin(v->children), std::end(v->children), [&](const std::shared_ptr<TreeNode>& obj)
				{ return obj->data.action == actions[i]; });
			const auto found = itr != std::end(v->children);
			if(!found)
				available_actions.push_back(actions[i]);
		}
		//create new leaf node v' of v with action 'a'
		NodeData n_data;
		int a = chooseRandomAction(available_actions);
		n_data.action = a;
		n_data.players_turn = !v->data.players_turn;
		connect4::copyState(n_data.current_state, v->data.current_state);
		connect4::dropCoin(n_data.current_state, n_data.action, getCoin(n_data.players_turn));
		//add node to tree and return new node
		std::shared_ptr<TreeNode> n = v->addChild(n_data);
		n->data.terminal = isNodeTerminal(*n);
		v->data.fully_expanded = v->children.size() == actions.size();
		return n;
	}

	int MCTS::defaultPolicy(const TreeNode& v0)
	{
		TreeNode v = v0;
		std::vector<int> moves;
		while (not v.data.terminal)
		{
			v.data.players_turn = !v.data.players_turn;
			std::vector<int> available_actions = connect4::getAvailableMoves(v.data.current_state);
			if (available_actions.size() == 0)
			{
				//connect4::display(v.data.current_state);
				v.depth = turns_remaining;
				break;
			}
			const int action = chooseRandomAction(available_actions);
			moves.push_back(action);
			const auto coin = getCoin(v.data.players_turn);
			connect4::dropCoin(v.data.current_state, action, coin);
			v.depth += 1;
			v.data.terminal = isNodeTerminal(v);
		}
		const bool node_win = (v0.data.players_turn == v.data.players_turn); //
		const int reward = (turns_remaining - v.depth);
		const bool draw = (reward == 0);
		if (draw)
			return 0;
		else
			return node_win ? reward : -reward;
	}
	}

	void MCTS::backPropagate(std::shared_ptr<TreeNode>& node, int delta)
	{
		node->data.visited += 1;
		node->data.reward += delta;
		if(node->hasParent)
		{
			backPropagate(node->parent, -delta);
		}
		return;
	}

	bool MCTS::isNodeTerminal(const TreeNode& leaf_node)
	{
		if (leaf_node.depth == (turns_remaining-1)) //draw condition
			return true;
		else
		{
			const char coin = getCoin(leaf_node.data.players_turn);
			return connect4::checkWinner(leaf_node.data.current_state, coin);
		}
	}
}