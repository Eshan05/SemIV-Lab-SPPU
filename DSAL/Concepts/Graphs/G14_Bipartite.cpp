// 11. Bipartite graphs: A 2-colouring is a BFS or DFS that alternates sides.
// Demo: Colour every vertex 0 or 1 so each edge crosses. The colour is the
// parity of the BFS layer, and one same-colour edge proves an odd cycle.

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

static int colour[MAXV];
static int layerOf[MAXV];
static vector<int> visitOrder;
static bool twoColourable = true;
static string trouble;

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
      cout << "  Rejected: A self-loop can never be 2-coloured.\n";
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

static void loadDemo(bool oddCycle) {
  resetGraph();

  if (oddCycle) {
    n = 7;
    cout << "Odd-cycle demo: 7 vertices.\n";
    cout << "  Ring 0 - 1 - 2 - 3 - 4 - 0 has 5 edges, then a tail 3 - 5 - 6.\n";
    cout << "  An odd ring cannot split into two sides, so no colouring\n";
    cout << "  works.\n\n";
    addEdge(0, 1);
    addEdge(1, 2);
    addEdge(2, 3);
    addEdge(3, 4);
    addEdge(4, 0);
    addEdge(3, 5);
    addEdge(5, 6);
  } else {
    n = 9;
    cout << "Bipartite demo: 9 vertices.\n";
    cout << "  Ring 0 - 1 - 2 - 3 - 4 - 5 - 6 - 7 - 0 has 8 edges, then 8 - 0.\n";
    cout << "  Every ring edge crosses sides, so two colours are enough.\n\n";
    addEdge(0, 1);
    addEdge(1, 2);
    addEdge(2, 3);
    addEdge(3, 4);
    addEdge(4, 5);
    addEdge(5, 6);
    addEdge(6, 7);
    addEdge(7, 0);
    addEdge(8, 0);
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

static void resetColours() {
  for (int i = 0; i < MAXV; i++) {
    colour[i] = -1;
    layerOf[i] = -1;
  }
  visitOrder.clear();
  twoColourable = true;
  trouble = "";
}

static string sideText(int c) {
  if (c == 0) return "0";
  if (c == 1) return "1";
  return "uncoloured";
}

static void reportColours() {
  cout << "\nColour  :";
  for (int i = 0; i < n; i++) cout << " " << sideText(colour[i]);
  cout << "\nOrder   :";
  for (int v : visitOrder) cout << " " << v;
  cout << "\n";

  if (!twoColourable) {
    cout << "\nNot bipartite: " << trouble << "\n";
    cout << "A same-colour edge closes an odd cycle, so the two sides cannot\n";
    cout << "be separated and a 2-colouring does not exist.\n";
    return;
  }

  cout << "\nSide 0:";
  int side0 = 0;
  for (int i = 0; i < n; i++) {
    if (colour[i] != 0) continue;
    cout << " " << i;
    side0++;
  }
  if (side0 == 0) cout << " (None)";
  cout << "\nSide 1:";
  int side1 = 0;
  for (int i = 0; i < n; i++) {
    if (colour[i] != 1) continue;
    cout << " " << i;
    side1++;
  }
  if (side1 == 0) cout << " (None)";
  cout << "\nEvery edge has one end on each side, so the graph is bipartite.\n";
  cout << "Sides hold " << side0 << " and " << side1 << " vertices.\n";
}

static void runBfsColour() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }

  resetColours();
  cout << "\n--- BFS 2-colouring ---\n";

  vector<int> q;
  int front = 0;

  for (int start = 0; start < n && twoColourable; start++) {
    if (colour[start] != -1) continue;
    colour[start] = 0;
    layerOf[start] = 0;
    visitOrder.push_back(start);
    q.push_back(start);
    cout << "New component: Give V" << start << " colour 0 at layer 0.\n";

    while (front < (int)q.size() && twoColourable) {
      int u = q[front];
      front++;
      cout << "  Pop V" << u << " (Colour " << colour[u] << ", layer "
           << layerOf[u] << ").\n";

      for (int v : adj[u]) {
        if (colour[v] == -1) {
          colour[v] = 1 - colour[u];
          layerOf[v] = layerOf[u] + 1;
          visitOrder.push_back(v);
          q.push_back(v);
          cout << "    Edge " << u << " - " << v << ": V" << v
               << " uncoloured, so give it colour " << colour[v]
               << " at layer " << layerOf[v] << ".\n";
        } else if (colour[v] == colour[u]) {
          twoColourable = false;
          trouble = "Edge " + to_string(u) + " - " + to_string(v) +
                    " joins V" + to_string(u) + " and V" + to_string(v) +
                    ", both colour " + sideText(colour[u]) + ".";
          cout << "    Edge " << u << " - " << v << ": Both are colour "
               << colour[u] << ", so this edge stays inside one side.\n";
          cout << "    V" << u << " sits at layer " << layerOf[u] << " and V" << v
               << " at layer " << layerOf[v]
               << ", so two routes of the same length reach them.\n";
          cout << "    An edge between two same-layer vertices closes an odd"
               << " ring.\n";
          break;
        } else {
          cout << "    Edge " << u << " - " << v << ": V" << v
               << " is colour " << colour[v] << ", different, so the edge"
               << " crosses.\n";
        }
      }
    }
  }

  reportColours();
  if (twoColourable) {
    cout << "\nColour equals the BFS layer parity: Even layers became side 0,\n";
    cout << "odd layers side 1.\n";
  }
}

static void dfsColour(int u, int depth) {
  string pad(depth * 2, ' ');
  cout << pad << "Enter V" << u << " (Colour " << colour[u] << ")\n";

  for (int v : adj[u]) {
    if (!twoColourable) return;
    if (colour[v] == -1) {
      colour[v] = 1 - colour[u];
      layerOf[v] = depth + 1;
      visitOrder.push_back(v);
      cout << pad << "  Edge " << u << " - " << v << ": V" << v
           << " uncoloured, give it colour " << colour[v] << ".\n";
      dfsColour(v, depth + 1);
    } else if (colour[v] == colour[u]) {
      twoColourable = false;
      trouble = "Edge " + to_string(u) + " - " + to_string(v) + " joins V" +
                to_string(u) + " and V" + to_string(v) + ", both colour " +
                sideText(colour[u]) + ".";
      cout << pad << "  Edge " << u << " - " << v << ": Both are colour "
           << colour[u] << ", a conflict.\n";
      return;
    } else {
      cout << pad << "  Edge " << u << " - " << v << ": V" << v
           << " is colour " << colour[v] << ", different, fine.\n";
    }
  }

  cout << pad << "Leave V" << u << "\n";
}

static void runDfsColour() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }

  resetColours();
  cout << "\n--- DFS 2-colouring ---\n";

  for (int start = 0; start < n && twoColourable; start++) {
    if (colour[start] != -1) continue;
    colour[start] = 0;
    layerOf[start] = 0;
    visitOrder.push_back(start);
    cout << "New component: Give V" << start << " colour 0.\n";
    dfsColour(start, 0);
  }

  reportColours();
  if (twoColourable) {
    cout << "\nDFS reaches the same two sides, just in a different order.\n";
  }
}

static void dfsQuiet(int u) {
  for (int v : adj[u]) {
    if (!twoColourable) return;
    if (colour[v] == -1) {
      colour[v] = 1 - colour[u];
      visitOrder.push_back(v);
      dfsQuiet(v);
    } else if (colour[v] == colour[u]) {
      twoColourable = false;
      return;
    }
  }
}

static void runCompare() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }

  resetColours();
  vector<int> q;
  int front = 0;
  for (int start = 0; start < n && twoColourable; start++) {
    if (colour[start] != -1) continue;
    colour[start] = 0;
    visitOrder.push_back(start);
    q.push_back(start);
    while (front < (int)q.size() && twoColourable) {
      int u = q[front];
      front++;
      for (int v : adj[u]) {
        if (colour[v] == -1) {
          colour[v] = 1 - colour[u];
          visitOrder.push_back(v);
          q.push_back(v);
        } else if (colour[v] == colour[u]) {
          twoColourable = false;
          break;
        }
      }
    }
  }
  int bfsColour[MAXV];
  vector<int> bfsOrder = visitOrder;
  bool bfsOk = twoColourable;
  for (int i = 0; i < n; i++) bfsColour[i] = colour[i];

  resetColours();
  for (int start = 0; start < n && twoColourable; start++) {
    if (colour[start] != -1) continue;
    colour[start] = 0;
    visitOrder.push_back(start);
    dfsQuiet(start);
  }

  cout << "\n--- BFS and DFS side by side ---\n";
  cout << "BFS order :";
  for (int v : bfsOrder) cout << " " << v;
  cout << "\nDFS order :";
  for (int v : visitOrder) cout << " " << v;
  cout << "\n";

  cout << "BFS colour:";
  for (int i = 0; i < n; i++) cout << " " << sideText(bfsColour[i]);
  cout << "\nDFS colour:";
  for (int i = 0; i < n; i++) cout << " " << sideText(colour[i]);
  cout << "\n";

  bool sameSides = true;
  for (int i = 0; i < n; i++) {
    if (bfsColour[i] != colour[i]) sameSides = false;
  }

  cout << "\nVerdict   : " << (bfsOk ? "Bipartite" : "Not bipartite") << "\n";
  if (bfsOk) cout << "Same sides: " << (sameSides ? "Yes" : "No") << "\n";
  else cout << "Same sides: (None, a 2-colouring does not exist)\n";
  if (!bfsOk) {
    cout << "\nBoth searches hit a wall of the same kind: An edge joining two\n";
    cout << "vertices the alternation already put on one side. They need not\n";
    cout << "stop on the same edge.\n";
  } else {
    cout << "\nThe sides agree because a 2-colouring of a component is unique\n";
    cout << "up to swapping 0 and 1, and both searches start at colour 0.\n";
    cout << "Only the visit order differs: BFS by layer, DFS by depth.\n";
  }
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
      cout << "  Rejected: A self-loop can never be 2-coloured.\n";
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
  cout << "  On its own it is its own component and is trivially coloured.\n";
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
  cout << "  No edges means every vertex is its own side, so it is bipartite.\n";
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
    cout << "\n=== G14: Bipartite Graphs ===\n";
    cout << "Graph: Undirected | V = " << n << ", E = " << edgeCount << "\n";
    cout << "1. Build an undirected graph\n";
    cout << "2. Load bipartite demo (9 vertices)\n";
    cout << "3. Load odd-cycle demo (7 vertices)\n";
    cout << "4. Graph operations\n";
    cout << "5. Colour with BFS\n";
    cout << "6. Colour with DFS\n";
    cout << "7. Run both and compare\n";
    cout << "8. Show adjacency list\n";
    cout << "9. Exit\n";

    int choice = readInt("Select: ");

    if (choice == 1) {
      buildGraph();
    } else if (choice == 2) {
      loadDemo(false);
    } else if (choice == 3) {
      loadDemo(true);
    } else if (choice == 4) {
      graphOperationsMenu();
    } else if (choice == 5) {
      runBfsColour();
    } else if (choice == 6) {
      runDfsColour();
    } else if (choice == 7) {
      runCompare();
    } else if (choice == 8) {
      showAdjacency();
    } else if (choice == 9) {
      cout << "Exiting...\n";
      break;
    } else {
      cout << "Invalid option.\n";
    }
  }

  return 0;
}
