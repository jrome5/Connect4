#include "RootParallelisation.h"
#include <cmath>
#include <iostream>
#include <time.h>
#include <algorithm>
#include <limits>
#include <pthread.h>
#include <numeric>

#define NUM_THREADS 4

typedef struct ThreadStruct
{
	int index;
	int remaining;

//	ThreadStruct(const int i, const int r) : index(i), remaining(r) {}
};

namespace MCTS
{
	constexpr int infinite = std::numeric_limits<int>::max();
	std::vector<std::shared_ptr<TreeNode>> search_trees;

	void *threadFunction(void *t)
	{
		ThreadStruct* data = static_cast<ThreadStruct*>(t);
		RootParallelisation cpu;

		cpu.threadSearch(search_trees[data->index], data->remaining, data->index);
		pthread_exit(NULL);
		return NULL;
	}

	int RootParallelisation::search(const state& s, const int remaining)
	{
		//create threads
		pthread_t threads[NUM_THREADS];
		pthread_attr_t attr;
		int rc;
		void* status;
		//create trees
		turns_remaining = remaining;
		NodeData node_data;
		node_data.current_state = s;
		// Initialize and set thread joinable
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
		//create threads and run tree searches
		search_trees.reserve(NUM_THREADS);
		ThreadStruct thread_data[NUM_THREADS];
		for (int i = 0; i < NUM_THREADS; i++)
		{
			std::shared_ptr<TreeNode> thread_root = std::make_shared<TreeNode>(node_data);
			search_trees.push_back(thread_root);
			thread_data[i].index = i;
			thread_data[i].remaining = remaining;
			rc = pthread_create(&threads[i], &attr, threadFunction, &thread_data[i]);
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

		//combine trees
		std::shared_ptr<TreeNode> master_tree = createMasterTree();
		int chosen_action = bestChild(master_tree, 0.0)->data.action;
		//free up memory
		master_tree->remove();
		return chosen_action;
	}

	std::shared_ptr<TreeNode> RootParallelisation::createMasterTree()
	{
		std::shared_ptr<TreeNode> master_tree = search_trees.front();
		//SORT THIS
		//iterate trees
		for (auto i = 1; i < search_trees.size(); i++)
		{
			for (auto& thread_child : search_trees[i]->children)
			{
				for (auto& master_child : master_tree->children)
				{
					if (master_child->data.action == thread_child->data.action)
					{
						master_child->data.reward += thread_child->data.reward;
						master_child->data.visited += thread_child->data.visited;
						break;
					}
				}
			}
			search_trees[i]->remove();
		}
		search_trees.clear();
		return master_tree;
	}

	void RootParallelisation::threadSearch(std::shared_ptr<TreeNode> root, const int remaining, const int random_seed)
	{
		turns_remaining = remaining;
		srand((unsigned)time(0) + random_seed);
		const int computational_limit = 10000;
		int num = 0;
		while (num < computational_limit)
		{
			std::shared_ptr<TreeNode> v0 = treePolicy(root);
			const int delta = defaultPolicy(v0);
			backPropagate(v0, delta);
			num++;
		}
	}
}