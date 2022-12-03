#include "asgt.h"

#include <utility>              // for std::get
#include <tuple>
#include <vector>

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
      digraph[a0].cost = log2(market[arc].cost);
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

  double matrix[2][n];
  memset(matrix, 0, sizeof(matrix));
  
  for(int i = 1 ; i <= n ; i++){
    for(const auto& arc : make_iterator_range(edges(digraph))){
        Vertex from = boost::source(arc, digraph);
        Vertex to = boost::target(arc, digraph);
        matrix[i%2][to] = matrix[(i-1)%2][to];
        if(matrix[i%2][to] > matrix[(i-1)%2][from] + digraph[arc].cost)
          matrix[i%2][to] = matrix[(i-1)%2][from] + digraph[arc].cost;
    }
  }

  // verifying if the graph has a negative cycle
  bool hasNegativeCycle = false;
  for(int i = 0 ; i < n ; i++){
    if(matrix[1][i] != matrix[0][i]){
      hasNegativeCycle = true;
      break;
    }
  }

  if(hasNegativeCycle){
  /* Replace `NegativeCycle(walk)` with `boost::none` in the next
   * command to trigger "negative cycle reported but not computed".
   * Comment the whole `return` and uncomment the remaining lines to
   * exercise construction of a feasible potential. */

  // encourage RVO
    return {true, boost::none, boost::none};
  }
  else{
    /* Replace `FeasiblePotential(digraph, y)` with `boost::none` in the
    * next command to trigger "feasible potential reported but not
    * computed". */

    // encourage RVO
    vector<double> y(num_vertices(digraph), 0.0);
    return {false, boost::none, boost::none};
  }
}

Loophole build_loophole(const NegativeCycle& negcycle,
                        const Digraph& aux_digraph,
                        const Digraph& market)
{
  /* bogus code */
  const Arc& b0 = *(out_edges(0, market).first);
  const Arc& b1 = *(out_edges(1, market).first);

  Walk w(market, 0);
  w.extend(b0);
  w.extend(b1);

  // encourage RVO
  return Loophole(w);
}

FeasibleMultiplier build_feasmult(const FeasiblePotential& feaspot,
                                  const Digraph& aux_digraph,
                                  const Digraph& market)
{
  vector<double> z(num_vertices(market), 1.0);

  // encourage RVO
  return FeasibleMultiplier(market, z);
}
