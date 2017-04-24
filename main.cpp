#define CATCH_CONFIG_MAIN
#include<climits>
#include<vector>
#include<limits>
#include<memory>
#include "Catch-master/include/catch.hpp"
#include "graph.hpp" 

TEST_CASE( "Doing operations on an empty graph", "[empty]" ) {
    std::unique_ptr<Graph> graph = std::make_unique<Graph>();
    REQUIRE( graph->Count() == 0 );

    SECTION( "Disconnecting in an empty graph" ) {
        graph->Disconnect(1, 2);
        REQUIRE( graph->Count() == 0);
    }

    SECTION( "Checking connections on an empty graph" ) {
        REQUIRE( !graph->IsConnected(1, 2) );
    }
    
    SECTION( "Deleting nodes" ) {
        graph->Delete(0);
        graph->Delete(-1);
        graph->Delete(1);
        REQUIRE( graph->Count() == 0 );
    }

    SECTION( "Deep copying the graph and checking the previoius properties" ) {
        auto copy = graph->DeepCopy();
        REQUIRE( copy.Count() == 0 );
        REQUIRE( !graph->IsConnected(1, 2) );
    }
    
    SECTION( "Reversing the graph and checking the previous properties" ) {
        auto emptyReversed = graph->DeepCopy();
        REQUIRE( emptyReversed.Count() == 0 );
        REQUIRE( !emptyReversed.IsConnected(1, 2) );
    }

    SECTION( "Shortest path on an empty graph" ) {
        auto pathVec = graph->ShortestPath(1, 2);
        REQUIRE(pathVec.empty());
    }
}

TEST_CASE( "Testing add/delete", "[AddDelete]" ) {
    std::unique_ptr<Graph> graph = std::make_unique<Graph>();
    REQUIRE( graph->Count() == 0);
    int64 from = graph->AddNode();
    int64 to = graph->AddNode();
    REQUIRE( graph->Count() == 2);
    SECTION( "checking connections on deletion" ) {
        graph->Connect(from, to);
        REQUIRE( graph->IsConnected(from, to) );
        graph->Delete(to);
        REQUIRE( !graph->IsConnected(from, to) );
    }
    SECTION( "checking shortest path on deletion" ) {
        graph->Connect(from, to);
        std::vector<int64> path = graph->ShortestPath(from, to);
        // shortest path length == number of nodes on the path,
        // inclusive of "from" and "to"
        REQUIRE( path.size() == 2 );
        // disconnect
        graph->Disconnect(from, to);
        REQUIRE( graph->ShortestPath(from, to).size() == 0 );
        // connect again
        graph->Connect(from, to);
        REQUIRE( graph->ShortestPath(from, to).size() == 2 );
        // delete the destination
        graph->Delete(to);
        path = graph->ShortestPath(from, to);
        REQUIRE( path.size() == 0);
    }
}

std::unique_ptr<Graph> make_graph(std::string filepath) {
  unique_ptr<Graph> result = std::make_unique<Graph>();
  std::ifstream input_file;
  input_file.open(filepath);
  int m;
  for(int i = 0; i < m; ++i) {
    int64 from, to;
    input_file >> from >> to;
    if(!result.Connect(from)
  }
}

