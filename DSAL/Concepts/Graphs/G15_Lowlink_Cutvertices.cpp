// 12. Lowlink, bridges and articulation points: One DFS answers all three.
// Demo: Low[v] says how far back above v the subtree can reach. If it cannot
// reach above the parent, the arc up is a bridge; if it cannot reach the
// parent, the parent is a cut vertex.

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

const int MAXV = 15;

static int n = 0;
static int edgeCount = 0;
static int matrixG[MAXV][MAXV];
static vector<int> adj[MAXV];

static int disc[MAXV];
static int low[MAXV];
static int parentOf[MAXV];
static bool seen[MAXV];
static bool isCut[MAXV];
static vector<int> bridgeU;
static vector<int> bridgeV;
static int timerStep = 0;

static int readInt(const string &prompt) {
  int x;
  while (true) {
    cout << prompt;
    if (cin >> x) {
      cin.ignore(1024, '\n');
      return x;
    }
    if (cin.eof()) {
      cout << "\nInput closed.\n";
      exit(0);
    }
    cout << "Invalid number. Try again.\n";
    cin.clear();
    cin.ignore(1024, '\n');
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
    for (int j = 0; j < MAXV; j++) matrixG[i][j] = 0;
  }
  n = 0;
  edgeCount = 0;
}

static void addEdge(int u, int v) {
  matrixG[u][v] = 1;
  matrixG[v][u] = 1;
  adj[u].push_back(v);
  adj[v].push_back(u);
  edgeCount++;
}

static void buildGraph() {
  int newN = readRange("Number of vertices (0.." + to_string(MAXV) + "): ",
                       0, MAXV);

  resetGraph();
  n = newN;

  if (n == 0) {
    cout << "Build complete: 0 vertices, 0 edges.\n";
    return;
  }
  cout << "Undirected edges only. Enter two vertex numbers in 0.." << (n - 1)
       << ".\n";
  int e = 0;
  while (true) {
    e++;
    cout << "\nEdge " << e << "\n";
    int u = readInt("  From vertex (-1 to stop): ");
    if (u == -1) break;
    int v = readInt("  To vertex (-1 to stop): ");
    if (v == -1) break;
    if (u < 0 || u >= n || v < 0 || v >= n) {
      cout << "  Rejected: Vertices must be in 0.." << (n - 1) << ".\n";
      e--;
      continue;
    }
    if (u == v) {
      cout << "  Rejected: A self-loop would be its own second route.\n";
      e--;
      continue;
    }
    if (matrixG[u][v]) {
      cout << "  Rejected: That edge already exists.\n";
      e--;
      continue;
    }
    addEdge(u, v);
  }

  cout << "\nBuild complete: " << n
       << (n == 1 ? " vertex, " : " vertices, ") << edgeCount
       << (edgeCount == 1 ? " edge.\n" : " edges.\n");
}

static void loadDemo(int which) {
  resetGraph();

  if (which == 1) {
    n = 6;
    cout << "Barbell demo: 6 vertices.\n";
    cout << "  Triangle 0 - 1 - 2 - 0, bridge 2 - 3, triangle 3 - 4 - 5 - 3.\n";
    cout << "  The single edge 2 - 3 carries all traffic between the halves.\n\n";
    addEdge(0, 1);
    addEdge(1, 2);
    addEdge(2, 0);
    addEdge(2, 3);
    addEdge(3, 4);
    addEdge(4, 5);
    addEdge(5, 3);
  } else if (which == 2) {
    n = 5;
    cout << "Ring demo: 5 vertices.\n";
    cout << "  One cycle 0 - 1 - 2 - 3 - 4 - 0, so every edge has a second\n";
    cout << "  route around the ring. Expect no bridges and no cut vertex.\n\n";
    addEdge(0, 1);
    addEdge(1, 2);
    addEdge(2, 3);
    addEdge(3, 4);
    addEdge(4, 0);
  } else {
    n = 5;
    cout << "Two-triangles demo: 5 vertices.\n";
    cout << "  Triangles 0 - 1 - 2 - 0 and 0 - 3 - 4 - 0 share only V0.\n";
    cout << "  Removing V0 splits the graph, yet no single edge is a bridge.\n\n";
    addEdge(0, 1);
    addEdge(1, 2);
    addEdge(2, 0);
    addEdge(0, 3);
    addEdge(3, 4);
    addEdge(4, 0);
  }

  cout << "Demo loaded: " << n << " vertices.\n";
}

static void showAdjacency() {
  cout << "\n--- Adjacency List ---\n";
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
  cout << "Each line lists neighbours in insertion order.\n";
}

static void resetSearch() {
  for (int i = 0; i < MAXV; i++) {
    disc[i] = -1;
    low[i] = -1;
    parentOf[i] = -1;
    seen[i] = false;
    isCut[i] = false;
  }
  bridgeU.clear();
  bridgeV.clear();
  timerStep = 0;
}

static void noteBridge(int u, int v) {
  bridgeU.push_back(u);
  bridgeV.push_back(v);
}

static void dfs(int u, int depth, bool verbose) {
  string pad(depth * 2, ' ');
  seen[u] = true;
  disc[u] = timerStep;
  low[u] = disc[u];
  timerStep++;
  if (verbose) {
    cout << pad << "Enter V" << u << " with disc " << disc[u] << ".\n";
  }

  int children = 0;
  for (int v : adj[u]) {
    if (v == parentOf[u]) {
      if (verbose) {
        cout << pad << "  Edge " << u << " - " << v << ": The edge back to the"
             << " parent, skip it.\n";
      }
      continue;
    }

    if (!seen[v]) {
      parentOf[v] = u;
      children++;
      if (verbose) {
        cout << pad << "  Edge " << u << " - " << v << ": Tree edge, descend.\n";
      }
      dfs(v, depth + 1, verbose);

      if (low[v] < low[u]) low[u] = low[v];
      if (verbose) {
        cout << pad << "    Return from V" << v << ", so low[" << u << "] = min("
             << disc[u] << ", " << low[v] << ") = " << low[u] << ".\n";
      }

      if (low[v] > disc[u]) {
        noteBridge(u, v);
        if (verbose) {
          cout << pad << "    Bridge check " << u << " - " << v << ": Low[" << v
               << "] = " << low[v] << " > disc[" << u << "] = " << disc[u]
               << ", so the only way up is this edge.\n";
        }
      } else if (verbose) {
        cout << pad << "    Bridge check " << u << " - " << v << ": Low[" << v
             << "] = " << low[v] << " <= disc[" << u << "] = " << disc[u]
             << ", so a back edge still reaches the top.\n";
      }

      if (parentOf[u] != -1) {
        if (low[v] >= disc[u]) {
          isCut[u] = true;
          if (verbose) {
            cout << pad << "    Cut check V" << u << ": Low[" << v
                 << "] = " << low[v] << " >= disc[" << u << "] = " << disc[u]
                 << ", so V" << v << "'s side has no route around V" << u
                 << ".\n";
          }
        } else if (verbose) {
          cout << pad << "    Cut check V" << u << ": Low[" << v
               << "] = " << low[v] << " < disc[" << u << "] = " << disc[u]
               << ", so V" << v << "'s side climbs past V" << u << ".\n";
        }
      }
    } else {
      if (disc[v] < low[u]) low[u] = disc[v];
      if (verbose) {
        cout << pad << "  Edge " << u << " - " << v
             << ": Back edge to a visited vertex, so low[" << u << "] drops to "
             << low[u] << ".\n";
      }
    }
  }

  if (parentOf[u] == -1 && children > 1) {
    isCut[u] = true;
    if (verbose) {
      cout << pad << "Root V" << u << " has " << children
           << " children, so removing it splits them apart.\n";
    }
  }

  if (verbose) {
    cout << pad << "Leave V" << u << " with disc " << disc[u] << " and low "
         << low[u] << ".\n";
  }
}

static void runSearch(bool verbose) {
  resetSearch();

  if (verbose) cout << "\n--- Lowlink search ---\n";
  for (int start = 0; start < n; start++) {
    if (seen[start]) continue;
    if (verbose) cout << "\nNew component at V" << start << ".\n";
    dfs(start, 0, verbose);
  }
}

static void reportBridges() {
  cout << "\nBridges:";
  if (bridgeU.empty()) {
    cout << " (None)\n";
  } else {
    for (int i = 0; i < (int)bridgeU.size(); i++) {
      cout << " " << bridgeU[i] << " - " << bridgeV[i];
    }
    cout << "\n";
  }
}

static void reportCuts() {
  cout << "Cut vertices:";
  bool any = false;
  for (int i = 0; i < n; i++) {
    if (!isCut[i]) continue;
    cout << " " << i;
    any = true;
  }
  if (!any) cout << " (None)";
  cout << "\n";
}

static void runTrace() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }

  runSearch(true);

  cout << "\nDisc:";
  for (int i = 0; i < n; i++) cout << " " << disc[i];
  cout << "\nLow :";
  for (int i = 0; i < n; i++) cout << " " << low[i];
  cout << "\n";
  reportBridges();
  reportCuts();
}

static void runBridgesOnly() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }
  runSearch(false);
  cout << "\n--- Bridges ---\n";
  reportBridges();
  if (bridgeU.empty()) {
    cout << "Every edge lies on a cycle, so removing one never disconnects.\n";
  } else {
    cout << "Each listed edge is the only route between the two sides it joins.\n";
  }
}

static void runCutsOnly() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }
  runSearch(false);
  cout << "\n--- Cut vertices ---\n";
  reportCuts();
  cout << "A cut vertex raises the number of components when it is removed,\n";
  cout << "even when every one of its incident edges lies on a cycle.\n";
}

static string edgeLabel(int u, int v) {
  return "(" + to_string(u) + " - " + to_string(v) + ")";
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
    if (u < 0 || u >= n || v < 0 || v >= n) {
      cout << "  Rejected: Vertices must be in 0.." << (n - 1) << ".\n";
      cout << "  Please re-enter this edge.\n";
      continue;
    }
    if (u == v) {
      cout << "  Rejected: A self-loop would be its own second route.\n";
      cout << "  Please re-enter this edge.\n";
      continue;
    }
    if (matrixG[u][v]) {
      cout << "  Rejected: " << edgeLabel(u, v) << " already exists.\n";
      cout << "    Undirected edges are unordered: " << u << " - " << v
           << " is the same edge as " << v << " - " << u << ".\n";
      cout << "  Please re-enter this edge.\n";
      continue;
    }

    addEdge(u, v);
    cout << "  Added E" << (edgeCount - 1) << " " << edgeLabel(u, v) << "\n";
    cout << "    matrix[" << u << "][" << v << "] = 1, matrix[" << v << "]["
         << u << "] = 1\n";
    cout << "  E = " << edgeCount << (edgeCount == 1 ? " edge" : " edges")
         << " now.\n";
    return;
  }
}

static void removeNeighbour(int u, int v) {
  for (int i = 0; i < (int)adj[u].size(); i++) {
    if (adj[u][i] != v) continue;
    adj[u].erase(adj[u].begin() + i);
    return;
  }
}

static void recountEdges() {
  edgeCount = 0;
  for (int u = 0; u < n; u++) {
    for (int v = 0; v < n; v++) {
      if (matrixG[u][v] && u <= v) edgeCount++;
    }
  }
}

static void removeEdgeInteractive() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }

  cout << "\nRemove an edge (V = " << n << ", so vertices 0.." << (n - 1)
       << ")\n";
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
      cout << "  Rejected: Vertices must be in 0.." << (n - 1) << ".\n";
      cout << "  Please re-enter this edge.\n";
      continue;
    }
    if (!matrixG[u][v]) {
      cout << "  Rejected: " << edgeLabel(u, v) << " is not in the graph.\n";
      cout << "    Undirected edges are unordered: " << u << " - " << v
           << " is the same edge as " << v << " - " << u << ".\n";
      cout << "  Please re-enter this edge.\n";
      continue;
    }

    matrixG[u][v] = 0;
    matrixG[v][u] = 0;
    removeNeighbour(u, v);
    if (u != v) removeNeighbour(v, u);
    edgeCount--;

    cout << "  Removed " << edgeLabel(u, v) << "\n";
    cout << "    matrix[" << u << "][" << v << "] = 0, matrix[" << v << "]["
         << u << "] = 0\n";
    cout << "  E = " << edgeCount << (edgeCount == 1 ? " edge" : " edges")
         << " now.\n";
    return;
  }
}

static void addVertexInteractive() {
  if (n >= MAXV) {
    cout << "Vertex limit reached: MAXV = " << MAXV << ".\n";
    return;
  }

  int fresh = n;
  for (int i = 0; i <= fresh; i++) {
    matrixG[fresh][i] = 0;
    matrixG[i][fresh] = 0;
  }
  adj[fresh].clear();
  n++;

  cout << "\nAdded V" << fresh << " with no edges.\n";
  cout << "  V = " << n << ", E = " << edgeCount << ".\n";
  cout << "  It is a component of its own, and removing it changes nothing.\n";
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
    cout << "  Rejected: Vertex must be in 0.." << (n - 1) << ".\n";
  }

  int kept[MAXV][MAXV];
  for (int a = 0; a < n; a++) {
    for (int b = 0; b < n; b++) kept[a][b] = 0;
  }
  for (int a = 0; a < n; a++) {
    if (a == v) continue;
    for (int b = 0; b < n; b++) {
      if (b == v) continue;
      int na = (a < v) ? a : a - 1;
      int nb = (b < v) ? b : b - 1;
      kept[na][nb] = matrixG[a][b];
    }
  }

  vector<int> keptAdj[MAXV];
  for (int a = 0; a < n; a++) {
    if (a == v) continue;
    int na = (a < v) ? a : a - 1;
    for (int w : adj[a]) {
      if (w == v) continue;
      keptAdj[na].push_back((w < v) ? w : w - 1);
    }
  }

  int before = edgeCount;
  n--;
  for (int a = 0; a < MAXV; a++) {
    adj[a].clear();
    for (int b = 0; b < MAXV; b++) matrixG[a][b] = 0;
  }
  for (int a = 0; a < n; a++) {
    adj[a] = keptAdj[a];
    for (int b = 0; b < n; b++) matrixG[a][b] = kept[a][b];
  }
  recountEdges();

  cout << "\nRemoved V" << v << ".\n";
  if (v < n) cout << "  Vertices above V" << v << " shift down by one.\n";
  cout << "  Edges lost: " << before - edgeCount;
  cout << " (Recounted from the matrix.)\n";
  cout << "  V = " << n << ", E = " << edgeCount << ".\n";
}

static void clearAllEdges() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }
  if (edgeCount == 0) {
    cout << "\nThe graph already has no edges.\n";
    return;
  }

  int before = edgeCount;
  for (int u = 0; u < n; u++) {
    adj[u].clear();
    for (int w = 0; w < n; w++) matrixG[u][w] = 0;
  }
  edgeCount = 0;

  cout << "\nRemoved all " << before
       << (before == 1 ? " edge.\n" : " edges.\n");
  cout << "  V = " << n << ", E = 0. The vertices are still there.\n";
  cout << "  With no edges at all, only a lone vertex is irreplaceable.\n";
}

static void graphOperationsMenu() {
  while (true) {
    cout << "\n--- Graph operations ---\n";
    cout << "Graph: Undirected | V = " << n << ", E = " << edgeCount << "\n";
    cout << "1. Add an edge to the current graph\n";
    cout << "2. Remove an edge from the current graph\n";
    cout << "3. Add a vertex to the current graph\n";
    cout << "4. Remove a vertex from the current graph\n";
    cout << "5. Clear all edges\n";
    cout << "0. Back to the main menu\n";

    int choice = readInt("Select: ");

    if (choice == 1) {
      addEdgeInteractive();
    } else if (choice == 2) {
      removeEdgeInteractive();
    } else if (choice == 3) {
      addVertexInteractive();
    } else if (choice == 4) {
      removeVertexInteractive();
    } else if (choice == 5) {
      clearAllEdges();
    } else if (choice == 0) {
      return;
    } else {
      cout << "Invalid option.\n";
    }
  }
}

int main() {
  resetGraph();

  while (true) {
    cout << "\n=== G15: Lowlink, Bridges, Cut Vertices ===\n";
    cout << "Graph: Undirected | V = " << n << ", E = " << edgeCount << "\n";
    cout << "1. Build an undirected graph\n";
    cout << "2. Load barbell demo (6 vertices)\n";
    cout << "3. Load ring demo (5 vertices)\n";
    cout << "4. Load two-triangles demo (5 vertices)\n";
    cout << "5. Graph operations\n";
    cout << "6. Trace the search, then report both\n";
    cout << "7. Bridges only\n";
    cout << "8. Cut vertices only\n";
    cout << "9. Show adjacency list\n";
    cout << "10. Exit\n";

    int choice = readInt("Select: ");

    if (choice == 1) {
      buildGraph();
    } else if (choice == 2) {
      loadDemo(1);
    } else if (choice == 3) {
      loadDemo(2);
    } else if (choice == 4) {
      loadDemo(3);
    } else if (choice == 5) {
      graphOperationsMenu();
    } else if (choice == 6) {
      runTrace();
    } else if (choice == 7) {
      runBridgesOnly();
    } else if (choice == 8) {
      runCutsOnly();
    } else if (choice == 9) {
      showAdjacency();
    } else if (choice == 10) {
      cout << "Exiting...\n";
      break;
    } else {
      cout << "Invalid option.\n";
    }
  }

  return 0;
}
