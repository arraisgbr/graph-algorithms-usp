#pragma region // libraries
#include<iostream>
#include<utility>
#define BOOST_ALLOW_DEPRECATED_HEADERS // silence warnings
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>
#pragma endregion

enum direction{ FORWARD = 1, BACKWARD = -1};

#pragma region // bundled properties
struct BundledVertex{

};

struct BundledArc{
    direction dir;
    int capacity;
    int flow = 0;
};
#pragma endregion

#pragma region // typedefs
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, BundledVertex, BundledArc> Digraph;
typedef boost::graph_traits<Digraph>::vertex_descriptor Vertex;
typedef boost::graph_traits<Digraph>::edge_descriptor Arc;
#pragma endregion

#pragma region // namespaces
using boost::add_edge;
using boost::num_vertices;
using boost::make_iterator_range;
using std::endl;
using std::pair;
using std::vector;
#pragma endregion

vector<Arc>& read_digraph(Digraph &digraph, int m, std::istream &in){
    vector<Arc> input_order;
    while(m--){
        int u, v, c; std::cin >> u >> v >> c;
        Arc a;
        std::tie(a, std::ignore) = add_edge(--u, --v, digraph);
        digraph[a].dir = FORWARD; digraph[a].capacity = c;
        input_order.push_back(a);
    }

    return input_order;
}

Digraph& get_residual(Digraph &digraph){
    Digraph residual(num_vertices(digraph));
    for(auto arc : make_iterator_range(boost::edges(digraph))){
        Vertex source = boost::source(arc, digraph);
        Vertex target = boost::target(arc, digraph);
        pair<Arc, bool> arcForward = add_edge(source, target, residual);
        pair<Arc, bool> arcBackward = add_edge(source, target, residual);

        residual[arcForward.first].dir = FORWARD;
        residual[arcForward.first].capacity = digraph[arc].capacity - digraph[arc].flow;
        
        residual[arcBackward.first].dir = BACKWARD;
        residual[arcForward.first].capacity = digraph[arc].flow;
    }

    return residual;
}

// Funciona?
void print_residual(Digraph &residual, vector<Arc> &input_order){
    for(auto arc : input_order){
        std::cout << residual[arc].capacity << " " << residual[arc].capacity;
    }
}

int min_capacity(Digraph &residual, vector<int> &path){
    int min_cap = INT32_MAX;
    
    for(int i = 0 ; i < path.size() - 1 ; i++){
        pair<Arc, bool> a = boost::edge(path[i], path[i+1], residual);
        min_cap = std::min(min_cap, residual[a.first].capacity);
    }
    
    return min_cap;
}

// toDo
pair<pair<vector<int>, vector<bool>>, bool> bfs(Digraph &residual){

}

// toDo
Digraph& change_digraph(Digraph &digraph){

}

void print_path(Digraph &digraph, vector<int> &path, vector<Arc> &input_order){
    for(auto arc : input_order){
        Vertex source = boost::source(arc, digraph);
        Vertex target = boost::target(arc, digraph);
        for(int i = 0 ; i < path.size()-1 ; i++){
            if(path[i] == source && path[i+1] == target){
                if(digraph[arc].dir == FORWARD) std::cout << i << " ";
                else std::cout << -i << " ";
            }
        }
    }
}

void print_cut(vector<bool> &s){
    for(int i = 0 ; i < s.size() ; i++){
        if(s[i] == true) std::cout << i << " ";
    }
}

void edmonds_karp(Digraph &digraph, vector<Arc> &input_order, int source, int target){
    int maxFlow, e; maxFlow = e = 0;
    while(true){
        Digraph residual = get_residual(digraph);
        print_residual(residual, input_order);
        pair<pair<vector<int>, vector<bool>>, bool> bfs_results = bfs(residual);
        if(bfs_results.second){
            e = min_capacity(residual, bfs_results.first.first);
            maxFlow += e;
            // digraph = change_digraph(residual);
            std::cout << "0 " << e << " " <<  bfs_results.first.first.size() << endl;
            print_path(digraph, bfs_results.first.first, input_order);
            std::cout << endl;
        }
        else{
            std::cout << "1 " << maxFlow << " " << bfs_results.first.second.size() << endl;
            print_cut(bfs_results.first.second);
            std::cout << endl;
        }
    }
}

int main(int argc, char **argv){

    int n, m; std::cin >> n >> m;
    int s, t; std::cin >> s >> t; s--; t--;

    Digraph digraph(n);

    vector<Arc> input_order = read_digraph(digraph, m, std::cin);

    edmonds_karp(digraph, input_order, s, t);
}