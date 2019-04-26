//
// Created by mike on 4/24/19.
//

#ifndef NEAT_GENETRACKER_H
#define NEAT_GENETRACKER_H
#include <map>
#include <algorithm>
#include <iostream>
#include <string>

class GeneTracker {
private:
    int m_innovationCount = 0;
	std::map<std::pair<int, int>, int> m_innovationMap;
public:
    int getInnovationId(int sourceId, int destinationId){
		if (sourceId == destinationId) {
			throw std::runtime_error("Looping gene! " + std::to_string(sourceId) + "->" + std::to_string(destinationId));
		}
		auto pair = m_innovationMap.find(std::make_pair(sourceId, destinationId));
		if (pair == m_innovationMap.end())
		{
			m_innovationCount++;
			m_innovationMap.insert(std::make_pair(std::make_pair(sourceId, destinationId), m_innovationCount));
			return m_innovationCount;
		}
		else {
			return pair->second;
		}
    };

	int getCurrent() {
		return m_innovationCount;
	}

	void print() {
		for (auto pair : m_innovationMap)
		{
			std::cout << pair.first.first << ", " << pair.first.second << ": " << pair.second << std::endl;
		}
	}
};


#endif //NEAT_GENETRACKER_H
