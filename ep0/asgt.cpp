/* This is the main file that the students must work on to submit; the
 * other one is arb.h
 */

#include "asgt.h"

using boost::add_edge;

void dfs(Arb &arb, const Vertex& root, int *start, int *end, int &time){
  start[root] = time++;
  for(const auto& arc : make_iterator_range(boost::out_edges(root, arb))) {
    Vertex to = boost::target(arc, arb);
    if(start[to] == -1) dfs(arb, to, start, end, time);
  }
  end[root] = time++;
}

Arb read_arb(std::istream& in){
  int num_vertices; in >> num_vertices; 
  Arb tree(num_vertices);

  while(--num_vertices){
    int x, y; in >> x >> y;
    add_edge(--x, --y, tree);
  }

  return tree;
}

HeadStart preprocess (Arb &arb, const Vertex& root){
  int num_vertices = boost::num_vertices(arb);
  int time = 0;
  HeadStart infos(num_vertices);
  dfs(arb, root, infos.start, infos.end, time);
  return infos;
}

bool is_ancestor (const Vertex& u, const Vertex& v, const HeadStart& data){
  if(u == v) return true;
  if(data.start[u] < data.start[v] && data.end[v] < data.end[u]) return true;
  return false;
}
