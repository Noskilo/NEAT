//
// Created by mike on 4/24/19.
//

#ifndef NEAT_NODE_H
#define NEAT_NODE_H


#include "NodeType.h"
#include <iostream>

class Node {
private:
    int m_id;
    NodeType m_type;
    float m_value = 0.0f;
public:
    Node() : m_id(-1), m_type(NodeType::INVALID) {};

    Node(int t_id, NodeType t_type) : m_id(t_id), m_type(t_type) {}

    int getId() const {
        return m_id;
    }

    NodeType getType() const {
        return m_type;
    };

    void print();

    Node *clone();

    float getValue() const;

    void setValue(float t_value);
};


#endif //NEAT_NODE_H
