#include "LeafParallelisation.h"
#include <pthread.h>
#include <numeric>

#define NUM_THREADS 4

namespace MCTS
{
	int simulation_results;
	std::shared_ptr<TreeNode> leaf_node;

	void* threadRollout(void* args)
	{
		srand((unsigned)time(0) + int(args));
		TreeNode v = *leaf_node;
		while (not v.data.terminal)
		{
			std::vector<int> available_actions = v.data.current_state.getAvailableMoves();
			if (available_actions.size() == 0)
			{
				return 0;
			}
			const int action = chooseRandomAction(available_actions);
			v = *v.addChild(action, false);
			v.data.terminal = (v.data.terminal or v.depth == MAX_TURNS);
		}
		const bool node_win = (leaf_node->data.players_turn == v.data.players_turn);
		simulation_results += calculateReward(v, node_win);
		v.remove();
		return NULL;
	}

	float LeafParallelisation::defaultPolicy(const std::shared_ptr<TreeNode> v0)
	{
		leaf_node = v0;
		simulation_results = 0;
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
			rc = pthread_create(&threads[i], &attr, threadRollout, (void*) i);
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
		//std::cout << simulation_results << std::endl;
		const float average_reward = simulation_results / NUM_THREADS;
		delete attr;
		delete status;
		leaf_node.reset();
		return average_reward;
	}
}