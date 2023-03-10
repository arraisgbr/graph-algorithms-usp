#pragma region // libraries
#include<iostream>
#include<utility>
#include<queue>
#define BOOST_ALLOW_DEPRECATED_HEADERS // silence warnings
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>
#pragma endregion

enum direction{ FORWARD = 1, BACKWARD = -1};

#pragma region // bundled properties
struct BundledVertex{
    int father;
};

struct BundledArc{
    direction dir;
    int capacity;
    int flow;
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
using std::queue;
#pragma endregion

vector<Arc> read_digraph(Digraph &digraph, int m, std::istream &in){
    vector<Arc> input_order;
    while(m--){
        int u, v, c; std::cin >> u >> v >> c;
        Arc a;
        std::tie(a, std::ignore) = add_edge(--u, --v, digraph);
        digraph[a].dir = FORWARD; digraph[a].capacity = c; digraph[a].flow = 0;
        input_order.push_back(a);
    }

    return input_order;
}

Digraph* get_residual(Digraph &digraph){
    Digraph *residual = new Digraph(num_vertices(digraph));
    for(auto arc : make_iterator_range(boost::edges(digraph))){
        Vertex source = boost::source(arc, digraph);
        Vertex target = boost::target(arc, digraph);
        Arc arcForward = add_edge(source, target, *residual).first;
        Arc arcBackward = add_edge(source, target, *residual).first;

        (*residual)[arcForward].dir = FORWARD;
        (*residual)[arcForward].capacity = digraph[arc].capacity - digraph[arc].flow;
        
        (*residual)[arcBackward].dir = BACKWARD;
        (*residual)[arcBackward].capacity = digraph[arc].flow;
    }

    return residual;
}

void print_residual(Digraph &residual, vector<Arc> &input_order){
    for(auto arc : input_order)
        std::cout << residual[arc].capacity << " " << residual[arc].flow << std::endl;
}

int min_capacity(Digraph &residual, vector<Arc> &path){
    int min_cap = INT32_MAX;
    for(auto arc : path){
        if(residual[arc].capacity > 0)
            min_cap = std::min(min_cap, residual[arc].capacity);
    }
    return min_cap;
}

pair<pair<vector<Arc>, vector<bool>>, bool> bfs(Digraph &residual, int source, int target){
    vector<bool> vertices(num_vertices(residual), false);

    queue<int> queueBfs; queueBfs.push(source);

    vertices[source] = true;
    residual[source].father = -1;

    // bfs
    while(!queueBfs.empty()){
        int from = queueBfs.front(); queueBfs.pop();
        for(auto arc : make_iterator_range(boost::out_edges(from, residual))){
            if(residual[arc].capacity > 0){
                Vertex to = boost::target(arc, residual);
                if(!vertices[to]){
                    residual[to].father = from;
                    vertices[to] = true;
                    queueBfs.push(to);
                }
            }
        }
    }

    bool hasPath = vertices[target];

    // if has a path, construct the path
    vector<Arc> pathArcs;
    if(hasPath){
        vector<int> pathVertices;
        pathVertices.push_back(target);

        while(residual[target].father != -1){
            int from = residual[target].father;
            pathVertices.push_back(from);
            target = from;
        }

        reverse(pathVertices.begin(), pathVertices.end());

        for(int i = 0 ; i < pathVertices.size() - 1 ; i++){
            int from = pathVertices[i];
            int to = pathVertices[i+1];
            Arc arc = boost::edge(from, to, residual).first;
            pathArcs.push_back(arc);
        }

        return {{pathArcs, vertices}, hasPath};
    }
    else return std::make_pair(std::make_pair(pathArcs, vertices), hasPath);

}

void change_digraph(Digraph &residual, vector<Arc> &path, int e){
    for(auto arc : path){
        if(residual[arc].dir == FORWARD) residual[arc].capacity -= e;
        else residual[arc].capacity += e;
    }
}

void print_path(Digraph &residual, vector<Arc> &path, vector<Arc> &input_order){
    for(int i = 0 ; i < input_order.size() ; i++){
        if(std::find(path.begin(), path.end(), input_order[i]) != path.end()){
            if(residual[input_order[i]].dir == FORWARD) std::cout << i+1 << " ";
            else std::cout << -(i+1) << " ";
        }
    }
}

void print_cut(vector<bool> &s){
    for(int i = 0 ; i < s.size() ; i++){
        if(s[i] == true) std::cout << i+1 << " ";
    }
}

int count_cut(vector<bool> &s){
    int ans = 0;
    for(bool vertice : s){
        if(vertice) ans++;
    }
    return ans;
}

void edmonds_karp(Digraph &digraph, vector<Arc> &input_order, int source, int target){
    int maxFlow, e; maxFlow = e = 0;
    Digraph *residual = get_residual(digraph);
    while(true){
        print_residual(*residual, input_order);
        pair<pair<vector<Arc>, vector<bool>>, bool> bfs_results = bfs(*residual, source, target);
        if(bfs_results.second){
            e = min_capacity(*residual, bfs_results.first.first);
            maxFlow += e;
            change_digraph(*residual, bfs_results.first.first, e);
            std::cout << "0 " << e << " " <<  bfs_results.first.first.size() << endl;
            print_path(*residual, bfs_results.first.first, input_order);
            std::cout << endl;
        }
        else{
            std::cout << "1 " << maxFlow << " " << count_cut(bfs_results.first.second) << endl;
            print_cut(bfs_results.first.second);
            std::cout << endl;
            return;
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