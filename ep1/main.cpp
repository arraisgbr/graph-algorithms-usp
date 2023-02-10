// libs
#pragma region
#include <iostream>
#include <cmath>
#include <stack>
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
using boost::num_vertices;
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

// tarjan
#pragma region
std::pair<bool, int> verifySat(int *scc, int n){
    bool sat = true;
    bool varUnsat;
    for(int i = 0 ; i < n ; i++){
        int iNeg = toValidOrFalse(i, n);
        if(scc[i] == scc[iNeg]){
            varUnsat = i;
            sat = false;
            break;
        }
    }
    return std::make_pair(sat, varUnsat);
}

void tarjanR(Digraph &digraph, int vertex, int *scc, int *disc, int *low, std::stack<int> &stack, bool *inStack, int sccNum){
    static int time = 0;
    disc[vertex] = low[vertex] = ++time;
    stack.push(vertex);
    inStack[vertex] = true;

    for(Arc arc : make_iterator_range(out_edges(vertex, digraph))){
        Vertex to = target(arc, digraph);
        if(disc[to] == -1){
            tarjanR(digraph, to, scc, disc, low, stack, inStack, sccNum);
            low[vertex] = std::min(low[vertex], low[to]);
        }
        else if(inStack[to])
            low[vertex] = std::min(low[vertex], disc[to]);
    }

    //debug
    // std::cout << "Chegou no stack" << std::endl;

    if(disc[vertex] == low[vertex]){
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

std::pair<bool, int> tarjan(Digraph &digraph){
    int n = num_vertices(digraph);
    std::stack<int> stack;
    
    int sccNum = 0;
    int *disc = new int[2*n]; 
    int *low = new int[2*n]; 
    bool *inStack = new bool[2*n]; 
    int *scc = new int[2*n];
    for(int i = 0 ; i < 2*n ; i++){
        scc[i] = disc[i] = low[i] = -1;
        inStack[i] = false;
    }

    for(int i = 0 ; i < 2*n ; i++){
        if(disc[i] == -1){
            sccNum++;
            tarjanR(digraph, i, scc, disc, low, stack, inStack, sccNum);
        }
    }

    std::pair<bool, int> ans = verifySat(scc, n);    

    return ans;
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
        else a -= 1;
        if(b < 0) b = toValidOrFalse(b, n);
        else b -= 1;

        // getting the negation of the variable
        int aNeg, bNeg;
        if(a >= n) aNeg = toTrue(a, n);
        else aNeg = toValidOrFalse(a, n);
        if(b >= n) bNeg = toTrue(b, n);
        else bNeg = toValidOrFalse(b, n);
        
        add_edge(aNeg, b, digraph);
        add_edge(bNeg, a, digraph);
    }

    for(Vertex vertex : make_iterator_range(vertices(digraph))){
        std::cout << vertex << ": ";
        for(Arc arc : make_iterator_range(out_edges(vertex, digraph))){
            Vertex to = target(arc, digraph);
            std::cout << to << " ";
        }
        std::cout << std::endl;
    }

    std::pair<bool, int> ans = tarjan(digraph);

    if(ans.first){
        std::cout << "YES" << std::endl;
        // toDo
    } 
    else{
        std::cout << "NO" << std::endl;
        std::cout << ans.second << std::endl;
        // toDo
    }

}