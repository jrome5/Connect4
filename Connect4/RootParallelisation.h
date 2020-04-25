#pragma once
#include "MCTS.h"

namespace MCTS
{
	void* threadFunction(void* i);

	class RootParallelisation : public MCTS
	{
	public:
		int search(const state& s, const int remaining);

		void threadSearch(std::shared_ptr<TreeNode> root, const int remaining, const int random_seed);

	private:
		std::shared_ptr<TreeNode> createMasterTree();

	};
}