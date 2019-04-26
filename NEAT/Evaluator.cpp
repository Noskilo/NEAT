//
// Created by mike on 4/24/19.
//

#include <algorithm>
#include "Evaluator.h"
#include <string>
#include "Stopwatch.h"

Evaluator::Evaluator(int t_populationSize, Genome& t_seed, GeneTracker* t_geneTracker) {

	m_geneTracker = t_geneTracker;
	m_populationSize = t_populationSize;
	m_alpha = nullptr;
	for (int i = 0; i < t_populationSize; i++) {
		m_currentGeneration.push_back(t_seed.clone());
	}

}

Evaluator::~Evaluator() {
	for (auto& genome : m_currentGeneration) {
		delete genome;
		genome = nullptr;
	}
	m_currentGeneration.clear();

	for (auto& genome : m_nextGeneration) {
		delete genome;
		genome = nullptr;
	}
	m_nextGeneration.clear();
}

float Evaluator::compatibilityDistance(Genome* genome1, Genome* genome2) {

	int excessGenes = 0;
	int disjointGenes = 0;
	float weightDifference = 0;
	int matchingGenes = 0;
	int N = std::max(genome1->getGenes().size(), genome2->getGenes().size());


	int highestInnovation1 = genome1->lastGeneInnovation();
	int highestInnovation2 = genome2->lastGeneInnovation();

	int indices = std::max(highestInnovation1, highestInnovation2);

	for (int i = 1; i <= indices; i++) {
		bool gene1Exists = genome1->hasGene(i);
		bool gene2Exists = genome2->hasGene(i);

		if (!gene1Exists && highestInnovation1 < i && gene2Exists) {
			excessGenes++;
		}
		else if (!gene2Exists && highestInnovation2 < i && gene1Exists) {
			excessGenes++;
		}

		if (!gene1Exists && highestInnovation1 > i && gene2Exists) {
			disjointGenes++;
		}
		else if (!gene2Exists && highestInnovation2 > i && gene1Exists) {
			disjointGenes++;
		}

		if (gene1Exists && gene2Exists) {
			matchingGenes++;
			weightDifference += std::abs(genome1->getGene(i).getWeight() - genome2->getGene(i).getWeight());
		}
	}

	return (float)((excessGenes * C1) / N + (disjointGenes * C2) / N + (weightDifference / matchingGenes) * C3);
}

Genome* Evaluator::crossover(Genome * parent1, Genome * parent2) {

	Genome* child = new Genome();
	Genome* strong = nullptr;
	Genome* weak = nullptr;

	if (parent1->getFitness() > parent2->getFitness()) {
		strong = parent1;
		weak = parent2;
	}
	else if (parent1->getFitness() < parent2->getFitness()) {
		strong = parent2;
		weak = parent1;
	}
	else {
		strong = parent1;
		weak = parent2;
	}

	for (auto& pair : strong->getNodes()) {
		child->addNode(pair.second);
	}

	for (auto& pair : strong->getGenes()) {
		if (weak->hasGene(pair.second.getId())) {
			if (m_randomEngine.nextBool()) {
				child->addGene(pair.second);
			}
			else {
				child->addGene(weak->getGene(pair.second.getId()));
			}
		}
		else {
			child->addGene(pair.second);
		}
	}

	return child;
}

void Evaluator::evaluate() {

	for (Species* s : m_species) {
		s->reset();
	}

	m_highScore = 0.0f;
	m_speciesMap.clear();
	fitnessMap.clear();
	delete m_alpha;
	m_alpha = nullptr;

	for (Genome* genome : m_currentGeneration) {
		bool foundSpecies = false;
		for (Species* s : m_species) {
			if (genome->compatibility(s->getMascot(), C1, C2, C3) < DT) {
				s->addMember(genome);
				m_speciesMap.insert({ genome, s });
				foundSpecies = true;
				break;
			}
		}

		if (!foundSpecies) {
			Species* newSpecies = new Species(*genome);
			m_species.push_back(newSpecies);
			m_speciesMap.insert({ genome, newSpecies });
		}
	}

	for (auto& s : m_species) {
		if (s->size() == 0) {
			delete s;
			s = nullptr;
		}
	}
	m_species.erase(std::remove(begin(m_species), end(m_species), nullptr),
		end(m_species));

	for (Genome* g : m_currentGeneration) {
		Species* s = m_speciesMap.at(g);

		float score = evaluateGenome(g);
		fitnessMap.insert({ score, g });
		float adjustedScore = score / s->size();
		g->setFitness(adjustedScore);
		s->addAdjustedFitness(adjustedScore);

		if (score > m_highScore || m_alpha == nullptr) {
			m_highScore = score;
			m_alpha = g->clone();
		}
	}

	float minFitness = fitnessMap.begin()->first;
	float maxFitness = fitnessMap.rbegin()->first;
	float fitnessRange = std::max(1.0f, maxFitness  -  minFitness);



	for (Species* s : m_species) {
		s->setAdjustedFitness((s->getAdjustedFitness() - minFitness) / fitnessRange);
		s->electMascot(m_randomEngine);
		m_nextGeneration.push_back(s->getFittest()->clone());
	}

	
	while (m_nextGeneration.size() < m_populationSize) { // replace removed genomes by randomly breeding
		Species* s = getRandomSpeciesBiasedAdjustedFitness();

		Genome* p1 = s->getRandomGenomeBiasedAdjustedFitness(m_randomEngine);
		Genome* p2 = s->getRandomGenomeBiasedAdjustedFitness(m_randomEngine);

		Genome* child = crossover(p1, p2);

		if (m_randomEngine.nextFloat() < MUTATION_RATE) {
			child->mutate(m_randomEngine);
		}
		if (m_randomEngine.nextFloat() < ADD_CONNECTION_RATE) {
			child->mutateGeneConnection(m_randomEngine, m_geneTracker);
		}
		if (m_randomEngine.nextFloat() < ADD_NODE_RATE && child->getNodes().size() <= MAX_NODES) {
			child->mutateNode(m_randomEngine, m_geneTracker);
		}

		m_nextGeneration.push_back(child);
	}

	for (Genome* g : m_currentGeneration) {
		delete g;
		g = nullptr;
	}
	m_currentGeneration.clear();
	m_currentGeneration = m_nextGeneration;
	m_nextGeneration.clear();
}

Species* Evaluator::getRandomSpeciesBiasedAdjustedFitness() {
	double completeWeight = 0.0;
	for (Species* s : m_species) {
		completeWeight += s->getAdjustedFitness();
	}

	double r = m_randomEngine.nextFloat() * completeWeight;
	double countWeight = 0.0;
	Species* fittestSpecies = m_species[0];
	for (Species* s : m_species) {
		if (s->getAdjustedFitness() > fittestSpecies->getAdjustedFitness()) {
			fittestSpecies = s;
		}
		countWeight += s->getAdjustedFitness();
		if (countWeight >= r) {
			return s;
		}
	}

	return fittestSpecies;
	throw std::runtime_error(
		"WTF! Species count: " + std::to_string(m_species.size()) + " Total adjusted fitness is: " +
		std::to_string(completeWeight));
}

Genome Evaluator::getAlpha() const {
	return *m_alpha;
}

int Evaluator::speciesCount() const {
	return m_species.size();
}

float Evaluator::highScore() const {
	return m_highScore;
}
