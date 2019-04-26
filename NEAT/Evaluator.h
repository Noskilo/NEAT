//
// Created by mike on 4/24/19.
//

#ifndef NEAT_EVALUATOR_H
#define NEAT_EVALUATOR_H


#include <unordered_map>
#include <vector>
#include "GeneTracker.h"
#include "RandomEngine.h"
#include "Genome.h"
#include "Species.h"

class Evaluator {
private:
    GeneTracker* m_geneTracker;
    RandomEngine m_randomEngine;

    const float C1 = 1.0f;
    const float C2 = 1.0f;
    const float C3 = 0.5f;
    const float DT = 500.0f;
    const float MUTATION_RATE = 0.4f;
    const float ADD_CONNECTION_RATE = 0.2f;
    const float ADD_NODE_RATE = 0.2f;
	const int MAX_NODES = 15;

    int m_populationSize;
    float m_highScore = 0.0f;
    Genome* m_alpha;

    std::vector<Genome *> m_currentGeneration;
    std::vector<Genome *> m_nextGeneration;
    std::vector<Species*> m_species;
    std::unordered_map<Genome*, Species*> m_speciesMap;
	std::map<float, Genome*> fitnessMap;

    float compatibilityDistance(Genome *genome1, Genome *genome2);
    Genome* crossover(Genome* parent1, Genome* parent2);
    Species* getRandomSpeciesBiasedAdjustedFitness();

    virtual float evaluateGenome(Genome* genome) = 0;
public:
    Evaluator(int t_populationSize, Genome &t_seed, GeneTracker *t_geneTracker);

    ~Evaluator();

public:
    void evaluate();


    Genome getAlpha() const;
    int speciesCount() const;
    float highScore() const;

};


#endif //NEAT_EVALUATOR_H
