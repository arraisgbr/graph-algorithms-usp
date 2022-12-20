#include<iostream>
#include<utility>
#define BOOST_ALLOW_DEPRECATED_HEADERS // silence warnings
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>

enum direction{ FORWARD = 1, BACKWARD = -1};

struct BundledVertex{

};

struct BundledArc{
    direction dir;
    int capacity;
    int flow = 0;
};

#pragma region
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, BundledVertex, BundledArc> Digraph;
typedef boost::graph_traits<Digraph>::vertex_descriptor Vertex;
typedef boost::graph_traits<Digraph>::edge_descriptor Arc;
#pragma endregion

#pragma region
using boost::add_edge;
using boost::num_vertices;
using boost::make_iterator_range;
using std::pair;
#pragma endregion

void read_digraph(Digraph &digraph, int m, std::istream &in){
    while(m--){
        int u, v, c; std::cin >> u >> v >> c;
        Arc a;
        std::tie(a, std::ignore) = add_edge(--u, --v, digraph);
        digraph[a].dir = FORWARD; digraph[a].capacity = c;
    }
}

int main(int argc, char **argv){

    int n, m; std::cin >> n >> m;
    int s, t; std::cin >> s >> t; s--; t--;

    Digraph digraph(n);

    read_digraph(digraph, m, std::cin);

}