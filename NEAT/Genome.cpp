//
// Created by mike on 4/24/19.
//

#include "Genome.h"

Node &Genome::getNode(int t_id) {
    return m_nodes[t_id];
}

Gene &Genome::getGene(int t_id) {
    return m_genes[t_id];
}

void Genome::addNode(const Node &t_node) {
    if (t_node.getType() == NodeType::INPUT) {
        m_inputs.push_back(t_node.getId());
    } else if (t_node.getType() == NodeType::OUTPUT) {
        m_outputs.push_back(t_node.getId());
    }

    m_nodes.insert({t_node.getId(), t_node});
}

void Genome::addGene(const Gene &t_gene) {
    m_genes.insert({t_gene.getId(), t_gene});
}

float Genome::compatibility(Genome& other, const float C1, const float C2, const float C3)
{
	float nodeDistance = 0.0f;

	int distjointNodes = 0;
	for (auto& k2 : other.getNodes())
	{
		if (!hasNode(k2.first))
		{
			distjointNodes++;
		}
	}

	for (auto& k1 : m_nodes) {
		if (!other.hasNode(k1.first)) {
			distjointNodes++;
		}
	}

	int maxNodes = std::max(m_nodes.size(), other.getNodes().size());

	nodeDistance = (nodeDistance + C2 * distjointNodes) / maxNodes;


	float connectionDistance = 0.0f;
	int disjointConnections = 0;
	for (auto& k2 : other.getGenes())
	{
		if (!hasGene(k2.first))
		{
			disjointConnections++;
		}
	}

	for (auto& k1 : m_genes)
	{
		if (!other.hasGene(k1.first))
		{
			disjointConnections++;
		}
		else {
			auto gene1 = k1.second;
			auto gene2 = other.getGene(k1.first);
			float homoDistance = std::abs(gene1.getWeight() - gene2.getWeight());
			if (gene1.isExpressed() != gene2.isExpressed())
			{
				homoDistance += 1.0f;
			}
			connectionDistance += homoDistance * C3;
		}
	}

	return nodeDistance + connectionDistance;
}

void Genome::generatePhenotype(GeneTracker* geneTracker)
{
	Genome genome;

	auto n1 = Node(1, NodeType::INPUT);
	auto n2 = Node(2, NodeType::INPUT);
	auto n3 = Node(3, NodeType::OUTPUT);


	genome.addNode(n1);
	genome.addNode(n2);
	genome.addNode(n3);

	genome.addGene(Gene(geneTracker->getInnovationId(n1.getId(), n3.getId()), n1.getId(), n3.getId(), 1.0f, true));
	genome.addGene(Gene(geneTracker->getInnovationId(n2.getId(), n3.getId()), n2.getId(), n3.getId(), 1.0f, true));

	*this = genome;
}

void Genome::print() {

    std::cout << "============" << std::endl;
    std::cout << "Fitness: " << m_fitness << std::endl;
    std::cout << m_nodes.size() << " [";
    for (auto &pair :m_nodes) {
        pair.second.print();

        if (pair.first != m_nodes.rbegin()->first) {
            std::cout << ", ";
        }

    }
    std::cout << "]" << std::endl;
    std::cout << "************" << std::endl;
    for (auto &pair : m_genes) {
        pair.second.print();
        std::cout << std::endl;
    }
    std::cout << "============" << std::endl;
}

void Genome::mutate(RandomEngine &t_randomEngine) {

    for (auto &pair : m_genes) {
        if (t_randomEngine.nextFloat() < PROBABILITY_PERTURBING) {
            pair.second.setWeight(pair.second.getWeight() * (t_randomEngine.nextFloat() * 4.0f - 2.0f));
        } else {
            pair.second.setWeight(t_randomEngine.randomRange(-30.0f, 30.0f));
        }

    }

}

void Genome::mutateGeneConnection(RandomEngine &t_randomEngine, GeneTracker *t_geneTracker, int t_maxAttempts) {
    int tries = 0;
    bool success = false;
    while (tries < t_maxAttempts && !success) {
        tries++;
        auto node1 = m_nodes.begin();
        std::advance(node1, t_randomEngine.nextInt(m_nodes.size()));
        auto node2 = m_nodes.begin();
        std::advance(node2, t_randomEngine.nextInt(m_nodes.size()));

        if (node1->second.getId() == node2->second.getId()) {
            continue;
        }

        float weight = t_randomEngine.nextFloat() * 2.0f - 1.0f;

        bool reversed = false;
        if (node1->second.getType() == NodeType::HIDDEN && node2->second.getType() == NodeType::INPUT) {
            reversed = true;
        } else if (node1->second.getType() == NodeType::OUTPUT && node2->second.getType() == NodeType::HIDDEN) {
            reversed = true;
        } else if (node1->second.getType() == NodeType::OUTPUT && node2->second.getType() == NodeType::INPUT) {
            reversed = true;
        }

        bool connectionImpossible = false;
        if (node1->second.getType() == NodeType::INPUT && node2->second.getType() == NodeType::INPUT) {
            connectionImpossible = true;
        } else if (node1->second.getType() == NodeType::OUTPUT && node2->second.getType() == NodeType::OUTPUT) {
            connectionImpossible = true;
        }

        bool connectionExists = false;
        for (auto &pair : m_genes) {
            if (pair.second.getSourceId() == node1->second.getId() &&
                pair.second.getDestinationId() == node2->second.getId()) { // existing connection
                connectionExists = true;
                break;
            } else if (pair.second.getSourceId() == node2->second.getId() &&
                       pair.second.getDestinationId() == node1->second.getId()) { // existing connection
                connectionExists = true;
                break;
            }
        }

        if (connectionExists || connectionImpossible) {
            continue;
        }

		int sourceId = reversed ? node2->second.getId() : node1->second.getId();
		int destinationId = reversed ? node1->second.getId() : node2->second.getId();
        Gene newGene(t_geneTracker->getInnovationId(sourceId, destinationId), sourceId, destinationId, weight, true);

        m_genes.insert({newGene.getId(), newGene});
        success = true;
    }

}

void Genome::mutateNode(RandomEngine &t_randomEngine, GeneTracker *t_geneTracker) {

    auto gene = m_genes.begin();
    std::advance(gene, t_randomEngine.nextInt(m_genes.size()));

    Node sourceNode = m_nodes[gene->second.getSourceId()];
    Node destinationNode = m_nodes[gene->second.getDestinationId()];

    gene->second.disable();

    Node newNode(m_nodes.size()+1, NodeType::HIDDEN);
    Gene sourceToNew(t_geneTracker->getInnovationId(sourceNode.getId(), newNode.getId()), sourceNode.getId(), newNode.getId(), 1.0f, true);
    Gene newToDestination(t_geneTracker->getInnovationId(newNode.getId(), destinationNode.getId()), newNode.getId(), destinationNode.getId(), gene->second.getWeight(),
                          true);

    m_nodes.insert({newNode.getId(), newNode});
    m_genes.insert({sourceToNew.getId(), sourceToNew});
    m_genes.insert({newToDestination.getId(), newToDestination});
}

const bool Genome::hasGene(int t_id) const {
    return m_genes.find(t_id) != m_genes.end();
}

const bool Genome::hasNode(int t_id) const {
    return m_nodes.find(t_id) != m_nodes.end();
}

float Genome::getFitness() const {
    return m_fitness;
}

void Genome::setFitness(float t_fitness) {
    m_fitness = t_fitness;
}

const std::map<int, Node> &Genome::getNodes() const {
    return m_nodes;
}

const std::map<int, Gene> &Genome::getGenes() const {
    return m_genes;
}

std::vector<float> Genome::evaluate(std::vector<float> inputs) {
    if (inputs.size() != m_inputs.size()) {
        throw std::runtime_error("Incorrect number of inputs!");
    }

    int i = 0;
    for (auto input : m_inputs) {
        auto &node = m_nodes[input];
        node.setValue(inputs[i]);
        i++;
    }

    std::vector<float> result;


    for (auto out : m_outputs) {
        int connectionCount = 0;
        auto &outputNode = m_nodes.at(out);
        outputNode.setValue(0.0f);

        for (auto &pair : m_genes) {
            pair.second.traveresed = false;
            if (pair.second.getDestinationId() == out && pair.second.isExpressed()) {
                connectionCount++;
                outputNode.setValue(outputNode.getValue() + traverse(pair.second));
            }
        }
        if (connectionCount == 0) {
            outputNode.setValue(0);
        } else {
            outputNode.setValue(outputNode.getValue() / connectionCount);
        }

        result.push_back(outputNode.getValue());
    }


    return result;
}

float Genome::traverse(Gene &gene) {
    gene.traveresed = true;
    if (m_nodes[gene.getSourceId()].getType() == NodeType::INPUT) {
        return m_nodes[gene.getSourceId()].getValue() * gene.getWeight();
    } else {

        int connectionCount = 0;

        auto &hiddenNode = m_nodes.at(gene.getSourceId());
        hiddenNode.setValue(0.0f);

        for (auto &pair : m_genes) {
            if (pair.second.getDestinationId() == gene.getSourceId() && pair.second.isExpressed() &&
                !pair.second.traveresed) {
                connectionCount++;
                hiddenNode.setValue(hiddenNode.getValue() + traverse(pair.second));
            }
        }

        if (connectionCount == 0) {
            hiddenNode.setValue(0.0);
        } else {
            hiddenNode.setValue(hiddenNode.getValue() / connectionCount);
        }


        return hiddenNode.getValue();


    }

}
