//
// Created by mike on 4/24/19.
//

#include "Node.h"

void Node::print() {
    std::string nodeType = "";

    switch (m_type) {
        case NodeType::HIDDEN:
            nodeType = "H";
            break;
        case NodeType::INPUT:
            nodeType = "I";
            break;
        case NodeType::OUTPUT:
            nodeType = "O";
            break;
        default:
            nodeType = "X";
            break;
    }

    std::cout << m_id << ":" << nodeType << ":" << m_value;
}

Node *Node::clone() {
    return new Node(*this);
}

float Node::getValue() const {
    return m_value;
}

void Node::setValue(float t_value) {
    m_value = t_value;
}
