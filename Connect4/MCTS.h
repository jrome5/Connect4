#pragma once
#include <vector>
#include "connect4.h"
#include "tree.h"
#include <cmath>
#include <iostream>
#ifndef  MCTS_H
#define MCTS_H

namespace MCTS
{
	typedef connect4::Board state;
	typedef int action;

	class MCTS
	{
	public:
		MCTS(){};
		~MCTS(){};

		/*
		* @brief MCTS search.
		* @param state of the game
		* @return next action
		*/
		action search(const action player_action)
		{
			state s;
			connect4::initializeBoard(s);
			connect4::dropCoin(s, player_action, getCoin(true));
			NodeData node_data;
			node_data.action = player_action;
			connect4::copyState(node_data.current_state, s);
			TreeNode* node = traversal_tree.CreateNode(node_data);
			traversal_tree.setRoot(node);
			int n = 0;
			//while (n < computational_limit)
			//{
				TreeNode v0 = treePolicy(traversal_tree);
				auto delta = defaultPolicy(v0);
				backPropagate(traversal_tree, v0, delta);
				return delta;
				//n++;
			//}
			//return bestChild(tree.getRoot(), 0).data.action;
		}

	private:

		TreeNode bestChild(const TreeNode& root_node, float c)
		{
			int root_visited = root_node.data.visited;
			TreeNode best;
			double best_score = 0;
			for (auto leaf : root_node.children)
			{
				double score = (leaf->data.reward / leaf->data.visited);
				if (c != 0)
					score += c * std::sqrt(2 * std::log(root_visited / leaf->data.visited));
				if (score > best_score)
				{
					best = *leaf;
				}
			}
			return best;
		}

		/*
		* @brief Select or create a node from the nodes in the tree
		* @param root_node	
		* @return new or existing node from tree
		*/
		TreeNode treePolicy(Tree& tree)
		{
			TreeNode& v = tree.getRoot();
			while (not v.data.terminal)
			{
				if (not v.data.fully_expanded)
					return expand(tree, v);
				else
					v = bestChild(v, 1.0f / sqrt(2.0f));
			}
			return v;
		}

		//void calculateState(state& s, TreeNode leaf_node)
		//{
		//	//construct list of actions root->leaf
		//	//proceed to take actions
		//	//return state
		//	std::cout << leaf_node.data.action;
		//	std::vector<action> actions;
		//	actions.push_back(leaf_node.data.action);
		//	if (leaf_node.hasParent)
		//	{
		//		TreeNode* root = leaf_node.parent;
		//		while (true)
		//		{
		//			actions.push_back(root->data.action);
		//			if (root->hasParent)
		//				root = root->parent;
		//			else
		//				break;
		//		}
		//	}
		//	connect4::initializeBoard(s);
		//	bool is_players_turn = true;
		//	while (actions.size())
		//	{
		//		char coin = getCoin(is_players_turn);
		//		action choice = actions.back();
		//		actions.pop_back();
		//		connect4::dropCoin(s, choice, coin);
		//		is_players_turn = !is_players_turn;
		//	}

			//connect4::display(s);
		//}

		action chooseRandomAction(const std::vector<action> available_actions)
		{
			if (available_actions.size() == 0)
			{
				std::cout << "Somethings wrong. 0 actions available to choose";
				return -1;
			}
			const int random_choice = rand() % available_actions.size();
			return available_actions[random_choice];
		}
		
		TreeNode expand(Tree& tree, TreeNode& v)
		{
			//choose action 'a' from untried actions
			std::vector<action> available_actions = connect4::getAvailableMoves(v.data.current_state);
			for (auto& child : v.children)
				for (int i = 0; i < available_actions.size(); i++)
					if (child->data.action = available_actions[i])
						available_actions.erase(available_actions.begin() + i);
			//create new leaf node v' of v with action 'a'
			TreeNode n;
			action a = chooseRandomAction(available_actions);
			n.data.action = a;
			const bool player_turn = (v.depth + 1) % 2 == 0;
			connect4::copyState(n.data.current_state, v.data.current_state);
			connect4::dropCoin(n.data.current_state, a, getCoin(player_turn));
			//add node to tree and return new node
			isNodeTerminal(n);
			tree.InsertNode(&v, &n);
			return n;
		}
	
		/*
		* @breif Play out the domain from a terminal state to produce an estimate
		* @param initial_state	state of the game
		* @param v				node 
		* @return reward for state
		*/
		int defaultPolicy(const TreeNode v0)
		{
			/*
			function DEFAULTPOLICY(s) 
			while s is non-terminal do 
			choose a = A(s) uniformly at random 
			s <- f(s,a) 
			return reward for state s
			*/
			TreeNode v = v0;
			bool is_terminal = false;
			while(not v.data.terminal)
			{
				v.data.players_turn = !v.data.players_turn;
				std::vector<action> available_actions = connect4::getAvailableMoves(v.data.current_state);
				v.data.action = chooseRandomAction(available_actions);
				auto coin = getCoin(v.data.players_turn);
				connect4::dropCoin(v.data.current_state, v.data.action, coin);
				v.depth++;
				v.data.terminal = isNodeTerminal(v);
				std::cout << v.data.action << " " << v.depth << std::endl;
			}
			std::cout << "terminal node reached";
			connect4::display(v.data.current_state);
			std::cout << "last turn " << v.data.action;
			if (v.depth == 42) //draw
				return 0;
			else
			{
				const bool player_win = (v.depth % 2) == 0;
				std::cout << player_win;
				return player_win;
			}
		}

		void backPropagate(Tree& tree, TreeNode& leaf_node, const int delta)
		{
			
		}

		int calculateReward(const bool player_win, const TreeNode leaf_node)
		{
			if (player_win != leaf_node.data.players_turn) //XOR
				return -1*player_win;
			return player_win;
		}

		bool isNodeTerminal(TreeNode& leaf_node)
		{
			if (leaf_node.depth == 42) //draw condition
				return true;
			const char coin = getCoin(leaf_node.data.players_turn);
			return  connect4::checkWinner(leaf_node.data.current_state, coin);
		}

		char getCoin(const bool players_turn)
		{
			return players_turn ? connect4::player_coin : connect4::computer_coin;
		}
		
	private:
		int computational_limit = 500;
		Tree traversal_tree;
		int number_of_turns = 0;
	};
}
#endif // ! MCTS_H
