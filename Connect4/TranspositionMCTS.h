#pragma once
#include "MCTS.h"
#include <unordered_map>
namespace MCTS
{
	class TranspositionMCTS : public MCTS
	{
	public :
		int search(const state& s, const int remaining) override;
	protected:

		std::shared_ptr<TreeNode> expand(std::shared_ptr<TreeNode>& v) override;

		std::unordered_map<connect4::Board, std::shared_ptr<TreeNode>> transposition_table_;
	};
}
