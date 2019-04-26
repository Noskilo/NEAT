//
// Created by mike on 4/24/19.
//

#include "Species.h"
#include <string>

void Species::reset() {
    m_genomes.clear();
    m_totalAdjustedFitness = 0.0f;
}

void Species::electMascot(RandomEngine &t_randomEngine) {
    m_mascot = *m_genomes[t_randomEngine.nextInt(m_genomes.size())];
}

void Species::addMember(Genome *t_member) {
    m_genomes.push_back(t_member);
}

void Species::setAdjustedFitness(float t_adjustedFitness)
{
	m_totalAdjustedFitness = t_adjustedFitness;
}

Genome &Species::getMascot() {
    return m_mascot;
}

void Species::addAdjustedFitness(float t_adjustedFitness) {
    m_totalAdjustedFitness += t_adjustedFitness;
}

Genome *Species::getFittest() {
    std::sort(m_genomes.begin(), m_genomes.end(), CompareFitness());

    return m_genomes.back();
}

double Species::getAdjustedFitness() {
    return m_totalAdjustedFitness;
}

Genome *Species::getRandomGenomeBiasedAdjustedFitness(RandomEngine& t_randomEngine) {
    double completeWeight = 0.0;
    for (Genome* g : m_genomes) {
        completeWeight += g->getFitness();
    }
    double r = t_randomEngine.nextFloat() * completeWeight;
    double countWeight = 0.0;
	Genome* fittestGenome = m_genomes[0];
    for (Genome* g : m_genomes) {
		if (fittestGenome->getFitness() < g->getFitness()) {
			fittestGenome = g;
		}
        countWeight += g->getFitness();
        if (countWeight >= r) {
            return g;
        }
    }

	return fittestGenome;

    throw std::runtime_error("WTF! Total adjusted weight is: " + std::to_string(completeWeight));
}
