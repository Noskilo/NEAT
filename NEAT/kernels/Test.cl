enum NodeType { INPUT, OUTPUT, HIDDEN, INVALID};

struct Node {
    int m_id;
    enum NodeType m_type;
    float m_value;
};

struct Gene {
    int source;
    int destination;
    float weight;
    int id;
    bool expressed;
};

kernel void Test(global struct Node* nodes)
{
    nodes[get_global_id(0)].m_value +=23.0f;
}