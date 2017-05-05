#include "graph.hpp"
#include<vector>
#include<iostream>
#include<queue>

void Graph::Node::InsertIncoming(int64 from) {
  incoming_->insert(from);
}

void Graph::Node::InsertOutgoing(int64 to) {
  outgoing_->insert(to);
}

void Graph::Node::EraseIncoming(int64 from) {
  incoming_->erase(from);
}

void Graph::Node::EraseOutgoing(int64 to) {
  outgoing_->erase(to);
}

bool Graph::Node::ContainsEdgeTo(int64 to) {
  // apparently the standard c++ way to check if something is in range
  // call this too many times to not make this a method...
  return outgoing_->find(to) != outgoing_->end();
}

bool Graph::Node::ContainsEdgeFrom(int64 from) {
  return incoming_->find(from) != incoming_->end();
}

bool Graph::Contains(int64 nodeid) {
  return nodemap_.find(nodeid) != nodemap_.end();
}

std::unique_ptr<Graph::Node> Graph::Node::DeepCopy() {
  // create a new node
  std::unique_ptr<Node> result = std::unique_ptr<Node>(new Node());
  // go through all the incoming and outgoing edges and insert them in the
  // result. That's all there is to the state of the node, since we are not
  // storing the id...
  for(int64 v : *outgoing_) {
    result->outgoing_->insert(v);
  }
  for(int64 v : *incoming_) {
    result->incoming_->insert(v);
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
  while(Contains(id_counter)) {
    id_counter = (id_counter % max_int64) + 1;
  }
  nodemap_.insert(std::make_pair(id_counter, std::unique_ptr<Node>(new Node())));
  return id_counter;
}

int64 Graph::AddNode(int64 id) {
  if(!Contains(id)) {
    nodemap_.insert(std::make_pair(id, std::unique_ptr<Node>(new Node())));
  }
  return id;
}

int64 Graph::Count() {
  // an invariant we mantain is that number of actual nodes == number of nodes in the map
  return nodemap_.size();
}

void Graph::Connect(int64 from, int64 to) {
  // check if both nodes are in the graph
  if(Contains(from) && Contains(to)) {
    // insert the edge
    nodemap_.at(from)->InsertOutgoing(to);
    nodemap_.at(to)->InsertIncoming(from);
  }
}

void Graph::Disconnect(int64 from, int64 to) {
  // check if both nodes are in the graph
  if(Contains(from) && Contains(to)) {
    // delete the edge
    nodemap_.at(from)->EraseOutgoing(to);
    nodemap_.at(to)->EraseIncoming(from);
  }
}

bool Graph::IsConnected(int64 from, int64 to) {
  bool result = false;
  if(Contains(from)) {
    result = nodemap_.at(from)->ContainsEdgeTo(to);
  }
  return result;
}

Graph Graph::DeepCopy() {
  // make a new graph and copy over all nodes. The nodes have their own deep
  // copy method
  auto result = Graph();
  for(auto& kv : nodemap_) {
    // k -> id, v -> node
    result.nodemap_.insert(std::make_pair(kv.first, kv.second->DeepCopy()));
  }
  return result;
}

void Graph::Reverse(Graph* graph_to_reverse) {
  // all this does is swap the incoming and outgoing sets for each node
  auto& nodemap_ = graph_to_reverse->nodemap_;
  for(std::pair<const int64, std::unique_ptr<Node>>& kv : nodemap_) {
    // k -> id, v -> node pointer. We don't care about k here...
    kv.second->outgoing_.swap(kv.second->incoming_);
  } 
}

void Graph::Delete(int64 node) {
  // if the node is in the graph... 
  if(Contains(node)) {
    // get the set of nodes which have an edge to it... 
    for(int64 id : *(nodemap_.at(node)->incoming_)) {
      // delete the pointer to this node from those nodes...
      nodemap_.at(id)->EraseOutgoing(node);
    }
    // and finally get rid of the node
    nodemap_.erase(node);
  }
}

// This is going to a simple breadth first search
std::vector<int64> Graph::ShortestPath(int64 from, int64 to) {
  std::vector<int64> result = std::vector<int64>();
  // no point doing anything if the nodes are not in the graph
  if(!Contains(from) || !Contains(to)) return result;
  // we need a way to keep track of things we have already visited
  std::unordered_set<int64> visited;
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
    for(int64 neighbor : *(nodemap_.at(current)->outgoing_)) {
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
