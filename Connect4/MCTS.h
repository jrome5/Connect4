#pragma once
#ifndef  MCTS_H
#define MCTS_H
#include <vector>
#include "tree.h"
#include "connect4.h"

namespace MCTS
{
	typedef connect4::Board state;

	class MCTS
	{
	public:
		MCTS();

		~MCTS();

		/*
		* @brief MCTS search
		* @param state of the game
		* @return next action
		*/
		int search(const state& s);

	private:

		std::shared_ptr<TreeNode> bestChild(const std::shared_ptr<TreeNode>& root_node, const double c);

		/*
		* @brief Select or create a node from the nodes in the tree
		* @param root_node	
		* @return new or existing node from tree
		*/
		std::shared_ptr<TreeNode> treePolicy(std::shared_ptr<TreeNode>& root);

		int chooseRandomAction(const std::vector<int>& available_actions);
		
		std::shared_ptr<TreeNode> expand(std::shared_ptr<TreeNode>& v);
		
	
		/*
		* @brief Play out the domain from a terminal state to produce an estimate
		* @param initial_state	state of the game
		* @param v				node 
		* @return reward for state
		*/
		int defaultPolicy(const TreeNode& v0);

		////UCT backup with two players
		void backPropagate(std::shared_ptr<TreeNode>& node, int delta);
		
		bool isNodeTerminal(const TreeNode& leaf_node);
		
		char getCoin(const bool players_turn)
		{
			return players_turn ? connect4::player_coin : connect4::computer_coin;
		}

		int computational_limit = 5000;
		int turns_remaining = 42;
	};
}
#endif // ! MCTS_H
