#pragma once
#include "MCTS.h"
#include "tree.h"
#include <shared_mutex>
namespace MCTS
{
	namespace LocalMutexMCTS
	{
		/*
		* @brief MCTS search
		* @param state of the game
		* @return next action
		*/
		int search(const state& s, const int turn);

		std::shared_ptr<MutexNode> bestChild(const std::shared_ptr<MutexNode>& root_node, const double c);

		/*
		* @brief Select or create a node from the nodes in the tree
		* @param root_node
		* @return new or existing node from tree
		*/
		std::shared_ptr<MutexNode> treePolicy(std::shared_ptr<MutexNode>& root);

		std::shared_ptr<MutexNode> expand(std::shared_ptr<MutexNode>& v);


		/*
		* @brief Play out the domain from a terminal state to produce an estimate
		* @param initial_state	state of the game
		* @param v				node
		* @return reward for state
		*/
		float defaultPolicy(const std::shared_ptr<TreeNode> v0);

		////UCT backup with two players
		void backPropagate(std::shared_ptr<MutexNode>& node, float delta);

	}
}