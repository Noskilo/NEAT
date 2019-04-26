//
// Created by mike on 4/24/19.
//

#include "Gene.h"

void Gene::print() {

    if (!m_expressed) {
        std::cout << "X ";
    }
    std::cout << "(" << m_source << ")";

    std::cout << "-" << "[" << m_id << ":" << m_weight << "]" << "->";

    std::cout << "(" << m_destination << ")";

}

Gene *Gene::clone() {
    return new Gene(*this);
}