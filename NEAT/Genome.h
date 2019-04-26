//
// Created by mike on 4/24/19.
//

#ifndef NEAT_GENOME_H
#define NEAT_GENOME_H


#include "Node.h"
#include "Gene.h"
#include "RandomEngine.h"
#include "GeneTracker.h"
#include <map>
#include <vector>
#include <iterator>

class Genome {
private:
    std::map<int, Node> m_nodes;
    std::map<int, Gene> m_genes;
    const float PROBABILITY_PERTURBING = 0.9f;
    float m_fitness = 0.0f;
    std::vector<int> m_inputs;
    std::vector<int> m_outputs;

    float traverse(Gene &gene);

public:
    Genome() {};

    void mutate(RandomEngine &t_randomEngine);

    void mutateGeneConnection(RandomEngine &t_randomEngine, GeneTracker *t_geneTracker, int t_maxAttempts = 10);

    void mutateNode(RandomEngine &t_randomEngine, GeneTracker *t_geneTracker);

    int lastGeneInnovation() {
        return m_genes.rbegin()->first;
    };

    std::vector<float> evaluate(std::vector<float> inputs);

    Node &getNode(int t_id);

    Gene &getGene(int t_id);

    const bool hasGene(int t_id) const;

    const bool hasNode(int t_id) const;

    void addNode(const Node &t_node);

    void addGene(const Gene &t_gene);

    Genome *clone() {
        return new Genome(*this);
    }

	float compatibility(Genome& other, const float C1, const float C2, const float C3);

	void generatePhenotype(GeneTracker* geneTracker);

    void print();

    float getFitness() const;

    void setFitness(float t_fitness);

    const std::map<int, Node> &getNodes() const;

    const std::map<int, Gene> &getGenes() const;


    Genome &operator=(const Genome &other) {
        if (this != &other) {
            m_nodes = other.m_nodes;
            m_genes = other.m_genes;
            m_fitness = other.m_fitness;
            m_inputs = other.m_inputs;
            m_outputs = other.m_outputs;
        }
        return *this;
    };

    bool operator<(const Genome &other) const {
        return this->m_fitness < other.m_fitness;
    }

    bool operator>(const Genome &other) const {
        return this->m_fitness > other.m_fitness;
    }

};


#endif //NEAT_GENOME_H
