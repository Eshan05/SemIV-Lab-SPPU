// 2. Graph CRUD + basic properties: Degrees, #V, #E, density, regular,
// complete, connected.
// Demo: Load a graph, edit it, and watch the counts and properties react.

#include <algorithm>
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

// The edge list is the source of truth. The matrix and lists are rebuilt from
// it after a deletion, which keeps every view consistent.
static Edge edges[MAXE];

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

static int readRange(const string &prompt, int lo, int hi) {
  while (true) {
    int x = readInt(prompt);
    if (x >= lo && x <= hi) return x;
    cout << "  Number must be in " << lo << ".." << hi << ".\n";
  }
}

static void resetGraph() {
  for (int i = 0; i < MAXV; i++) {
    adj[i].clear();
    invAdj[i].clear();
    for (int j = 0; j < MAXV; j++) matrixG[i][j] = 0;
  }
  n = 0;
  edgeCount = 0;
}

static string edgeLabel(int u, int v) {
  string s = "(" + to_string(u);
  s += directed ? " -> " : " - ";
  s += to_string(v) + ")";
  return s;
}

static string edgeText(int e) {
  return edgeLabel(edges[e].u, edges[e].v);
}

static void insertDerived(int u, int v) {
  matrixG[u][v] = 1;
  adj[u].push_back(v);
  if (directed) {
    invAdj[v].push_back(u);
  } else {
    matrixG[v][u] = 1;
    adj[v].push_back(u);
  }
}

static void rebuild() {
  for (int i = 0; i < MAXV; i++) {
    adj[i].clear();
    invAdj[i].clear();
    for (int j = 0; j < MAXV; j++) matrixG[i][j] = 0;
  }
  for (int e = 0; e < edgeCount; e++) {
    insertDerived(edges[e].u, edges[e].v);
  }
}

static int findEdge(int u, int v) {
  for (int e = 0; e < edgeCount; e++) {
    if (edges[e].u == u && edges[e].v == v) return e;
    if (!directed && edges[e].u == v && edges[e].v == u) return e;
  }
  return -1;
}

static int outDegree(int v) {
  return (int)adj[v].size();
}

static int inDegree(int v) {
  return directed ? (int)invAdj[v].size() : (int)adj[v].size();
}

static int totalDegree(int v) {
  return directed ? outDegree(v) + inDegree(v) : outDegree(v);
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

  edges[edgeCount].u = u;
  edges[edgeCount].v = v;
  insertDerived(u, v);

  int e = edgeCount;
  edgeCount++;
  cout << "  Added E" << e << " " << edgeLabel(u, v) << "\n";
  cout << "    matrix[" << u << "][" << v << "] = 1";
  if (!directed) cout << ", matrix[" << v << "][" << u << "] = 1";
  cout << "\n";
  cout << "    adj[" << u << "] += " << v;
  if (directed) cout << ", inv[" << v << "] += " << u;
  else cout << ", adj[" << v << "] += " << u;
  cout << "\n";
  if (directed) {
    cout << "    out[" << u << "] = " << outDegree(u) << ", in[" << v
         << "] = " << inDegree(v) << "\n";
  } else {
    cout << "    deg[" << u << "] = " << outDegree(u) << ", deg[" << v
         << "] = " << outDegree(v) << "\n";
  }
  return true;
}

static void buildGraph() {
  int mode = readRange("Graph type (1 = Undirected, 2 = Directed): ", 1, 2);
  int newN = readRange("Number of vertices (0.." + to_string(MAXV) + "): ",
                       0, MAXV);

  int possible = (mode == 2) ? newN * (newN - 1) : newN * (newN - 1) / 2;
  int requested = readRange("Number of edges to enter (0.." +
                                to_string(possible) + "): ",
                            0, possible);

  resetGraph();
  directed = (mode == 2);
  n = newN;

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

static void loadDemo(bool asDirected) {
  resetGraph();
  directed = asDirected;

  if (asDirected) {
    n = 4;
    cout << "Directed demo: 4 vertices.\n";
    cout << "  Cycle 0 -> 1 -> 2 -> 0, chord 0 -> 2, tail 2 -> 3.\n";
    cout << "  Weakly connected, not regular, not complete.\n\n";

    int demoEdges[5][2] = {{0, 1}, {1, 2}, {2, 0}, {0, 2}, {2, 3}};
    for (int i = 0; i < 5; i++) addEdge(demoEdges[i][0], demoEdges[i][1]);
  } else {
    n = 6;
    cout << "Undirected demo: 6 vertices.\n";
    cout << "  Square 0 - 1 - 2 - 3 - 0 with diagonal 0 - 2, plus edge 4 - 5.\n";
    cout << "  Two components, so connected is No until an edge joins 4-5 to the\n";
    cout << "  square.\n\n";

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

static void removeEdgeInteractive() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }

  cout << "\nRemove edge (V = " << n << ", so vertices 0.." << (n - 1) << ")\n";
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
    if (u < 0 || u >= n || v < 0 || v >= n) {
      cout << "  Vertices must be in 0.." << (n - 1) << ".\n";
      cout << "  Please re-enter this edge.\n";
      continue;
    }

    int idx = findEdge(u, v);
    if (idx == -1) {
      cout << "  Edge not found.\n";
      cout << "  Please re-enter this edge.\n";
      continue;
    }

    Edge old = edges[idx];
    for (int e = idx; e < edgeCount - 1; e++) edges[e] = edges[e + 1];
    edgeCount--;
    rebuild();

    cout << "  Removed E" << idx << " " << edgeLabel(old.u, old.v) << "\n";
    cout << "    matrix[" << old.u << "][" << old.v << "] = 0";
    if (!directed) cout << ", matrix[" << old.v << "][" << old.u << "] = 0";
    cout << "\n";
    cout << "    adj[" << old.u << "] -= " << old.v;
    if (directed) cout << ", inv[" << old.v << "] -= " << old.u;
    else cout << ", adj[" << old.v << "] -= " << old.u;
    cout << "\n";
    cout << "  E = " << edgeCount << (edgeCount == 1 ? " edge" : " edges")
         << " now.\n";
    return;
  }
}

static void addVertex() {
  if (n >= MAXV) {
    cout << "Vertex limit reached (" << MAXV << ").\n";
    return;
  }

  n++;
  cout << "Added V" << (n - 1) << ". V = " << n << ".\n";
}

static void removeVertexInteractive() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }

  int v;
  while (true) {
    v = readInt("  Vertex to remove (-1 to cancel): ");
    if (v == -1) {
      cout << "  Cancelled.\n";
      return;
    }
    if (v >= 0 && v < n) break;
    cout << "  Vertex must be in 0.." << (n - 1) << ".\n";
  }

  int removed = 0;
  int keep = 0;
  for (int e = 0; e < edgeCount; e++) {
    if (edges[e].u == v || edges[e].v == v) {
      removed++;
      continue;
    }
    edges[keep] = edges[e];
    if (edges[keep].u > v) edges[keep].u--;
    if (edges[keep].v > v) edges[keep].v--;
    keep++;
  }
  edgeCount = keep;
  n--;
  rebuild();

  cout << "Removed V" << v << " and " << removed
       << (removed == 1 ? " incident edge.\n" : " incident edges.\n");
  if (v < n) cout << "  Vertices above V" << v << " shift down by one.\n";
  cout << "  V = " << n << ", E = " << edgeCount << ".\n";
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

static void showAdjacencyLists() {
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

  if (directed) {
    cout << "\n--- Inverse Adjacency List (Incoming) ---\n";
    for (int i = 0; i < n; i++) {
      cout << setw(3) << i << ":";
      if (invAdj[i].empty()) cout << " (None)";
      else
        for (int v : invAdj[i]) cout << " " << v;
      cout << "\n";
    }
  }
}

static void showDegrees() {
  cout << "\n--- Degrees ---\n";
  if (n == 0) {
    cout << "(Empty)\n";
    return;
  }

  if (directed) {
    cout << "  V   Out  In  Total\n";
    for (int i = 0; i < n; i++) {
      cout << setw(3) << i << setw(5) << outDegree(i) << setw(4) << inDegree(i)
           << setw(7) << totalDegree(i) << "\n";
    }

    vector<int> outs;
    vector<int> ins;
    for (int i = 0; i < n; i++) {
      outs.push_back(outDegree(i));
      ins.push_back(inDegree(i));
    }
    sort(outs.begin(), outs.end(), greater<int>());
    sort(ins.begin(), ins.end(), greater<int>());
    cout << "  Out sequence:";
    for (int x : outs) cout << " " << x;
    cout << "\n  In sequence:";
    for (int x : ins) cout << " " << x;
    cout << "\n";
  } else {
    vector<int> degs;
    int sum = 0;
    for (int i = 0; i < n; i++) {
      cout << "  V" << i << ": " << outDegree(i) << "\n";
      degs.push_back(outDegree(i));
      sum += outDegree(i);
    }
    sort(degs.begin(), degs.end(), greater<int>());
    cout << "  Degree sequence:";
    for (int x : degs) cout << " " << x;
    cout << "\n  Min " << degs.back() << ", max " << degs.front()
         << ", average " << fixed << setprecision(2)
         << (n == 0 ? 0.0 : (double)sum / n) << "\n";
    cout << defaultfloat;
  }
}

static bool isRegular() {
  if (n <= 1) return true;
  if (directed) {
    int out0 = outDegree(0);
    int in0 = inDegree(0);
    for (int i = 1; i < n; i++) {
      if (outDegree(i) != out0 || inDegree(i) != in0) return false;
    }
  } else {
    int deg0 = outDegree(0);
    for (int i = 1; i < n; i++) {
      if (outDegree(i) != deg0) return false;
    }
  }
  return true;
}

static void showCounts() {
  int possible = directed ? n * (n - 1) : n * (n - 1) / 2;
  double density = possible ? (double)edgeCount / possible : 0.0;

  cout << "\n--- Counts and Density ---\n";
  cout << "  V = " << n << ", E = " << edgeCount << "\n";
  cout << "  Possible edges: " << possible << "\n";
  cout << "  Density: " << edgeCount << " / " << possible << " = " << fixed
       << setprecision(3) << density << "\n";
  cout << defaultfloat;

  if (n == 0) {
    cout << "  Regular: Yes (No vertices)\n";
    cout << "  Complete: Yes (No vertices)\n";
    return;
  }

  int minOut = outDegree(0);
  int maxOut = outDegree(0);
  int minIn = inDegree(0);
  int maxIn = inDegree(0);
  for (int i = 1; i < n; i++) {
    minOut = min(minOut, outDegree(i));
    maxOut = max(maxOut, outDegree(i));
    minIn = min(minIn, inDegree(i));
    maxIn = max(maxIn, inDegree(i));
  }

  if (directed) {
    cout << "  Regular: " << (isRegular() ? "Yes" : "No")
         << " (Out " << minOut << ".." << maxOut << ", In " << minIn << ".."
         << maxIn << ")\n";
  } else {
    cout << "  Regular: " << (isRegular() ? "Yes" : "No") << " (Degrees "
         << minOut << ".." << maxOut << ")\n";
  }

  cout << "  Complete: " << (edgeCount == possible ? "Yes" : "No")
       << " (" << edgeCount << " of " << possible << " edges)\n";
}

static int reachableCount() {
  if (n == 0) return 0;
  bool seen[MAXV] = {false};
  int queue[MAXV];
  int front = 0;
  int back = 0;

  queue[back++] = 0;
  seen[0] = true;
  int count = 0;

  while (front < back) {
    int u = queue[front++];
    count++;
    for (int v = 0; v < n; v++) {
      bool link = directed ? (matrixG[u][v] || matrixG[v][u])
                           : matrixG[u][v];
      if (link && !seen[v]) {
        seen[v] = true;
        queue[back++] = v;
      }
    }
  }
  return count;
}

static void showConnectivity() {
  cout << "\n--- Connectivity ---\n";
  if (n == 0) {
    cout << (directed ? "  Weakly connected: Yes (No vertices)\n"
                      : "  Connected: Yes (No vertices)\n");
    return;
  }

  int reach = reachableCount();
  if (directed) {
    cout << "  Weakly connected: " << (reach == n ? "Yes" : "No")
         << " (V0's weak component holds " << reach << " of " << n
         << " vertices)\n";
  } else {
    cout << "  Connected: " << (reach == n ? "Yes" : "No") << " (" << reach
         << " of " << n << " reachable from V0)\n";
  }
}

static void showAll() {
  showMatrix();
  showAdjacencyLists();
  showDegrees();
  showCounts();
  showConnectivity();
}

int main() {
  resetGraph();

  while (true) {
    cout << "\n=== G02: Graph CRUD + Properties ===\n";
    cout << "Graph: " << (directed ? "Directed" : "Undirected")
         << " | V = " << n << ", E = " << edgeCount << "\n";
    cout << "1. Build / rebuild graph\n";
    cout << "2. Load undirected demo (6 vertices)\n";
    cout << "3. Load directed demo (4 vertices)\n";
    cout << "4. Add edge\n";
    cout << "5. Remove edge\n";
    cout << "6. Add vertex\n";
    cout << "7. Remove vertex\n";
    cout << "8. Show adjacency matrix\n";
    cout << "9. Show adjacency lists\n";
    cout << "10. Show degrees\n";
    cout << "11. Counts, density, regular, complete\n";
    cout << "12. Connected or not\n";
    cout << "13. Show all properties\n";
    cout << "14. Exit\n";

    int choice = readInt("Select: ");

    if (choice == 1) {
      buildGraph();
    } else if (choice == 2) {
      loadDemo(false);
    } else if (choice == 3) {
      loadDemo(true);
    } else if (choice == 4) {
      addEdgeInteractive();
    } else if (choice == 5) {
      removeEdgeInteractive();
    } else if (choice == 6) {
      addVertex();
    } else if (choice == 7) {
      removeVertexInteractive();
    } else if (choice == 8) {
      showMatrix();
    } else if (choice == 9) {
      showAdjacencyLists();
    } else if (choice == 10) {
      showDegrees();
    } else if (choice == 11) {
      showCounts();
    } else if (choice == 12) {
      showConnectivity();
    } else if (choice == 13) {
      showAll();
    } else if (choice == 14) {
      cout << "Exiting...\n";
      break;
    } else {
      cout << "Invalid option.\n";
    }
  }

  return 0;
}
