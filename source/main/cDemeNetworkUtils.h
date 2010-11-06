/*
 *  cDemeNetworkUtils.h
 *  Avida
 *
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
 *
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; version 2
 *  of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#ifndef cDemeNetworkUtils_h
#define cDemeNetworkUtils_h

/* THIS HEADER REQUIRES BOOST */
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/graph/visitors.hpp>
#include <numeric>
#include <vector>


//! A BGL breadth-first visitor to calculate distances between vertices.
template <class Distance>
class NetworkDistanceVisitor : public boost::bfs_visitor< > {
public:
  NetworkDistanceVisitor(Distance d) : distance(d) { }
  template <class Graph>
  void tree_edge(typename boost::graph_traits<Graph>::edge_descriptor e, Graph& g) {
    typename boost::graph_traits<Graph>::vertex_descriptor u, v;
    u = boost::source(e, g);
    v = boost::target(e, g);
    distance[v] = distance[u] + 1;
  }
	
private:
  Distance distance;
};

//! An ease-of-use typedef to support distance calculations.
typedef std::vector<size_t> NetworkDistanceVector;

//! An ease-of-use typedef to support distance matrices.
typedef std::vector<NetworkDistanceVector> NetworkDistanceMatrix;  


/*! Is the passed-in network connected?
 */
template <typename Network>
bool network_is_connected(const Network& network) {
  std::vector<int> components(boost::num_vertices(network));
  return boost::connected_components(network, &components[0]) == 1;
}


/*! Are the two passed-in vertices connected?
 */
template <typename Network>
bool network_2cells_connected(typename Network::vertex_descriptor u, typename Network::vertex_descriptor v, Network& network) {
  NetworkDistanceVector dv(boost::num_vertices(network),0);
  NetworkDistanceVisitor<size_t*> visitor(&dv[0]);
  boost::breadth_first_search(network, u, boost::visitor(visitor));
  
  // If the distance from u to v is 0, there is no path.
  return dv[v] != 0;
}


/*! Calculate the distances between each pair of nodes; 0 == not connected.
 */
template <typename Network>
NetworkDistanceMatrix all_pairs_distances(const Network& network) {
	NetworkDistanceMatrix distance_matrix(boost::num_vertices(network),
                                        NetworkDistanceVector(boost::num_vertices(network), 0));
	
  for(size_t i=0; i<boost::num_vertices(network); ++i) {
    NetworkDistanceVisitor<size_t*> visitor(&distance_matrix[i][0]);
    typename Network::vertex_descriptor src = boost::vertices(network).first[i];
    boost::breadth_first_search(network, src, boost::visitor(visitor));
  }
  
  return distance_matrix;
}


/*! Calculate the characteristic path length of the passed-in network.  CPL is the
 mean distance between all pairs of vertices.
 */
template <typename Network>
double characteristic_path_length(const Network& network) {
	NetworkDistanceMatrix distance_matrix = all_pairs_distances(network);
  
  // Mean path length from v to v'.
  double cpl_sum=0.0;
  for(size_t i=0; i<boost::num_vertices(network); ++i) {
    cpl_sum += std::accumulate(distance_matrix[i].begin(), distance_matrix[i].end(), 0.0) / (boost::num_vertices(network)-1);
  }
  
  return cpl_sum / boost::num_vertices(network);
}


/*! Calculate the diameter of the passed-in network.
 */
template <typename Network>
size_t diameter(const Network& network) {
	NetworkDistanceMatrix distance_matrix = all_pairs_distances(network);
  
  // Mean path length from v to v'.
	NetworkDistanceVector max_distances;
  for(size_t i=0; i<boost::num_vertices(network); ++i) {
    max_distances.push_back(*std::max_element(distance_matrix[i].begin(), distance_matrix[i].end()));
  }
  
  return *std::max_element(max_distances.begin(), max_distances.end());
}


/*! Calculate the Euclidean distance between two vertices in a network.
 
 Vertices must define the following method:
   std::pair<double,double> location(),
 which returns the vertex's X-Y coordinates.
 */
template <typename Network>
double distance(typename Network::vertex_descriptor u, 
								typename Network::vertex_descriptor v, const Network& network) {
	std::pair<double,double> s,d;
	s = network[u].location();
	d = network[v].location();
	return sqrt(pow(s.first-d.first,2.0) + pow(s.second-d.second,2.0));
}


/*! Calculate the sum of all link lengths in the given network.
 
 See the distance() function above for requirements.
 */
template <typename Network>
double link_length_sum(const Network& network) {
	typename Network::edge_iterator ei,ei_end;
	double sum=0.0;
	
	for(tie(ei,ei_end)=edges(network); ei!=ei_end; ++ei) {
		sum += distance(source(*ei,network), target(*ei,network), network);
	}
	
	return sum;
}


/*! Calculate the clustering coefficient of the passed-in network.  Clustering coefficient
 is roughly a measure of "how many of my friends are themselves friends?"
 */
template <typename Network>
double clustering_coefficient(const Network& network) {
  // For each vertex in the graph, calculate the number of edges between vertices in the neighborhood.
  typename Network::vertex_iterator vi, vi_end;
  std::vector<double> cluster_coeffs;
  for(boost::tie(vi,vi_end)=vertices(network); vi!=vi_end; ++vi) {
    // Get the list of vertices which are in the neighborhood of vi.
    typedef typename Network::adjacency_iterator adjacency_iterator;
    std::pair<adjacency_iterator, adjacency_iterator> adjacent = boost::adjacent_vertices(*vi, network);
    typedef std::set<typename Network::vertex_descriptor> neighborhood;
    neighborhood neighbors;
    for(; adjacent.first!=adjacent.second; ++adjacent.first) {
      neighbors.insert(*adjacent.first);
    }
    
    if(neighbors.size() > 1) {
      // Now, count the edges between vertices in the neighborhood.
      unsigned int neighborhood_edge_count=0;
      for(typename neighborhood::iterator i=neighbors.begin(); i!=neighbors.end(); ++i) {
        typedef typename Network::out_edge_iterator out_edge_iterator;
        std::pair<out_edge_iterator,out_edge_iterator> oe = out_edges(*i,network);
        for(; oe.first!=oe.second; ++oe.first) {
          if(neighbors.find(target(*oe.first,network))!=neighbors.end()) {
            ++neighborhood_edge_count;
          }
        }
      }
      neighborhood_edge_count /= 2;
      cluster_coeffs.push_back((double)neighborhood_edge_count / (neighbors.size()*(neighbors.size()-1)/2));
    } else {
      // This used to be 1.0... but, if our neighborhood size is <1, there are zero edges out of a possible 0.
      cluster_coeffs.push_back(0.0);
    }
  }
  
  // Clustering coefficient:
  return std::accumulate(cluster_coeffs.begin(), cluster_coeffs.end(), 0.0) / cluster_coeffs.size();
}

#endif
