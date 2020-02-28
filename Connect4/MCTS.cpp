#include "MCTS.h"
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <ctime>

namespace MCTS
{
	MCTS::MCTS()
	{
	}
	MCTS::~MCTS()
	{
	}

	int MCTS::search(const state& s)
	{
		turns_remaining--;
		NodeData node_data;
		connect4::copyState(node_data.current_state, s);
		auto root = std::make_shared<TreeNode>(node_data);
		int num = 0;
		while (num < computational_limit)
		{
			std::shared_ptr<TreeNode> v0 = treePolicy(root);
			int delta = defaultPolicy(*v0);
			backPropagate(v0, delta);
			num++;
		}
		int chosen_action = bestChild(root, 0.0)->data.action;
		turns_remaining--;
		return chosen_action;
	}

	std::shared_ptr<TreeNode> MCTS::bestChild(const std::shared_ptr<TreeNode>& root_node, const double c)
	{
		int root_visited = root_node->data.visited;
		int best = 0;
		double best_score = 0.0f;
		for (unsigned int i = 0; i < root_node->children.size(); i++)
		{
			std::shared_ptr<TreeNode> leaf = root_node->children[i];
			double score = double(leaf->data.reward) / double(leaf->data.visited);
			if (c > 0.0f)
				score += c * std::sqrt(2*double(std::log1p(root_visited / double(leaf->data.visited))));
			if (score >= best_score)
			{
				best = i;
			}
		}
		return root_node->children[best];
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
			bool found = false;
			for (const auto& child : v->children)
			{
				if (child->data.action == actions[i])
				{
					found = true;
					break;
				}
			}
			if (!found)
				available_actions.push_back(actions[i]);
		}
		//create new leaf node v' of v with action 'a'
		NodeData n_data;
		int a = chooseRandomAction(available_actions);
		n_data.action = a;
		const bool player_turn = ((v->depth + 1) % 2 == 0);
		connect4::copyState(n_data.current_state, v->data.current_state);
		connect4::dropCoin(n_data.current_state, n_data.action, getCoin(player_turn));
		//add node to tree and return new node
		std::shared_ptr<TreeNode> n = v->addChild(n_data);
		n->data.players_turn = player_turn;
		n->data.terminal = isNodeTerminal(*n);
		v->data.fully_expanded = v->children.size() == actions.size();
		return n;
	}

	int MCTS::defaultPolicy(const TreeNode& v0)
	{
		std::shared_ptr<TreeNode> v = std::make_shared<TreeNode>(v0.data);
		v->data.players_turn = v0.data.players_turn;
		v->depth = v0.depth;
		int i = v->depth;
		bool draw = false;
		while (not v->data.terminal)
		{
			v->data.players_turn = !v->data.players_turn;
			std::vector<int> available_actions = connect4::getAvailableMoves(v->data.current_state);
			if (available_actions.size() == 0)
			{
				draw = true;
				break;
			}
			v->data.action = chooseRandomAction(available_actions);
			auto coin = getCoin(v->data.players_turn);
			connect4::dropCoin(v->data.current_state, v->data.action, coin);
			v->depth++;
			v->data.terminal = isNodeTerminal(*v);
		}
		if (draw) //draw
			return 0;
		else
		{
			const bool player_win = (v->depth % 2) == 0;
			return player_win ? -1 : 1;
		}
	}

	void MCTS::backPropagate(std::shared_ptr<TreeNode>& node, int delta)
	{
		std::shared_ptr<TreeNode> v = node;
		while (v->hasParent)
		{
			v->data.visited += 1;
			v->data.reward += delta;
			delta *= -1;
			v = v->parent;
		}
	}

	bool MCTS::isNodeTerminal(const TreeNode& leaf_node)
	{
		if (leaf_node.depth >= turns_remaining) //draw condition
			return true;
		else
		{
			const char coin = getCoin(leaf_node.data.players_turn);
			return connect4::checkWinner(leaf_node.data.current_state, coin);
		}
	}
}