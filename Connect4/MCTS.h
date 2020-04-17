#pragma once
#include "connect4.h"
#include "tree.h"
#include <vector>

namespace MCTS
{
	typedef connect4::Board state;

	class MCTS
	{
	public:

		MCTS() {};

		virtual ~MCTS() {};
		/*
		* @brief MCTS search
		* @param state of the game
		* @return next action
		*/
		int search(const state& s, const int remaining);

	protected:

		std::shared_ptr<TreeNode> bestChild(const std::shared_ptr<TreeNode>& root_node, const double c);

		/*
		* @brief Select or create a node from the nodes in the tree
		* @param root_node	
		* @return new or existing node from tree
		*/
		std::shared_ptr<TreeNode> treePolicy(std::shared_ptr<TreeNode>& root);
		
		std::shared_ptr<TreeNode> expand(std::shared_ptr<TreeNode>& v);
		
	
		/*
		* @brief Play out the domain from a terminal state to produce an estimate
		* @param initial_state	state of the game
		* @param v				node 
		* @return reward for state
		*/
		float defaultPolicy(const std::shared_ptr<TreeNode> v0);

		////UCT backup with two players
		void backPropagate(std::shared_ptr<TreeNode>& node, float delta);

		int turns_remaining;
	};

	int calculateReward(const TreeNode leaf, const bool node_win, const int turns_remaining);

	int chooseRandomAction(const std::vector<int>& available_actions);
}