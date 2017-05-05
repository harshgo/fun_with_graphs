#ifndef GRAPH_H_INCLUDE
#define GRAPH_H_INCLUDE
#include<vector>
#include<map>
#include<unordered_set>
#include<memory>

typedef long long int64;

class Graph {

 public:
  // Reverse all the connections in the graph. This should make it
  // so that original.IsConnected(a, b) = true if and only if
  // reversed.IsConnected(b, a) = true. I made this method static just
  // so you have some exposure to a static method. You should reverse
  // the graph that's being passed in. See the Google style guide for
  // why we're passing this in as a point64er (*) instead of a reference
  // (&) This might be super annoying so save it for last and feel free
  // to request a code review before you've implemented this.
  static void Reverse(Graph* graph_to_reverse);

  // Adds a new node to the graph and returns a unique
  // identifier for that node.
  int64 AddNode();

  // Adds a new node to the graph with the specified id. if a node already
  // exists with that id, does nothing
  int64 AddNode(int64 id);

  // Returns the number of nodes in the graph.
  int64 Count();

  // Takes in the unique identifiers of two nodes and
  // connects them. This is a directed graph, so this function is
  // not commutative (i.e. Connect(1, 2) != Connect(2, 1)).
  void Connect(int64 from, int64 to);

  // Removes a connection from one node to another. If no
  // connection existed before, does nothing. This should also
  // be directional, so if there's a two-way connection between
  // two nodes, calling this method once should only remove one
  // of them.
  void Disconnect(int64 from, int64 to);

  // Returns true if there's a connection from -> to.
  bool IsConnected(int64 from, int64 to);

  // Returns a deep copy of the current graph. This copy should
  // have the following property:
  // 1) copy.IsConnected(a, b) should return the same thing as
  //    original.IsConnected(a, b) for all a, b.
  Graph DeepCopy();

  // Deletes a node and all of its incoming and outgoing connections.
  void Delete(int64 node);

  // Return the shortest path between two nodes. If no shortest
  // path exists, return an empty vector.
  std::vector<int64> ShortestPath(int64 from, int64 to);


 private:
  // Nested class representing node in the graph to manage connections
  class Node {
    // Since this class is not used anywhere else, OK to make it a friend
    // These represent incoming and outgoing edges.
    friend class Graph;
   public:
    Node() :
      outgoing_(new std::unordered_set<int64>()),
      incoming_(new std::unordered_set<int64>()) {}
    // A bunch of mutator methods to add/delete incoming/outgoing edges from
    // node
    void InsertOutgoing(int64 to);
    void InsertIncoming(int64 from);
    void EraseOutgoing(int64 to);
    void EraseIncoming(int64 from);
    // Two accessor methods. Return true if there exists an incoming/outgoing
    // edge
    bool ContainsEdgeFrom(int64 from);
    bool ContainsEdgeTo(int64 to);
    std::unique_ptr<Node> DeepCopy();
   private:
    // Reasons for having both sets:
    // 1. When a node is deleted, this makes it finding out which edges to
    //    delete much easier
    // 2. This makes reverse a lot easier
    std::unique_ptr<std::unordered_set<int64>> outgoing_;
    std::unique_ptr<std::unordered_set<int64>> incoming_;
  };

  // method to check if there is a certain node in the graph
  bool Contains(int64 nodeID);

  // collection of nodes, mapping ids to nodes
  std::map<int64, std::unique_ptr<Node>> nodemap_;

};

#endif
