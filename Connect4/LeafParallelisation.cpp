#include "LeafParallelisation.h"
#include <pthread.h>
#include <numeric>

#define NUM_THREADS 2

namespace MCTS
{
	std::shared_ptr<TreeNode> leaf_node;
	void* threadRollout(void* i)
	{
		const int turns_remaining = (int)i;
		TreeNode v = *leaf_node;
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
			v.data.terminal = (v.data.terminal or v.depth == MAX_TURNS);
		}
		const bool node_win = (leaf_node->data.players_turn == v.data.players_turn);
		*((int*)i) = calculateReward(v, node_win);
		pthread_exit(i);
	}

	float LeafParallelisation::defaultPolicy(const std::shared_ptr<TreeNode> v0)
	{
		int simulation_results[NUM_THREADS];
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
			rc = pthread_create(&threads[i], &attr, threadRollout, &simulation_results[i]);
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

		const float total_reward = std::accumulate(std::begin(simulation_results), std::end(simulation_results), 0);
		const float average_reward = total_reward / NUM_THREADS;
		return average_reward;
	}
}