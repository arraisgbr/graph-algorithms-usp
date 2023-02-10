// libs
#pragma region
#include <iostream>
#include <cmath>
#include <stack>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/range/iterator_range.hpp>
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
using boost::source;
using boost::target;
using boost::num_vertices;
#pragma endregion

// global variables
int *map;

// toValidOrFalse, toTrue and toNegative functions
#pragma region
int toValidOrFalse(int var, int num_vertices){
    return abs(var) + num_vertices;
}

int toTrue(int var, int num_vertices){
    return var - num_vertices;
}

int toNegative(int var, int num_vertices){
    if(var > num_vertices) return num_vertices - var;
    return var;
}
#pragma endregion

// tarjan
#pragma region
std::pair<bool, int> verifySat(int *scc, int num_vertices){
    bool sat = true;
    bool varUnsat;
    for(int i = 0 ; i < num_vertices ; i++){
        int iNeg = toValidOrFalse(i, num_vertices);
        if(scc[i] == scc[iNeg]){
            varUnsat = i;
            sat = false;
            break;
        }
    }
    return std::make_pair(sat, varUnsat);
}

void tarjanR(Digraph &digraph, int vertex, int *scc, int *disc, int *low, std::stack<int> &stack, bool *inStack, int &sccNum, int &time){
    disc[vertex] = low[vertex] = ++time;
    stack.push(vertex);
    inStack[vertex] = true;

    for(Arc arc : make_iterator_range(out_edges(vertex, digraph))){
        Vertex to = target(arc, digraph);
        if(disc[to] == -1){
            tarjanR(digraph, to, scc, disc, low, stack, inStack, sccNum, time);
            low[vertex] = std::min(low[vertex], low[to]);
        }
        else if(inStack[to])
            low[vertex] = std::min(low[vertex], disc[to]);
    }

    if(disc[vertex] == low[vertex]){
        sccNum++;
        while(stack.top() != vertex){
            Vertex curr = stack.top(); stack.pop();
            inStack[curr] = false;
            scc[curr] = sccNum;
        }
        stack.pop();
        scc[vertex] = sccNum;
        inStack[vertex] = false;
    }
}

std::pair<bool, int> tarjan(Digraph &digraph, int num_vertices){
    std::stack<int> stack;
    
    int sccNum = 0;
    int time = 0;
    int *disc = new int[2*num_vertices]; 
    int *low = new int[2*num_vertices]; 
    bool *inStack = new bool[2*num_vertices]; 
    int *scc = new int[2*num_vertices];
    for(int i = 0 ; i < 2*num_vertices ; i++){
        scc[i] = disc[i] = low[i] = -1;
        inStack[i] = false;
    }

    for(int i = 0 ; i < 2*num_vertices ; i++){
        if(disc[i] == -1)
            tarjanR(digraph, i, scc, disc, low, stack, inStack, sccNum, time);
    }

    std::pair<bool, int> ans = verifySat(scc, num_vertices);

    return ans;
}
#pragma endregion

// read digraph
#pragma region
std::pair<Digraph, int> readDigraph(std::istream &in){
    int num_vertices, m; in >> num_vertices >> m;

    std::vector<std::pair<int, int>> arcs;

    while(m--){
        int a, b; std::cin >> a >> b;
        // getting the positive value for the vertex
        if(a < 0) a = toValidOrFalse(a, num_vertices);
        if(b < 0) b = toValidOrFalse(b, num_vertices);

        // getting the negation of the variable
        int aNeg, bNeg;
        if(a > num_vertices) aNeg = toTrue(a, num_vertices);
        else aNeg = toValidOrFalse(a, num_vertices);
        if(b > num_vertices) bNeg = toTrue(b, num_vertices);
        else bNeg = toValidOrFalse(b, num_vertices);
        
        arcs.push_back(std::make_pair(aNeg, b));
        arcs.push_back(std::make_pair(bNeg, a));
    }

    return std::make_pair(Digraph(arcs.begin(), arcs.end(), 2*num_vertices), num_vertices);
}
#pragma endregion

//dfs and print path
#pragma region
void dfsR(Digraph &digraph, int varUnsat, int varUnsatNeg, std::stack<Vertex> &path, bool *vis){
    vis[varUnsat] = true;
    path.push(varUnsat);
    if(varUnsat == varUnsatNeg) return;
    for(Arc arc : make_iterator_range(out_edges(varUnsat, digraph))){
        Vertex to = target(arc, digraph);
        if(!vis[to]){
            dfsR(digraph, to, varUnsatNeg, path, vis);
            return;
        }
    }
}

void dfs(Digraph &digraph, int varUnsat, int varUnsatNeg, std::stack<Vertex> &path, int num_vertices){
    bool *vis = new bool[num_vertices]; memset(vis, false, sizeof(bool) *num_vertices);
    dfsR(digraph, varUnsat, varUnsatNeg, path, vis);
}

void printPath(Digraph &digraph, std::stack<Vertex> &path, int num_vertices){
    std::cout << path.size()-1 << " ";
    std::vector<Vertex> pathInv;
    while(!path.empty()){
        pathInv.push_back(path.top()); path.pop();
    }
    std::reverse(pathInv.begin(), pathInv.end());
    for(auto vertex : pathInv) std::cout << map[vertex] << " ";
    std::cout << std::endl;
}
#pragma endregion

int main(){

    int debug; std::cin >> debug;

    std::pair<Digraph, int> digraphFull = readDigraph(std::cin);
    Digraph digraph = digraphFull.first;
    int num_vertices = digraphFull.second;

    map = new int[2*num_vertices];
    for(int i = 1 ; i <= num_vertices ; i++) map[i] = i;
    for(int i = num_vertices+1, j = -1 ; i <= 2*num_vertices ; i++, j--) map[i] = j;

    std::pair<bool, int> ans = tarjan(digraph, num_vertices);
    int varUnsat = ans.second;

    if(ans.first){
        std::cout << "YES" << std::endl;
        // toDo
    } 
    else{
        std::cout << "NO" << std::endl;
        std::cout << varUnsat << std::endl;
        std::stack<Vertex> path;
        std::stack<Vertex> pathRev;
        int varUnsatNeg = varUnsat > num_vertices ? toTrue(varUnsat, num_vertices) : toValidOrFalse(varUnsat, num_vertices);
        dfs(digraph, varUnsat, varUnsatNeg, path, num_vertices);
        dfs(digraph, varUnsatNeg, varUnsat, pathRev, num_vertices);
        printPath(digraph, path, num_vertices);
        printPath(digraph, pathRev, num_vertices);
    }

}