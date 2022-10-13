#include <thread>
#include "cCommandParser.h"
#include "cTCPex.h"

std::string myServerAddress;
std::string myServerPort;

raven::set::cTCPex tcpex;

void parse_command_line_options(int argc, char *argv[])
{
    raven::set::cCommandParser P;
    P.add("a", "address of server");
    P.add("p", "port where server listens for clients");

    P.parse(argc, argv);

    myServerAddress = P.value("a");
    myServerPort = P.value("p");

    if (myServerAddress.empty() || myServerPort.empty())
    {
        P.describe();
        exit(1);
    }
}

/// @brief handle input from keyboard by sending to server, then waiting for another line
void inputHandler()
{
    char name[1024];
    std::string myString;

    while (1)
    {
        if (!myString.empty())
        {
            // user has typed something - send it to the server
            tcpex.send(myString);

            // std::cout << theCIN.myString;
        }

        // wait for more typing
        std::cout << "type something: ";
        std::cin.getline(name, 1024);
        myString = name + std::string("\n");
    }
}

std::string msgProcessor(
    int client,
    const std::string &msg)
{
    std::cout << "\nfrom server: " << msg << "\n";
    return "";
}
void eventProcessor(
    int client,
    raven::set::cTCPex::eEvent type,
    const std::string &msg)
{
}

void connect_to_server()
{
    // loop until connection made
    while (1)
    {
        if (tcpex.connect_to_server(
                myServerAddress,
                myServerPort,
                std::bind(
                    &eventProcessor,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3),
                std::bind(
                    &msgProcessor,
                    std::placeholders::_1,
                    std::placeholders::_2)))
            break;

        // connection failed, wait 1 second then try again
        std::this_thread::sleep_for(
            std::chrono::seconds(1));
    }
    std::cout << "connected to server\n";
}

#include <list>
using namespace std;

class Graph
{
public:
    // each node has its own adjacency list
    map<int, list<int>> adj;
    // checking if each node has been visited or not
    vector<bool> visited;

    void addEdge(int n1, int n2);

    void DFS(int v, int w);
};

void Graph::addEdge(int n1, int n2)
{
    // add each other to their own adjacency list
    adj[n1].push_back(n2);
    adj[n2].push_back(n1);

    visited.resize( adj.size(), false );

}

void Graph::DFS(int par, int cur)
{

    // mark current node as visited
    visited[cur] = true;

    cout << "DFS " << cur << "\n";

    // for each node adjacent
    for (auto &i : adj[cur])
    {
        // check for back edges just once
        if (i < cur)
        {
            //  if it has been visited and it is not the parent node (detects a cycle)
            if (visited[i] && i != par)
            {
                cout << "Back edge " << cur << " " << i << "\n";
                cout << "Cycle detected! | ";
                cout << "Adjacent node: " << i << " | ";
                cout << "Parent node: " << par << "\n";
            }
        }

        // run DFS from unvisited node
        if (!visited[i])
        {
            DFS(cur, i);
        }
    }
}

void test()
{
    // create graph
    Graph graph;
    graph.addEdge(0, 1);
    graph.addEdge(1, 2);
    graph.addEdge(1, 3);
    graph.addEdge(2, 5);
    graph.addEdge(2, 3);
    graph.addEdge(3, 4);
    graph.addEdge(4, 6);

    // run DFS on the first node
    graph.DFS(0, 0);
}

main(int argc, char *argv[])
{
    test();

    // parse_command_line_options(argc, argv);

    // connect_to_server();

    // inputHandler();

    // return 0;
}
