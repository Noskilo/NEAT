//
// Created by mike on 4/24/19.
//

#ifndef NEAT_SPECIES_H
#define NEAT_SPECIES_H

#include <map>
#include "Genome.h"
#include <algorithm>

struct CompareFitness
{
    bool const operator()(Genome *lhs, Genome *rhs) const
    {
        return lhs->getFitness() < rhs->getFitness();
    }
};

class Species {
private:
    std::vector<Genome *> m_genomes;
    Genome m_mascot;
    float m_totalAdjustedFitness = 0.0f;
	std::vector<float> fitnessHistory;
public:
    Species(Genome &t_mascot) {
        m_mascot = t_mascot;
        m_genomes.push_back(&t_mascot);
    };

    void addAdjustedFitness(float t_adjustedFitness);

    void electMascot(RandomEngine &t_randomEngine);

    void reset();

    Genome* getFittest();
    void addMember(Genome *t_member);

	void setAdjustedFitness(float t_adjustedFitness);

    int size() {
        return m_genomes.size();
    };

    Genome &getMascot();

    Genome* getRandomGenomeBiasedAdjustedFitness(RandomEngine &t_randomEngine);

    double getAdjustedFitness();
};


#endif //NEAT_SPECIES_H
