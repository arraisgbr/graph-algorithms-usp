// libs
#pragma region
#include <iostream>
#include <cmath>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#pragma endregion

// defines
#pragma region
#define BOOST_ALLOW_DEPRECATED_HEADERS // silence warnings
#pragma endregion

// typedefs
#pragma region
typedef boost::adjacency_list<boost::vecS,
                              boost::vecS,
                              boost::directedS> Digraph;
typedef boost::graph_traits<Digraph>::vertex_descriptor Vertex;
typedef boost::graph_traits<Digraph>::edge_descriptor Arc;
#pragma endregion

// namespaces
#pragma region
using boost::add_edge;
using boost::make_iterator_range;
using boost::vertices;
using boost::out_edges;
using boost::target;
#pragma endregion

// toValidOrFalse and toTrue functions
#pragma region
int toValidOrFalse(int var, int n){
    return abs(var) + n;
}
int toTrue(int var, int n){
    return var - n;
}
#pragma endregion

int main(){

    int debug; std::cin >> debug;
    int n, m; std::cin >> n >> m;

    Digraph digraph(2*n);

    while(m--){
        int a, b; std::cin >> a >> b;
        // getting the positive value for the vertex
        if(a < 0) a = toValidOrFalse(a, n);
        if(b < 0) b = toValidOrFalse(a, n);

        // getting the negation of the variable
        int aNeg, bNeg;
        if(a > n) aNeg = toTrue(a, n);
        else aNeg = toValidOrFalse(a, n);
        if(b > n) bNeg = toTrue(b, n);
        else bNeg = toValidOrFalse(b, n);
        
        std::cout << aNeg << " " << b << std::endl;
        std::cout << bNeg << " " << a << std::endl;

        add_edge(aNeg, b, digraph);
        add_edge(bNeg, a, digraph);
    }
}