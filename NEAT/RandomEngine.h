//
// Created by mike on 4/24/19.
//

#ifndef NEAT_RANDOMENGINE_H
#define NEAT_RANDOMENGINE_H

#include <random>
#include <ctime>

class RandomEngine {
private:
	std::default_random_engine m_randomEngine;
public:

	RandomEngine() {
		m_randomEngine.seed(std::time(NULL));
	};

	bool nextBool() {
		std::uniform_int_distribution<int> distribution(0, 1);
		return distribution(m_randomEngine);
	};

	float nextFloat() {
		std::uniform_real_distribution<float> distribution(0, 1);
		return distribution(m_randomEngine);
	};

	int nextInt(int t_range = RAND_MAX) {
		std::uniform_int_distribution<int> distribution(0, t_range > 0 ? t_range - 1 : t_range);
		return distribution(m_randomEngine);
	};

	float randomRange(float from, float to) {
		std::uniform_real_distribution<float> distribution(from, to);
		return distribution(m_randomEngine);
	}
};


#endif //NEAT_RANDOMENGINE_H
