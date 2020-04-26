#include "MCTS.h"
#include <cmath>
#include <iostream>
#include <algorithm>
#include <limits>
#include <time.h>

namespace MCTS
{
	constexpr int infinite = std::numeric_limits<int>::max();

	inline void outputDistribution(const TreeNode& root)
	{
		std::cout << std::endl << "Root stats: N = " << root.data.visited << " Q: " << root.data.reward << std::endl;
		const auto N = double(root.data.visited);
		for (const auto& child : root.children)
		{
			const auto percent = 100* double(child->data.visited) / N;
			const auto avg = double(child->data.reward) / double(child->data.visited);
			std::cout << child->data.action << "- Visited: " << percent << "% Avg: " << avg << std::endl;
		}
	}

	int MCTS::search(const state& s, const int remaining)
	{
		turns_remaining = remaining;
		srand((unsigned)time(0));
		NodeData node_data;
		node_data.current_state = s;
		std::shared_ptr<TreeNode> root = std::make_shared<TreeNode>(node_data);
		const int computational_limit = 10000;
		int iterations = 0;
		while(iterations < computational_limit)
		{
			std::shared_ptr<TreeNode> v0 = treePolicy(root);
			const int delta = defaultPolicy(v0);
			backPropagate(v0, delta);
			iterations++;
		}
		const int chosen_action = bestChild(root, 0.0)->data.action;
//		outputDistribution(*root);
		root->remove();
		return chosen_action;
	}

	std::shared_ptr<TreeNode> MCTS::bestChild(const std::shared_ptr<TreeNode>& root_node, const double c)
	{
		const int root_visited = root_node->data.visited;
		if (root_node->children.size() == 0)
		{
			std::abort();
		}
		auto best = root_node->children.front();
		double best_score = -infinite;
		for (const auto& child: root_node->children)
		{
			double score = double(child->data.reward) / double(child->data.visited);
			if (c > 0.0)
			{
				const auto q = 2.0 * log(double(root_visited));
				score += c * std::sqrt(q / double(child->data.visited));
			}
			if (score > best_score)
			{
				best = child;
				best_score = score;
			}
		}
		return best;
	}

	std::shared_ptr<TreeNode> MCTS::treePolicy(std::shared_ptr<TreeNode>& root)
	{
		std::shared_ptr<TreeNode> v = root;
		while (not v->data.terminal)
		{
			if (not v->data.fully_expanded)
			{
				return expand(v);
			}
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
			return available_actions.front();
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
		std::vector<int> actions = v->data.current_state.getAvailableMoves();
		std::vector<int> available_actions;
		for (auto i = 0; i < actions.size(); i++)
		{
			const auto itr = std::find_if(v->children.cbegin(), v->children.cend(), [&](const std::shared_ptr<TreeNode>& obj)
				{ return obj->data.action == actions[i]; });
			const auto found = itr != v->children.cend();
			if(!found)
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
			std::shared_ptr<TreeNode> n = v->addChild(chooseRandomAction(available_actions));
			n->data.terminal = (n->data.terminal or n->depth == turns_remaining);
			v->data.fully_expanded = v->children.size() == actions.size();
			return n;
		}
	}

	int MCTS::defaultPolicy(const std::shared_ptr<TreeNode> v0)
	{
		auto v = *v0;
		std::vector<int> moves;
		while (not v.data.terminal)
		{
			v.data.players_turn = !v.data.players_turn;
			std::vector<int> available_actions = v.data.current_state.getAvailableMoves();
			if (available_actions.size() == 0)
			{
				return 0;
			}
			const int action = chooseRandomAction(available_actions);
			v = *v.addChild(action, false);
			v.data.terminal = (v.data.terminal or v.depth == turns_remaining);
		}
		const bool node_win = (v0->data.players_turn == v.data.players_turn); 
		return calculateReward(v, node_win);
	}

	void MCTS::backPropagate(std::shared_ptr<TreeNode>& node, int delta)
	{
		node->data.visited += 1;
		node->data.reward += delta;
		for(auto& parent : node->parents)
		{
			backPropagate(parent, -delta);
		}
		return;
	}

	int MCTS::calculateReward(const TreeNode leaf, const bool node_win)
	{
		const bool draw = (turns_remaining == leaf.depth);
		if (draw)
			return 0;
		else
			return node_win ? 1 : -1;
	}
}