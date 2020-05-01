#include <cmath>
#include <iostream>
#include <algorithm>
#include <limits>
#include <time.h>
#include <stdlib.h>
#include "LocalMutexMCTS.h"
#include <pthread.h>

#define NUM_THREADS 4

namespace MCTS
{
	namespace LocalMutexMCTS
	{
		constexpr int infinite = std::numeric_limits<int>::max();
		std::shared_ptr<MutexNode> root;

		void lock(std::shared_ptr<MutexNode>& node)
		{
			node->mutex.lock();
			//node->data.addVirtualLoss();
		}

		void unlock(std::shared_ptr<MutexNode>& node)
		{
			//node->data.removeVirtualLoss();
			node->mutex.unlock();
		}

		void* threadFunction(void *args)
		{
			const int computational_limit = 10000;
			int iterations = 0;
			while (iterations < computational_limit)
			{
				std::shared_ptr<MutexNode> v0 = treePolicy(root);
				auto vt = std::make_shared<TreeNode>(v0->data);
				vt->depth = v0->depth;
				const auto delta = defaultPolicy(vt);
				vt.reset();
				backPropagate(v0, delta);
				iterations++;
			}

			return NULL;
		}

		int search(const state& s, const int turn)
		{
			srand((unsigned)time(0));
			MCTS mcts;
			NodeData node_data;
			node_data.current_state = s;
			root = std::make_shared<MutexNode>(node_data);
			root->depth = turn;
			
			//create threads
			pthread_t threads[NUM_THREADS];
			pthread_attr_t attr;
			int rc;
			void* status;	
			// Initialize and set thread joinable
			pthread_attr_init(&attr);
			pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
			//create threads and run tree searches
			for (int i = 0; i < NUM_THREADS; i++)
			{
				rc = pthread_create(&threads[i], &attr, threadFunction, (void*)i);
				if (rc) {
					std::cout << "Error: unable to create thread," << rc << std::endl;
					exit(-1);
				}
			}
			for (int i = 0; i < NUM_THREADS; i++)
			{
				rc = pthread_join(threads[i], &status);
				if (rc) {
					std::cout << "Error:unable to join," << rc << std::endl;
					exit(-1);
				}
			}

			const int chosen_action = bestChild(root, 0.0)->data.action;
			//		outputDistribution(*root);
			root->remove();
			return chosen_action;
		}

		std::shared_ptr<MutexNode> bestChild(const std::shared_ptr<MutexNode>& root, const double c)
		{
			const int root_visited = root->data.visited;
			if (root->children.size() == 0)
			{
				std::abort();
			}
			auto best = root->children.front();
			double best_score = -infinite;
			for (const auto& child : root->children)
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

		std::shared_ptr<MutexNode> treePolicy(std::shared_ptr<MutexNode>& root)
		{
			std::shared_ptr<MutexNode> v = root;
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

		std::shared_ptr<MutexNode> expand(std::shared_ptr<MutexNode>& v)
		{
			lock(v);
			//choose action 'a' from untried actions
			std::vector<int> actions = v->data.current_state.getAvailableMoves();
			std::vector<int> available_actions;
			for (auto i = 0; i < actions.size(); i++)
			{
				const auto itr = std::find_if(v->children.cbegin(), v->children.cend(), [&](const std::shared_ptr<MutexNode>& obj)
					{ return obj->data.action == actions[i]; });
				const auto found = itr != v->children.cend();
				if (!found)
					available_actions.push_back(actions[i]);
			}
			if (available_actions.size() == 0)
			{
				v->data.fully_expanded = true;
				unlock(v);
				return v;
			}
			else
			{
				//create new leaf node v' of v with action 'a'
				//add node to tree and return new node
				std::shared_ptr<MutexNode> n = v->addChild(chooseRandomAction(available_actions));
				n->data.terminal = (n->data.terminal or n->depth == MAX_TURNS);
				v->data.fully_expanded = v->children.size() == actions.size();
				unlock(v);
				return n;
			}
		}

		float defaultPolicy(const std::shared_ptr<TreeNode> v0)
		{
			auto v = *v0;
			while (not v.data.terminal)
			{
				std::vector<int> available_actions = v.data.current_state.getAvailableMoves();
				if (available_actions.size() == 0)
				{
					return 0;
				}
				const int action = chooseRandomAction(available_actions);
				v.addSimChild(action);
				v.data.terminal = (v.data.terminal or v.depth == MAX_TURNS);
			}
			const bool node_win = (v0->data.players_turn == v.data.players_turn);
			return calculateReward(v, node_win);
		}

		void backPropagate(std::shared_ptr<MutexNode>& node, float delta)
		{
			lock(node);
			node->data.visited += 1;
			node->data.reward += delta;
			unlock(node);
			if (node->parent)
			{
				backPropagate(node->parent, -delta);
			}
			return;
		}
	} // end namespace LocalMutexMCTS
} //end namespace MCTS