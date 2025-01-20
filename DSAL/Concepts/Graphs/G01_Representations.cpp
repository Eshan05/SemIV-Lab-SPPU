// 1. Graph representations: Edge list, adjacency matrix, adjacency list,
// inverse adjacency list, multilist.
// Demo: An undirected square with a chord, and a directed cycle, each inspected
// five ways. The multilist stores every edge once and links it at both
// endpoints, unlike two directed adjacency entries.

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

const int MAXV = 15;
const int MAXE = 210;

struct Edge {
  int u;
  int v;
};

static int n = 0;
static int edgeCount = 0;
static bool directed = false;

static int matrixG[MAXV][MAXV];
static vector<int> adj[MAXV];
static vector<int> invAdj[MAXV];

// The multilist keeps one node per edge with two incidence links. nextU and
// nextV chain the edges meeting each endpoint; head and headSide start that
// chain, where headSide says whether the vertex is the edge's u (0) or v (1).
static Edge edges[MAXE];
static int nextU[MAXE];
static int nextV[MAXE];
static int head[MAXV];
static int headSide[MAXV];

static void clearInput() {
  cin.clear();
  cin.ignore(1024, '\n');
}

static int readInt(const string &prompt) {
  int x;
  while (true) {
    cout << prompt;
    if (cin >> x) {
      cin.ignore(1024, '\n');
      return x;
    }
    // Out of input (Piped file or Ctrl+Z): Exit rather than retry forever.
    if (cin.eof()) {
      cout << "\nInput closed.\n";
      exit(0);
    }
    cout << "Invalid number. Try again.\n";
    clearInput();
  }
}

static void resetGraph() {
  for (int i = 0; i < MAXV; i++) {
    adj[i].clear();
    invAdj[i].clear();
    head[i] = -1;
    headSide[i] = 0;
    for (int j = 0; j < MAXV; j++) matrixG[i][j] = 0;
  }
  for (int e = 0; e < MAXE; e++) {
    nextU[e] = -1;
    nextV[e] = -1;
  }
  n = 0;
  edgeCount = 0;
}

static string edgeText(int e) {
  string s = "(" + to_string(edges[e].u);
  s += directed ? " -> " : " - ";
  s += to_string(edges[e].v) + ")";
  return s;
}

// Append edge e to the incidence list of vertex, updating nextU when the
// vertex is the edge's u endpoint and nextV when it is the v endpoint.
static void appendIncident(int vertex, int e) {
  if (head[vertex] == -1) {
    head[vertex] = e;
    headSide[vertex] = (edges[e].u == vertex) ? 0 : 1;
    return;
  }

  int cur = head[vertex];
  while (true) {
    int side = (edges[cur].u == vertex) ? 0 : 1;
    int nxt = (side == 0) ? nextU[cur] : nextV[cur];
    if (nxt == -1) {
      if (side == 0) nextU[cur] = e;
      else nextV[cur] = e;
      return;
    }
    cur = nxt;
  }
}

// Index of the stored edge between u and v, or -1. Undirected pairs are unordered.
static int findEdge(int u, int v) {
  for (int e = 0; e < edgeCount; e++) {
    if (edges[e].u == u && edges[e].v == v) return e;
    if (!directed && edges[e].u == v && edges[e].v == u) return e;
  }
  return -1;
}

static bool addEdge(int u, int v) {
  if (u < 0 || u >= n || v < 0 || v >= n) {
    cout << "  Rejected: Vertices must be in 0.." << (n - 1) << ".\n";
    return false;
  }
  if (u == v) {
    cout << "  Rejected: Self-loops are skipped in this demo.\n";
    return false;
  }

  int existing = findEdge(u, v);
  if (existing != -1) {
    cout << "  Rejected: E" << existing << " " << edgeText(existing)
         << " already exists.\n";
    if (!directed) {
      cout << "    Undirected edges are unordered: " << u << " - " << v
           << " is the same edge as " << v << " - " << u << ".\n";
    }
    return false;
  }
  if (edgeCount >= MAXE) {
    cout << "  Rejected: Edge limit reached.\n";
    return false;
  }

  int e = edgeCount;
  edges[e].u = u;
  edges[e].v = v;
  nextU[e] = -1;
  nextV[e] = -1;

  matrixG[u][v] = 1;
  adj[u].push_back(v);
  invAdj[v].push_back(u);

  if (!directed) {
    matrixG[v][u] = 1;
    adj[v].push_back(u);
    invAdj[u].push_back(v);
  }

  appendIncident(u, e);
  appendIncident(v, e);

  edgeCount++;
  cout << "  Added E" << e << " " << edgeText(e) << "\n";
  cout << "    matrix[" << u << "][" << v << "] = 1";
  if (!directed) cout << ", matrix[" << v << "][" << u << "] = 1";
  cout << "\n";
  cout << "    adj[" << u << "] += " << v;
  if (!directed) cout << ", adj[" << v << "] += " << u;
  cout << "\n";
  cout << "    inv[" << v << "] += " << u;
  if (!directed) cout << ", inv[" << u << "] += " << v;
  cout << "\n";
  cout << "    multilist: E" << e << " linked at both endpoints\n";
  return true;
}

static int readRange(const string &prompt, int lo, int hi) {
  while (true) {
    int x = readInt(prompt);
    if (x >= lo && x <= hi) return x;
    cout << "  Number must be in " << lo << ".." << hi << ".\n";
  }
}

static void buildGraph() {
  int mode = readRange("Graph type (1 = Undirected, 2 = Directed): ", 1, 2);
  int newN = readRange("Number of vertices (0.." + to_string(MAXV) + "): ", 0, MAXV);

  int maxEdges = (mode == 2) ? newN * (newN - 1) : newN * (newN - 1) / 2;
  int requested = readRange("Number of edges to enter (0.." +
                                to_string(maxEdges) + "): ",
                            0, maxEdges);

  resetGraph();
  directed = (mode == 2);
  n = newN;

  // Count stored edges, not attempts: A rejected pair retries the same slot.
  while (edgeCount < requested) {
    cout << "\nEdge " << (edgeCount + 1) << " of " << requested << "\n";
    int u = readInt("  From vertex (-1 to stop): ");
    if (u == -1) {
      cout << "  Stopped early.\n";
      break;
    }
    int v = readInt("  To vertex (-1 to stop): ");
    if (v == -1) {
      cout << "  Stopped early.\n";
      break;
    }
    if (!addEdge(u, v)) cout << "  Please re-enter this edge.\n";
  }

  cout << "\nBuild complete: " << n << (n == 1 ? " vertex, " : " vertices, ")
       << edgeCount << (edgeCount == 1 ? " edge" : " edges") << " stored";
  if (edgeCount < requested) cout << " (" << requested << " requested)";
  cout << ".\n";
}

static void loadDemoGraph(bool asDirected) {
  resetGraph();
  directed = asDirected;

  if (asDirected) {
    n = 4;
    cout << "Directed demo: 4 vertices.\n";
    cout << "  Cycle 0 -> 1 -> 2 -> 0, chord 0 -> 2, tail 2 -> 3.\n";
    cout << "  Adjacency lists outgoing arcs; the inverse list shows incoming.\n\n";

    int demoEdges[5][2] = {{0, 1}, {1, 2}, {2, 0}, {0, 2}, {2, 3}};
    for (int i = 0; i < 5; i++) addEdge(demoEdges[i][0], demoEdges[i][1]);
  } else {
    n = 6;
    cout << "Undirected demo: 6 vertices.\n";
    cout << "  Square 0 - 1 - 2 - 3 - 0 with diagonal 0 - 2, plus separate edge 4 - 5.\n";
    cout << "  The 4-5 edge shows a second connected component.\n\n";

    int demoEdges[6][2] = {{0, 1}, {1, 2}, {2, 3}, {3, 0}, {0, 2}, {4, 5}};
    for (int i = 0; i < 6; i++) addEdge(demoEdges[i][0], demoEdges[i][1]);
  }

  cout << "\nDemo loaded: " << n << " vertices, " << edgeCount
       << (edgeCount == 1 ? " edge" : " edges") << " stored.\n";
}

static void addEdgeInteractive() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }

  cout << "\nAdd edge (V = " << n << ", so vertices 0.." << (n - 1) << ")\n";
  while (true) {
    int u = readInt("  From vertex (-1 to cancel): ");
    if (u == -1) {
      cout << "  Cancelled.\n";
      return;
    }
    int v = readInt("  To vertex (-1 to cancel): ");
    if (v == -1) {
      cout << "  Cancelled.\n";
      return;
    }
    if (addEdge(u, v)) {
      cout << "  E = " << edgeCount << (edgeCount == 1 ? " edge" : " edges")
           << " now.\n";
      return;
    }
    cout << "  Please re-enter this edge.\n";
  }
}

static void showEdgeList() {
  cout << "\n--- Edge List (E = " << edgeCount << ") ---\n";
  if (edgeCount == 0) {
    cout << "(Empty)\n";
    return;
  }
  for (int e = 0; e < edgeCount; e++) {
    cout << "E" << e << ": " << edgeText(e) << "\n";
  }
}

static void showMatrix() {
  cout << "\n--- Adjacency Matrix (" << n << " x " << n << ") ---\n";
  if (n == 0) {
    cout << "(Empty)\n";
    return;
  }

  cout << "    ";
  for (int j = 0; j < n; j++) cout << setw(3) << j;
  cout << "\n";

  for (int i = 0; i < n; i++) {
    cout << setw(3) << i << " ";
    for (int j = 0; j < n; j++) cout << setw(3) << matrixG[i][j];
    cout << "\n";
  }

  cout << (directed ? "matrix[u][v] = 1 means u -> v.\n"
                    : "Symmetric, so matrix[u][v] = matrix[v][u] = 1.\n");
}

static void showAdjacencyList() {
  cout << "\n--- Adjacency List (" << (directed ? "Outgoing" : "Neighbours")
       << ") ---\n";
  if (n == 0) {
    cout << "(Empty)\n";
    return;
  }

  for (int i = 0; i < n; i++) {
    cout << setw(3) << i << ":";
    if (adj[i].empty()) cout << " (None)";
    else
      for (int v : adj[i]) cout << " " << v;
    cout << "\n";
  }
}

static void showInverseList() {
  cout << "\n--- Inverse Adjacency List ("
       << (directed ? "Incoming" : "Same as adjacency for undirected") << ") ---\n";
  if (n == 0) {
    cout << "(Empty)\n";
    return;
  }

  for (int i = 0; i < n; i++) {
    cout << setw(3) << i << ":";
    if (invAdj[i].empty()) cout << " (None)";
    else
      for (int v : invAdj[i]) cout << " " << v;
    cout << "\n";
  }
}

static void showMultilist() {
  cout << "\n--- Multilist (Edge nodes + incidence links) ---\n";
  if (n == 0) {
    cout << "(Empty)\n";
    return;
  }

  cout << "Vertex heads:\n";
  for (int i = 0; i < n; i++) {
    cout << "  V" << i << " -> ";
    if (head[i] == -1) cout << "NULL";
    else cout << "E" << head[i] << " (" << (headSide[i] == 0 ? "u" : "v")
              << " side)";
    cout << "\n";
  }

  cout << "\nEdge nodes (Each stores both endpoints and two links):\n";
  for (int e = 0; e < edgeCount; e++) {
    cout << "  E" << e << ": " << edgeText(e)
         << " nextAtU=" << (nextU[e] == -1 ? "NULL" : "E" + to_string(nextU[e]))
         << " nextAtV=" << (nextV[e] == -1 ? "NULL" : "E" + to_string(nextV[e]))
         << "\n";
  }

  cout << "\nIncidence traversal (Follow the links at each vertex):\n";
  for (int i = 0; i < n; i++) {
    cout << "  V" << i << ":";
    int e = head[i];
    int side = headSide[i];
    int count = 0;

    while (e != -1) {
      cout << " E" << e << edgeText(e);
      int nxt = (side == 0) ? nextU[e] : nextV[e];
      e = nxt;
      if (e != -1) side = (edges[e].u == i) ? 0 : 1;
      count++;
    }

    if (count == 0) cout << " (None)";
    cout << "\n";
  }

  cout << (directed ? "Directed: Each edge is linked at its source (At u side) and "
                      "destination (At v side).\n"
                    : "Undirected: Each edge is linked once at each endpoint; "
                      "O(V + E) space.\n");
}

static void showAll() {
  showEdgeList();
  showMatrix();
  showAdjacencyList();
  showInverseList();
  showMultilist();
}

static void showComparison() {
  int entries = directed ? edgeCount : 2 * edgeCount;

  cout << "\n--- Storage Comparison (V = " << n << ", E = " << edgeCount
       << ") ---\n";
  cout << left << setw(22) << "Representation" << setw(16) << "Space"
       << "Best at\n";
  cout << string(64, '-') << "\n";
  cout << setw(22) << "Edge list" << setw(16) << (to_string(2 * edgeCount) + " ints")
       << "Kruskal, edge iteration\n";
  cout << setw(22) << "Adjacency matrix" << setw(16) << (to_string(n * n) + " cells")
       << "O(1) edge test; dense graphs\n";
  cout << setw(22) << "Adjacency list" << setw(16) << (to_string(n + entries) + " slots")
       << "Sparse graphs, neighbour scans\n";
  cout << setw(22) << "Inverse list" << setw(16) << (to_string(n + entries) + " slots")
       << "Incoming edges (Directed)\n";
  cout << setw(22) << "Multilist" << setw(16)
       << (to_string(n + 4 * edgeCount) + " fields")
       << "Edge-centric updates\n";

  cout << "\nComplexity notes:\n";
  cout << "  Edge list   : Find O(E), iterate O(E), insert O(1) append.\n";
  cout << "  Matrix      : Edge test O(1), neighbours O(V), space O(V^2).\n";
  cout << "  Adj. list   : Edge test O(deg), neighbours O(deg), space O(V +"
       << (directed ? " E).\n" : " 2E).\n");
  cout << "  Inverse list: Same costs, but answers 'who points to me?'.\n";
  cout << "  Multilist   : Incidence scan O(deg); deletion must unlink the edge\n";
  cout << "                from two lists, but the edge node itself is shared.\n";
  cout << "\nNote: The matrix is convenient for small or dense graphs; the list\n";
  cout << "representations are better when E is much smaller than V^2.\n";
  cout << right;
}

int main() {
  resetGraph();

  while (true) {
    cout << "\n=== G01: Graph Representations ===\n";
    cout << "Graph: " << (directed ? "Directed" : "Undirected")
         << " | V = " << n << ", E = " << edgeCount << "\n";
    cout << "1. Build / rebuild graph\n";
    cout << "2. Load undirected demo (6 vertices)\n";
    cout << "3. Load directed demo (4 vertices)\n";
    cout << "4. Add an edge to the current graph\n";
    cout << "5. Show edge list\n";
    cout << "6. Show adjacency matrix\n";
    cout << "7. Show adjacency list\n";
    cout << "8. Show inverse adjacency list\n";
    cout << "9. Show multilist\n";
    cout << "10. Show all representations\n";
    cout << "11. Compare storage costs\n";
    cout << "12. Exit\n";

    int choice = readInt("Select: ");

    if (choice == 1) {
      buildGraph();
    } else if (choice == 2) {
      loadDemoGraph(false);
    } else if (choice == 3) {
      loadDemoGraph(true);
    } else if (choice == 4) {
      addEdgeInteractive();
    } else if (choice == 5) {
      showEdgeList();
    } else if (choice == 6) {
      showMatrix();
    } else if (choice == 7) {
      showAdjacencyList();
    } else if (choice == 8) {
      showInverseList();
    } else if (choice == 9) {
      showMultilist();
    } else if (choice == 10) {
      showAll();
    } else if (choice == 11) {
      showComparison();
    } else if (choice == 12) {
      cout << "Exiting...\n";
      break;
    } else {
      cout << "Invalid option.\n";
    }
  }

  return 0;
}
