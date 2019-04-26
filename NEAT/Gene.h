//
// Created by mike on 4/24/19.
//

#ifndef NEAT_GENE_H
#define NEAT_GENE_H


#include "Node.h"

class Gene {
private:
    int m_source;
    int m_destination;
    float m_weight;
    int m_id;
    bool m_expressed;
public:
    Gene() : m_source(0), m_destination(0), m_weight(0.0f), m_id(-1), m_expressed(false) {};

    Gene(int t_id, int t_source, int t_destination, float t_weight, bool t_expressed) : m_id(t_id),
                                                                                          m_source(t_source),
                                                                                          m_destination(t_destination),
                                                                                          m_weight(t_weight),
                                                                                          m_expressed(t_expressed) {};
    bool traveresed = false;
    void disable() {
        m_expressed = false;
    }

    void print();

    Gene *clone();

    const int getSourceId() const {
        return m_source;
    }

    const int getDestinationId() const {
        return m_destination;
    }


    float getWeight() const {
        return m_weight;
    }

    void setWeight(float t_weight) {
        m_weight = t_weight;
    }

    int getId() const {
        return m_id;
    }

    bool isExpressed() const {
        return m_expressed;
    }

};


#endif //NEAT_GENE_H
