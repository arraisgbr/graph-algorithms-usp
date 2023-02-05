/* the definition of HeadStart may be changed in this file; the rest
 * MUST NOT be changed
 */

#ifndef ARB_H
#define ARB_H

#define BOOST_ALLOW_DEPRECATED_HEADERS // silence warnings
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>

typedef boost::adjacency_list<boost::vecS,
                              boost::vecS,
                              boost::directedS> Arb;
typedef boost::graph_traits<Arb>::vertex_descriptor Vertex;

/* Students must adapt this class for their needs; the innards of
   these objects are only accessed by the student's code */
class HeadStart {
public:
  int *start;
  int *end;
public:
  HeadStart(int num_vertices){
    this->start = new int[num_vertices]; memset(start, -1, sizeof(int) * num_vertices);
    this->end = new int[num_vertices]; memset(end, -1, sizeof(int) * num_vertices);
  }
};

#endif // #ifndef ARB_H
