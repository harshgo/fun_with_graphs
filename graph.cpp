#include "graph.hpp"
#include<vector>
#include<iostream>
#include<queue>

void Graph::Node::insertIncoming(int64 from) {
  incoming->insert(from);
}

void Graph::Node::insertOutgoing(int64 to) {
  outgoing->insert(to);
}

void Graph::Node::eraseIncoming(int64 from) {
  incoming->erase(from);
}

void Graph::Node::eraseOutgoing(int64 to) {
  outgoing->erase(to);
}

bool Graph::Node::containsEdgeTo(int64 to) {
  // apparently the standard c++ way to check if something is in range
  // call this too many times to not make this a method...
  return outgoing->find(to) != outgoing->end();
}

bool Graph::Node::containsEdgeFrom(int64 from) {
  return incoming->find(from) != incoming->end();
}

bool Graph::Contains(int64 nodeid) {
  return nodemap.find(nodeid) != nodemap.end();
}

Graph::Node Graph::Node::DeepCopy() {
  // create a new node
  auto result = Node();
  // go through all the incoming and outgoing edges and insert them in the
  // result. That's all there is to the state of the node, since we are not
  // storing the id...
  for(auto v : *outgoing) {
    result.outgoing->insert(v);
  }
  for(auto v : *incoming) {
    result.incoming->insert(v);
  }
  return result;
}

int64 Graph::AddNode() {
  // The counter is keeping track of the ids that have been assigned so far.
  static int64 id_counter = 0;
  static int64 max_int64 = std::numeric_limits<int64>::max();
  
  // The only reason we are doing mod maxint64 and checking that
  // the id already exists is on the off chance that about
  // maxint64 addNodes might have already happened on this graph.
  // Probably not going to happen, but good karma, right?
  
  while(Contains(id_counter)) id_counter = (id_counter % max_int64) + 1;
  nodemap.insert({id_counter, Node()});
  return id_counter;
}

int64 Graph::AddNode(int64 id) {
  if(!Contains(id)) {
    nodemap.insert({id, Node()});
  }
  return id;
}

int64 Graph::Count() {
  // an invariant we mantain is that number of actual nodes == number of nodes in the map
  return nodemap.size();
}

void Graph::Connect(int64 from, int64 to) {
  // check if both nodes are in the graph
  if(Contains(from) && Contains(to)) {
    // insert the edge
    nodemap.at(from).insertOutgoing(to);
    nodemap.at(to).insertIncoming(from);
  }
}

void Graph::Disconnect(int64 from, int64 to) {
  // check if both nodes are in the graph
  if(Contains(from) && Contains(to)) {
    // delete the edge
    nodemap.at(from).eraseOutgoing(to);
    nodemap.at(to).eraseIncoming(from);
  }
}

bool Graph::IsConnected(int64 from, int64 to) {
  bool result = false;
  if(Contains(from)) {
    result = nodemap.at(from).containsEdgeTo(to);
  }
  return result;
}

Graph Graph::DeepCopy() {
  // make a new graph and copy over all nodes. The nodes have their own deep
  // copy method
  auto result = Graph();
  for(auto& kv : nodemap) {
    // k -> id, v -> node
    result.nodemap.insert({kv.first, kv.second.DeepCopy()});
  }
  return result;
}

void Graph::Reverse(Graph* graph_to_reverse) {
  // all this does is swap the incoming and outgoing sets for each node
  auto& nodemap = graph_to_reverse->nodemap;
  for(auto& kv : nodemap) {
    // k -> id, v -> node. We don't care about k here...
    Graph::Node& node = kv.second;
    std::shared_ptr<std::set<int64> > temp = node.outgoing;
    node.outgoing = node.incoming;
    node.incoming = temp;
  } 
}

void Graph::Delete(int64 node) {
  // if the node is in the graph... 
  if(Contains(node)) {
    // get the set of nodes which have an edge to it... 
    std::shared_ptr<std::set<int64> > incoming = nodemap.at(node).incoming;
    for(auto it = incoming->begin(); it != incoming->end(); ++it) {
      // delete the pointer to this node from those nodes...
      nodemap.at(*it).eraseOutgoing(node);
    }
    // and finally get rid of the node
    nodemap.erase(node);
  }
}

// This is going to a simple breadth first search
std::vector<int64> Graph::ShortestPath(int64 from, int64 to) {
  auto result = std::vector<int64>();
  // no point doing anything if the nodes are not in the graph
  if(!Contains(from) || !Contains(to)) return result;
  // we need a way to keep track of things we have already visited
  std::set<int64> visited;
  // and a way to keep track of how we visited them, so each node keeps
  // track of where it came from
  std::map<int64, int64> backpointers;
  // just for consistency...
  backpointers.insert({from, -1});
  // a queue to visit the nodes in the right order
  std::queue<int64> q;
  q.push(from);
  // while the queue is not empty...
  while(!q.empty()) {
    // get the most current element...
    int64 current = q.front();
    visited.insert(current);
    q.pop();
    // if we have reached the destination, we are done
    if(current == to) break;
    // else, we add all our neighbors to the queue
    Node currNode = nodemap.at(current);
    for(int64 neighbor : *currNode.outgoing) {
      // do not need to do anything if the neighbor has been visited before
      if(visited.find(neighbor) == visited.end()) {
        backpointers.insert({neighbor, current});
        q.push(neighbor);
      }
    }
  }
  // first check if we ever reached the end
  if(visited.find(to) != visited.end()) {
    // then get the length of the shortest path
    // the reason for doing this is so that we know where to start filling
    // up the vector and do not have to do expensive inserts.
    int64 current = to;
    int64 shortestPathLength = 1;
    while(current != from) {
      shortestPathLength += 1;
      current = backpointers.at(current);
    }
    // now insert the result in the vector
    result.resize(shortestPathLength);
    current = to;
    for(int i = shortestPathLength - 1; i >= 0; --i) {
      result[i] = current;
      current = backpointers.at(current);
    }
  }
  return result;
}
