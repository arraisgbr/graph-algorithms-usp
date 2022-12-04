#include "asgt.h"

#include <utility>              // for std::get
#include <tuple>
#include <vector>
#include <stack>

#include <cmath>

#define BOOST_ALLOW_DEPRECATED_HEADERS // silence warnings
#include <boost/graph/adjacency_list.hpp>
#include <boost/optional.hpp>

#include "cycle.h"
#include "digraph.h"
#include "potential.h"

/* The code in this template file is all "bogus". It just illustrates
 * how to return answers back to main.cpp. */

/* The following declarations shorten the bogus code below. Feel free
 * to change/drop them. */
using boost::vertices;
using boost::edges;
using boost::num_vertices;
using boost::add_edge;
using boost::out_edges;
using boost::make_iterator_range;
using std::vector;
using std::stack;
using std::pair;

Digraph build_digraph(const Digraph& market)
{
  /* placeholder for NRVO */
  Digraph digraph(num_vertices(market));

  /* flip some signs in the arc costs below to exercise the many
   * execution pathways */
  /* create arcs 01 and 10 */
  // Arc a0, a1;
  // std::tie(a0, std::ignore) = add_edge(0, 1, digraph);
  // digraph[a0].cost = 11.0;
  // std::tie(a1, std::ignore) = add_edge(1, 0, digraph);
  // digraph[a1].cost = -17.0;

  for(const auto& vertex : make_iterator_range(vertices(market))){
    for(const auto& arc : make_iterator_range(out_edges(vertex, market))){
      Arc a0;
      Vertex to = boost::target(arc, market);
      std::tie(a0, std::ignore) = add_edge(vertex, to, digraph);
      digraph[a0].cost = -log(market[arc].cost);
    }
  }

  return digraph;
}

std::tuple<bool,
           boost::optional<NegativeCycle>,
           boost::optional<FeasiblePotential>>
has_negative_cycle(Digraph& digraph)
{
  // const Arc& a0 = *(out_edges(0, digraph).first);
  // const Arc& a1 = *(out_edges(1, digraph).first);

  // Walk walk(digraph, 0);
  // walk.extend(a0);
  // walk.extend(a1);

  // bellman-ford algorithm
  boost::graph_traits<Digraph>::vertices_size_type n = num_vertices(digraph);
  double matrix[2][n]; memset(matrix, 0, sizeof(matrix));
  
  for(int i = 1 ; i <= n ; i++){
    for(const auto& arc : make_iterator_range(edges(digraph))){
        Vertex from = boost::source(arc, digraph);
        Vertex to = boost::target(arc, digraph);
        if(matrix[i%2][to] > matrix[(i-1)%2][from] + digraph[arc].cost){
          digraph[to].pred = from;
          matrix[i%2][to] = matrix[(i-1)%2][from] + digraph[arc].cost;
          digraph[to].fp = matrix[i%2][to];
        }
    }
  }

  // verifying if the graph has a negative cycle
  bool hasNegativeCycle = false;
  int changed_vertex = -1;
  for(int i = 0 ; i < n ; i++){
    if(matrix[1][i] != matrix[0][i]){
      hasNegativeCycle = true;
      changed_vertex = i;
      break;
    }
  }

  if(hasNegativeCycle){
    // recovery a walk
    stack<int> auxWalk;
    bool visited[n]; memset(visited, 0, sizeof(visited));
    while(!visited[changed_vertex]){
      visited[changed_vertex] = true;
      auxWalk.push(changed_vertex);
      changed_vertex = digraph[changed_vertex].pred;
    }
    
    // constructing a walk object
    Walk walk = Walk(digraph, changed_vertex);
    Vertex pred = changed_vertex;
    while(!walk.is_cycle()){
      Vertex current = auxWalk.top(); auxWalk.pop();
      pair<Arc, bool> arc = boost::edge(pred, current, digraph);
      walk.extend(arc.first);
      pred = current;
    }

    return {true, NegativeCycle(walk), boost::none};
  }
  else{
    // copying the feasible potentials
    vector<double> y(num_vertices(digraph), 0.0);
    for(int i = 0 ; i < n ; i++) y[i] = matrix[1][i];
    
    return {false, boost::none, FeasiblePotential(digraph, y)};
  }
}

Loophole build_loophole(const NegativeCycle& negcycle,
                        const Digraph& aux_digraph,
                        const Digraph& market)
{
  vector<Arc> arcs = negcycle.get();
  Vertex firstV = boost::source(arcs[0], aux_digraph);
  Vertex secondV = boost::target(arcs[0], aux_digraph);
  Arc firstA = boost::edge(firstV, secondV, market).first;
  Walk w = Walk(market, firstV);
  w.extend(firstA);

  for(int i = 1 ; i < arcs.size() ; i++){
    Vertex from = boost::source(arcs[i], aux_digraph);
    Vertex to = boost::target(arcs[i], aux_digraph);
    Arc arcMarket = boost::edge(from, to, market).first;
    w.extend(arcMarket);
  }

  return Loophole(w);
}

FeasibleMultiplier build_feasmult(const FeasiblePotential& feaspot,
                                  const Digraph& aux_digraph,
                                  const Digraph& market)
{
  vector<double> z(num_vertices(market), 1.0);

  int n = num_vertices(market);
  for(int i = 0 ; i < n ; i++)
    z[i] = exp(-aux_digraph[i].fp);

  return FeasibleMultiplier(market, z);
}
