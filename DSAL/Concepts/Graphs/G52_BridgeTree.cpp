// 15c. Two-edge-connected components and the bridge tree: Find every bridge
// with one lowlink pass, then remove the bridges. Each piece that is left is a
// two-edge-connected component, because two routes join every pair inside it.
// Contracting those pieces gives a forest whose edges are exactly the bridges.
// Demo: Two triangles joined by a single edge, which that edge alone separates.

#include <algorithm>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

const int MAXV = 15;
const int TRACE_LIMIT = 70;

static int n = 0;
static int edgeCount = 0;
static int matrixG[MAXV][MAXV];
static vector<int> adj[MAXV];

static int disc[MAXV];
static int low[MAXV];
static bool seen[MAXV];
static bool isBridge[MAXV][MAXV];
static int compOf[MAXV];
static int compCount = 0;
static bool resultDone = false;
static int traceShown = 0;

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

static string plural(int count, const string &one, const string &many) {
  return to_string(count) + " " + (count == 1 ? one : many);
}

static string edgeText(int u, int v) {
  return "(" + to_string(u) + " - " + to_string(v) + ")";
}

static void say(const string &line) {
  if (traceShown >= TRACE_LIMIT) return;
  traceShown++;
  cout << line << "\n";
}

static void resetGraph() {
  for (int i = 0; i < MAXV; i++) {
    adj[i].clear();
    for (int j = 0; j < MAXV; j++) matrixG[i][j] = 0;
  }
  n = 0;
  edgeCount = 0;
  resultDone = false;
}

static void clearResult() {
  for (int i = 0; i < MAXV; i++) {
    disc[i] = -1;
    low[i] = 0;
    seen[i] = false;
    compOf[i] = -1;
    for (int j = 0; j < MAXV; j++) isBridge[i][j] = false;
  }
  compCount = 0;
  resultDone = false;
}

static void addEdge(int u, int v) {
  matrixG[u][v] = 1;
  matrixG[v][u] = 1;
  adj[u].push_back(v);
  adj[v].push_back(u);
  edgeCount++;
}

static int bridgeCount() {
  int total = 0;
  for (int u = 0; u < n; u++) {
    for (int v = u + 1; v < n; v++) {
      if (isBridge[u][v]) total++;
    }
  }
  return total;
}

static void dfsLow(int u, int parent, int &timerStep) {
  disc[u] = timerStep;
  low[u] = timerStep;
  timerStep++;
  say("Enter V" + to_string(u) + ": Discovery " + to_string(disc[u]) +
      ", low starts at " + to_string(low[u]) + ".");

  for (int v : adj[u]) {
    if (v == parent) continue;
    if (disc[v] == -1) {
      dfsLow(v, u, timerStep);
      low[u] = min(low[u], low[v]);
      if (low[v] > disc[u]) {
        isBridge[u][v] = true;
        isBridge[v][u] = true;
        say("  Back at V" + to_string(u) + ": Low[V" + to_string(v) + "] = " +
            to_string(low[v]) + " is above discovery " + to_string(disc[u]) +
            ", so " + edgeText(u, v) + " is a bridge.");
      } else {
        say("  Back at V" + to_string(u) + ": Low[V" + to_string(v) + "] = " +
            to_string(low[v]) + " reaches V" + to_string(u) +
            ", so " + edgeText(u, v) + " sits on a cycle.");
      }
    } else if (disc[v] < disc[u]) {
      low[u] = min(low[u], disc[v]);
      say("  V" + to_string(v) + " was discovered above V" + to_string(u) +
          ", so low[V" + to_string(u) + "] can drop to " + to_string(disc[v]) +
          ".");
    } else {
      say("  V" + to_string(v) + " was discovered below V" + to_string(u) +
          ", so this edge adds nothing to low.");
    }
  }

  say("Leave V" + to_string(u) + ": Low = " + to_string(low[u]) + ".");
}

static void labelComponents() {
  for (int s = 0; s < n; s++) {
    if (compOf[s] != -1) continue;

    compOf[s] = compCount;
    int queue[MAXV];
    int head = 0;
    int tail = 0;
    queue[tail++] = s;
    vector<int> members(1, s);

    while (head < tail) {
      int u = queue[head++];
      for (int v : adj[u]) {
        if (isBridge[u][v]) continue;
        if (compOf[v] != -1) continue;
        compOf[v] = compCount;
        queue[tail++] = v;
        members.push_back(v);
      }
    }

    sort(members.begin(), members.end());
    string listed;
    for (int i = 0; i < (int)members.size(); i++) {
      if (i > 0) listed += ", ";
      listed += "V" + to_string(members[i]);
    }
    say("Component C" + to_string(compCount) + " holds " + listed + ".");
    compCount++;
  }
}

static void runBridgeTree() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }

  clearResult();
  traceShown = 0;

  cout << "\n--- Bridges, then the pieces they leave behind ---\n";
  cout << "A lowlink pass marks an edge as a bridge when the subtree below it\n";
  cout << "cannot climb back to the endpoint above. Removing the bridges splits\n";
  cout << "the graph into two-edge-connected pieces, and each remaining edge\n";
  cout << "keeps its piece whole.\n\n";

  cout << "--- Lowlink pass ---\n";
  int timerStep = 0;
  for (int s = 0; s < n; s++) {
    if (disc[s] != -1) continue;
    if (adj[s].empty()) {
      say("V" + to_string(s) + " has no edges, so it starts a piece of its own.");
      disc[s] = timerStep;
      low[s] = timerStep;
      timerStep++;
      continue;
    }
    dfsLow(s, -1, timerStep);
  }

  cout << "\n--- Two-edge-connected components ---\n";
  labelComponents();

  cout << "\n--- Bridge tree ---\n";
  int treeEdges = 0;
  for (int u = 0; u < n; u++) {
    for (int v = u + 1; v < n; v++) {
      if (!isBridge[u][v]) continue;
      treeEdges++;
      cout << "  C" << compOf[u] << " - C" << compOf[v] << " by "
           << edgeText(u, v) << "\n";
    }
  }
  if (treeEdges == 0) {
    cout << "  (No bridges, so the tree has one node and no edges.)\n";
  }

  int pieces = 0;
  for (int s = 0; s < n; s++) {
    if (adj[s].empty()) pieces++;
  }

  bool connected = true;
  bool reach[MAXV] = {false};
  int queue[MAXV];
  int head = 0;
  int tail = 0;
  reach[0] = true;
  queue[tail++] = 0;
  while (head < tail) {
    int u = queue[head++];
    for (int v : adj[u]) {
      if (reach[v]) continue;
      reach[v] = true;
      queue[tail++] = v;
    }
  }
  for (int s = 0; s < n; s++) {
    if (!reach[s]) connected = false;
  }

  cout << "\n--- Result ---\n";
  cout << "Bridges: " << bridgeCount() << ". Components: " << compCount
       << ". Tree edges: " << treeEdges << ".\n";
  if (connected) {
    cout << "The graph is connected, so the contracted graph is a tree with "
         << plural(compCount, "node", "nodes") << " and "
         << plural(treeEdges, "edge", "edges") << ".\n";
  } else {
    cout << "The graph is not connected, so the contracted graph is a forest\n";
    cout << "of " << plural(compCount, "node", "nodes") << " and "
         << plural(treeEdges, "edge", "edges") << ".\n";
  }
  if (bridgeCount() == 0) {
    cout << "No edge is a bridge, so nothing is split from anything and the\n";
    cout << "tree has no edges at all.\n";
  }
  if (pieces == 1) {
    cout << "1 vertex has no edges, so it is a piece on its own.\n";
  } else if (pieces > 1) {
    cout << pieces << " vertices have no edges, so each one is a piece on\n";
    cout << "its own, with no tree edge to any other piece.\n";
  }
  cout << "Every bridge carries all traffic between its two pieces, and two\n";
  cout << "edges between the same pieces would survive the removal, so the\n";
  cout << "contracted graph cannot hold a cycle.\n";
  resultDone = true;
}

static void showMatrix() {
  cout << "\n--- Adjacency matrix ---\n";
  if (n == 0) {
    cout << "(Empty)\n";
    return;
  }
  cout << "      ";
  for (int j = 0; j < n; j++) cout << setw(4) << j;
  cout << "\n";
  for (int i = 0; i < n; i++) {
    cout << setw(4) << i << "  ";
    for (int j = 0; j < n; j++) cout << setw(4) << matrixG[i][j];
    cout << "\n";
  }
  cout << "The table is symmetric, because an undirected edge belongs to both\n";
  cout << "ends. A diagonal entry stays zero: A loop is not an edge here.\n";
}

static void showEdges() {
  cout << "\n--- Edge list ---\n";
  if (n == 0 || edgeCount == 0) {
    cout << "(Empty)\n";
  } else {
    int shown = 0;
    for (int u = 0; u < n; u++) {
      for (int v = u + 1; v < n; v++) {
        if (!matrixG[u][v]) continue;
        shown++;
        cout << "  E" << (shown - 1) << ": " << edgeText(u, v);
        if (resultDone && isBridge[u][v]) cout << " is a bridge";
        cout << "\n";
      }
    }
  }
  if (resultDone) {
    cout << "A bridge tag appears only after a run, because only the lowlink\n";
    cout << "pass can tell which edges are the only way across.\n";
  }
}

static void buildGraph() {
  int fresh = readRange("Number of vertices (0.." + to_string(MAXV) + "): ", 0,
                        MAXV);
  resetGraph();
  n = fresh;

  if (n == 0) {
    cout << "Build complete: 0 vertices, 0 edges.\n";
    return;
  }

  int limit = n * (n - 1) / 2;
  cout << "Undirected edges only, vertices 0.." << (n - 1) << ".\n";

  while (true) {
    cout << "\nEdge " << (edgeCount + 1) << " (Limit " << limit
         << ", -1 to stop)\n";
    int u = readInt("  From vertex (0.." + to_string(n - 1) +
                    ", -1 to stop): ");
    if (u == -1) break;
    int v = readInt("  To vertex (0.." + to_string(n - 1) +
                    ", -1 to stop): ");
    if (v == -1) break;
    if (u < 0 || u >= n || v < 0 || v >= n) {
      cout << "  Rejected: Vertices must be in 0.." << (n - 1) << ".\n";
      continue;
    }
    if (u == v) {
      cout << "  Rejected: A loop gives no second route, so it is not an edge"
              " here.\n";
      continue;
    }
    if (matrixG[u][v]) {
      cout << "  Rejected: " << edgeText(u, v) << " already exists.\n";
      continue;
    }
    addEdge(u, v);
    cout << "  Added E" << (edgeCount - 1) << " " << edgeText(u, v) << "\n";
    cout << "  Edges = " << edgeCount << " now.\n";
  }

  cout << "\nBuild complete: " << n
       << (n == 1 ? " vertex, " : " vertices, ") << edgeCount
       << (edgeCount == 1 ? " edge.\n" : " edges.\n");
}

static void loadJoinedTriangles() {
  resetGraph();
  n = 6;
  cout << "Joined triangles demo: 6 vertices, 7 edges.\n";
  cout << "  0 - 1, 1 - 2, 2 - 0, 2 - 3, 3 - 4, 4 - 5, 5 - 3.\n";
  cout << "  Each triangle is two-edge-connected on its own, and the single\n";
  cout << "  edge 2 - 3 is the only way from one to the other.\n\n";

  addEdge(0, 1);
  addEdge(1, 2);
  addEdge(2, 0);
  addEdge(2, 3);
  addEdge(3, 4);
  addEdge(4, 5);
  addEdge(5, 3);

  cout << "Demo loaded: " << n << " vertices, " << edgeCount << " edges.\n";
}

static void loadChainDemo() {
  resetGraph();
  n = 8;
  cout << "Chain demo: 8 vertices, 9 edges.\n";
  cout << "  0 - 1, 1 - 2, 2 - 3, 3 - 0 hold a square.\n";
  cout << "  3 - 4 joins it to 4 - 5, 5 - 6, 6 - 4, a triangle.\n";
  cout << "  4 - 7 adds a leaf, so the tree becomes a path of three pieces.\n\n";

  addEdge(0, 1);
  addEdge(1, 2);
  addEdge(2, 3);
  addEdge(3, 0);
  addEdge(3, 4);
  addEdge(4, 5);
  addEdge(5, 6);
  addEdge(6, 4);
  addEdge(4, 7);

  cout << "Demo loaded: " << n << " vertices, " << edgeCount << " edges.\n";
}

static bool noGraph() { return n == 0; }

static void addEdgeInteractive() {
  if (noGraph()) {
    cout << "Build the graph first.\n";
    return;
  }
  if (edgeCount == n * (n - 1) / 2) {
    cout << "The graph is complete, so every pair is already joined.\n";
    return;
  }

  cout << "\nAdd an edge (V = " << n << ", so vertices 0.." << (n - 1) << ")\n";
  while (true) {
    int u = readInt("  From vertex (0.." + to_string(n - 1) +
                    ", -1 to cancel): ");
    if (u == -1) {
      cout << "  Cancelled.\n";
      return;
    }
    int v = readInt("  To vertex (0.." + to_string(n - 1) +
                    ", -1 to cancel): ");
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
      cout << "  Rejected: A loop gives no second route, so it is not an edge"
              " here.\n";
      cout << "  Please re-enter this edge.\n";
      continue;
    }
    if (matrixG[u][v]) {
      cout << "  Rejected: " << edgeText(u, v) << " already exists.\n";
      cout << "  Please re-enter this edge.\n";
      continue;
    }

    addEdge(u, v);
    clearResult();
    cout << "  Added E" << (edgeCount - 1) << " " << edgeText(u, v) << "\n";
    cout << "  Edges = " << edgeCount << " now.\n";
    return;
  }
}

static void removeEdgeInteractive() {
  if (noGraph()) {
    cout << "Build the graph first.\n";
    return;
  }

  cout << "\nRemove an edge (V = " << n << ", so vertices 0.." << (n - 1)
       << ")\n";
  while (true) {
    int u = readInt("  From vertex (0.." + to_string(n - 1) +
                    ", -1 to cancel): ");
    if (u == -1) {
      cout << "  Cancelled.\n";
      return;
    }
    int v = readInt("  To vertex (0.." + to_string(n - 1) +
                    ", -1 to cancel): ");
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
      cout << "  Rejected: " << edgeText(u, v) << " is not in the graph.\n";
      cout << "  Please re-enter this edge.\n";
      continue;
    }

    matrixG[u][v] = 0;
    matrixG[v][u] = 0;
    for (int i = 0; i < (int)adj[u].size(); i++) {
      if (adj[u][i] != v) continue;
      adj[u].erase(adj[u].begin() + i);
      break;
    }
    for (int i = 0; i < (int)adj[v].size(); i++) {
      if (adj[v][i] != u) continue;
      adj[v].erase(adj[v].begin() + i);
      break;
    }
    edgeCount--;
    clearResult();

    cout << "  Removed " << edgeText(u, v) << "\n";
    cout << "  Edges = " << edgeCount << " now.\n";
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
  clearResult();

  cout << "\nAdded V" << fresh << " with no edges.\n";
  cout << "  V = " << n << ", edges = " << edgeCount << ".\n";
  cout << "  A vertex with no edges is a piece on its own, and the lowlink\n";
  cout << "  pass starts and finishes there at once.\n";
}

static void removeVertexInteractive() {
  if (noGraph()) {
    cout << "Build the graph first.\n";
    return;
  }

  int v;
  while (true) {
    v = readInt("  Vertex to remove (0.." + to_string(n - 1) +
                ", -1 to cancel): ");
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

  edgeCount = 0;
  for (int a = 0; a < n; a++) {
    for (int b = 0; b < n; b++) {
      if (matrixG[a][b]) edgeCount++;
    }
  }
  edgeCount /= 2;
  clearResult();

  cout << "\nRemoved V" << v << ".\n";
  if (v < n) cout << "  Vertices above V" << v << " shift down by one.\n";
  cout << "  Edges lost: " << before - edgeCount;
  cout << " (Recounted from the matrix.)\n";
  cout << "  V = " << n << ", edges = " << edgeCount << ".\n";
}

static void clearEdges() {
  if (noGraph()) {
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
  clearResult();

  cout << "\nRemoved all " << before
       << (before == 1 ? " edge.\n" : " edges.\n");
  cout << "  V = " << n << ", edges = 0. The vertices are still there.\n";
  cout << "  With no edges, every vertex is a component and the tree is empty.\n";
}

static void operationsMenu() {
  while (true) {
    cout << "\n--- Graph operations ---\n";
    cout << "Graph: Undirected | V = " << n << ", E = " << edgeCount << "\n";
    cout << "1. Add an edge\n";
    cout << "2. Remove an edge\n";
    cout << "3. Add a vertex\n";
    cout << "4. Remove a vertex\n";
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
      clearEdges();
    } else if (choice == 0) {
      return;
    } else {
      cout << "Invalid option.\n";
    }
  }
}

int main() {
  resetGraph();
  clearResult();

  while (true) {
    cout << "\n=== G52: Bridge Tree and Two-Edge-Connected Components ===\n";
    cout << "Graph: Undirected | V = " << n << ", E = " << edgeCount << "\n";
    cout << "1. Build an undirected graph\n";
    cout << "2. Load the joined triangles demo (6 vertices)\n";
    cout << "3. Load the chain demo (8 vertices)\n";
    cout << "4. Graph operations\n";
    cout << "5. Run bridges and the bridge tree\n";
    cout << "6. Show the adjacency matrix\n";
    cout << "7. Show the edge list\n";
    cout << "8. Exit\n";

    int choice = readInt("Select: ");

    if (choice == 1) {
      buildGraph();
    } else if (choice == 2) {
      loadJoinedTriangles();
    } else if (choice == 3) {
      loadChainDemo();
    } else if (choice == 4) {
      operationsMenu();
    } else if (choice == 5) {
      runBridgeTree();
    } else if (choice == 6) {
      showMatrix();
    } else if (choice == 7) {
      showEdges();
    } else if (choice == 8) {
      cout << "Exiting...\n";
      break;
    } else {
      cout << "Invalid option.\n";
    }
  }

  return 0;
}
