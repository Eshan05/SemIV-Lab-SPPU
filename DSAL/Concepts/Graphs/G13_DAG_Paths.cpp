// 10. Shortest and longest paths in a DAG: One topological order, two DPs.
// Demo: After a topological sort every arc relaxes exactly once. Shortest uses
// min, longest uses max, and the same arcs in the wrong order give wrong paths.

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

const int MAXV = 15;
const int INF = 1000000;

static int n = 0;
static int edgeCount = 0;
static int weightG[MAXV][MAXV];
static vector<int> adj[MAXV];

static int indeg[MAXV];
static vector<int> topo;
static int dist[MAXV];
static int prevOf[MAXV];

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
    for (int j = 0; j < MAXV; j++) weightG[i][j] = 0;
  }
  n = 0;
  edgeCount = 0;
  topo.clear();
}

static void addArc(int u, int v, int w) {
  weightG[u][v] = w;
  adj[u].push_back(v);
  edgeCount++;
}

static void buildGraph() {
  int newN = readRange("Number of vertices (0.." + to_string(MAXV) + "): ",
                       0, MAXV);

  resetGraph();
  n = newN;

  if (n == 0) {
    cout << "Build complete: 0 vertices, 0 arcs.\n";
    return;
  }
  cout << "Directed weighted arcs. Enter u v w with vertices in 0.." << (n - 1)
       << " and weight 1..99.\n";
  int e = 0;
  while (true) {
    e++;
    cout << "\nArc " << e << "\n";
    int u = readInt("  From vertex (-1 to stop): ");
    if (u == -1) break;
    int v = readInt("  To vertex (-1 to stop): ");
    if (v == -1) break;
    int w = readInt("  Weight (1..99): ");
    if (u < 0 || u >= n || v < 0 || v >= n) {
      cout << "  Rejected: Vertices must be in 0.." << (n - 1) << ".\n";
      e--;
      continue;
    }
    if (w < 1 || w > 99) {
      cout << "  Rejected: Weight must be in 1..99.\n";
      e--;
      continue;
    }
    if (weightG[u][v] != 0) {
      cout << "  Rejected: That arc already exists.\n";
      e--;
      continue;
    }
    addArc(u, v, w);
  }

  cout << "\nBuild complete: " << n
       << (n == 1 ? " vertex, " : " vertices, ") << edgeCount
       << (edgeCount == 1 ? " arc.\n" : " arcs.\n");
}

static void loadDemo() {
  resetGraph();
  n = 7;

  cout << "Weighted DAG demo: 7 vertices.\n";
  cout << "  Arcs 0-2 (1), 2-1 (1), 2-3 (4), 1-4 (2), 3-4 (1), 4-5 (3),\n";
  cout << "  0-6 (5).\n";
  cout << "  Vertex 6 comes before vertex 1 in topological order, so the\n";
  cout << "  numbers 0..6 are not an order you may relax in.\n\n";

  addArc(0, 2, 1);
  addArc(2, 1, 1);
  addArc(2, 3, 4);
  addArc(1, 4, 2);
  addArc(3, 4, 1);
  addArc(4, 5, 3);
  addArc(0, 6, 5);

  cout << "Demo loaded: 7 vertices, " << edgeCount << " arcs.\n";
}

static void loadCyclicDemo() {
  resetGraph();
  n = 6;

  cout << "Cyclic demo: 6 vertices.\n";
  cout << "  Path 0-1-2-3-4-5 with 5-3 closing a cycle over V3, V4, V5.\n";
  cout << "  Kahn drains the queue at V2 and leaves three vertices behind.\n\n";

  addArc(0, 1, 2);
  addArc(1, 2, 3);
  addArc(2, 3, 1);
  addArc(3, 4, 4);
  addArc(4, 5, 2);
  addArc(5, 3, 5);

  cout << "Demo loaded: 6 vertices, " << edgeCount << " arcs.\n";
}

static void showAdjacency() {
  cout << "\n--- Weighted Adjacency List ---\n";
  if (n == 0) {
    cout << "(Empty)\n";
    return;
  }
  for (int i = 0; i < n; i++) {
    cout << setw(3) << i << ":";
    if (adj[i].empty()) cout << " (None)";
    else
      for (int v : adj[i]) cout << " " << v << "(" << weightG[i][v] << ")";
    cout << "\n";
  }
  cout << "Each entry is a neighbour with its weight in brackets, in insertion order.\n";
}

static string distText(int d) {
  if (d >= INF) return "inf";
  if (d <= -INF) return "-inf";
  return to_string(d);
}

static void countInDegrees() {
  for (int i = 0; i < n; i++) indeg[i] = 0;
  for (int u = 0; u < n; u++) {
    for (int v : adj[u]) indeg[v]++;
  }
}

static bool buildTopo() {
  countInDegrees();
  topo.clear();

  vector<int> q;
  int front = 0;
  for (int i = 0; i < n; i++) {
    if (indeg[i] == 0) q.push_back(i);
  }
  while (front < (int)q.size()) {
    int u = q[front];
    front++;
    topo.push_back(u);
    for (int v : adj[u]) {
      indeg[v]--;
      if (indeg[v] == 0) q.push_back(v);
    }
  }
  return (int)topo.size() == n;
}

static bool sortOrExplain() {
  countInDegrees();
  cout << "\nIn-degree:";
  for (int i = 0; i < n; i++) cout << " " << indeg[i];
  cout << "\n";

  if (buildTopo()) return true;

  cout << "Order stopped after " << topo.size() << " of " << n
       << " vertices:";
  for (int v : topo) cout << " " << v;
  cout << "\nThe remaining vertices sit on or behind a cycle, so there is no\n";
  cout << "order in which every arc points forward. The DP needs one.\n";
  return false;
}

static void initDist(int src, bool longest) {
  for (int i = 0; i < MAXV; i++) {
    dist[i] = longest ? -INF : INF;
    prevOf[i] = -1;
  }
  dist[src] = 0;
}

static void relaxFrom(int u, bool longest, bool verbose) {
  if (dist[u] >= INF || dist[u] <= -INF) {
    if (verbose) {
      cout << "Skip V" << u << ": No path reaches it yet, so its arcs"
           << " relax nothing.\n";
    }
    return;
  }
  for (int v : adj[u]) {
    int w = weightG[u][v];
    int cand = dist[u] + w;
    bool better = longest ? (cand > dist[v]) : (cand < dist[v]);
    if (verbose) {
      cout << "  " << u << " -> " << v << " (Weight " << w << "): " << dist[u]
           << " + " << w << " = " << cand;
    }
    if (better) {
      if (verbose) {
        cout << " beats " << distText(dist[v]) << ", so dist[" << v << "] = "
             << cand << ".\n";
      }
      dist[v] = cand;
      prevOf[v] = u;
    } else if (verbose) {
      cout << ", not better than " << distText(dist[v]) << ", keep "
           << distText(dist[v]) << ".\n";
    }
  }
}

static void printPath(int src, int target) {
  vector<int> path;
  int at = target;
  while (at != -1) {
    path.push_back(at);
    if (at == src) break;
    at = prevOf[at];
  }
  if (path.empty() || path[path.size() - 1] != src) {
    cout << "(No path)";
    return;
  }
  for (int i = (int)path.size() - 1; i >= 0; i--) {
    if (i < (int)path.size() - 1) cout << " -> ";
    cout << path[i];
  }
}

static void printResult(int src, bool longest) {
  cout << "\nDistance :";
  for (int i = 0; i < n; i++) cout << " " << distText(dist[i]);
  cout << "\n";

  cout << "Paths:\n";
  for (int i = 0; i < n; i++) {
    cout << "  V" << i << ": ";
    if (i == src) {
      cout << src << " (Start, cost 0)\n";
    } else if (dist[i] >= INF || dist[i] <= -INF) {
      cout << "(No path)\n";
    } else {
      printPath(src, i);
      cout << "  cost " << dist[i] << "\n";
    }
  }

  int far = -1;
  for (int i = 0; i < n; i++) {
    if (dist[i] >= INF || dist[i] <= -INF) continue;
    if (far == -1 || dist[i] > dist[far]) far = i;
  }
  if (far < 0) {
    cout << "Nothing is reachable from V" << src << ".\n";
    return;
  }
  if (far == src) {
    cout << "Nothing leaves V" << src << ", so every walk is just the start.\n";
    return;
  }
  if (longest) {
    cout << "Longest walk from V" << src << " ends at V" << far
         << " with cost " << dist[far] << ".\n";
  } else {
    cout << "Farthest vertex from V" << src << " is V" << far
         << ", at shortest cost " << dist[far] << ".\n";
  }
}

static void runDp(bool longest) {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }

  int src = readRange("  Source vertex (0.." + to_string(n - 1) + "): ", 0,
                      n - 1);

  cout << "\n--- " << (longest ? "Longest" : "Shortest")
       << " paths from V" << src << " ---\n";
  if (!sortOrExplain()) return;

  cout << "Topological order:";
  for (int v : topo) cout << " " << v;
  cout << "\n";

  initDist(src, longest);
  cout << "\nRelaxing one arc at a time, in that order:\n";
  for (int u : topo) relaxFrom(u, longest, true);
  printResult(src, longest);

  cout << "Every arc was relaxed once, and only after its tail had a final\n";
  cout << "distance, which is exactly what the topological order buys.\n";
}

static void runOrderCompare() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }

  int src = readRange("  Source vertex (0.." + to_string(n - 1) + "): ", 0,
                      n - 1);

  cout << "\n--- Why the order matters ---\n";
  if (!sortOrExplain()) return;

  cout << "Topological order:";
  for (int v : topo) cout << " " << v;
  cout << "\n";

  initDist(src, false);
  for (int u : topo) relaxFrom(u, false, false);
  int good[MAXV];
  for (int i = 0; i < n; i++) good[i] = dist[i];

  initDist(src, false);
  for (int u = 0; u < n; u++) relaxFrom(u, false, false);

  cout << "\nIn topological order :";
  for (int i = 0; i < n; i++) cout << " " << distText(good[i]);
  cout << "\nIn vertex-number order:";
  for (int i = 0; i < n; i++) cout << " " << distText(dist[i]);
  cout << "\n";

  int bad = 0;
  for (int i = 0; i < n; i++) {
    if (good[i] == dist[i]) continue;
    cout << "  Mismatch at V" << i << ": " << distText(good[i])
         << " in order, " << distText(dist[i]) << " by number.\n";
    bad++;
  }

  if (bad == 0) {
    cout << "\nThe two orders happen to agree on this graph. Move the arcs so a\n";
    cout << "predecessor has the larger number and they will disagree.\n";
    return;
  }

  cout << "\nRelaxing out of order never finds a shorter path than the truth:\n";
  cout << "an arc is used before its tail has its final distance, so the\n";
  cout << "cheap route through that arc is missed and can never come back.\n";
}

static string arcLabel(int u, int v) {
  return "(" + to_string(u) + " -> " + to_string(v) + ")";
}

static void addArcInteractive() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }

  cout << "\nAdd arc (V = " << n << ", so vertices 0.." << (n - 1) << ")\n";
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
      cout << "  Please re-enter this arc.\n";
      continue;
    }
    if (weightG[u][v] != 0) {
      cout << "  Rejected: " << arcLabel(u, v) << " already exists.\n";
      cout << "  Please re-enter this arc.\n";
      continue;
    }
    int w = readInt("  Weight (1..99, -1 to cancel): ");
    if (w == -1) {
      cout << "  Cancelled.\n";
      return;
    }
    if (w < 1 || w > 99) {
      cout << "  Rejected: Weight must be in 1..99.\n";
      cout << "  Please re-enter this arc.\n";
      continue;
    }

    addArc(u, v, w);
    cout << "  Added " << arcLabel(u, v) << " with weight " << w << "\n";
    cout << "    weightG[" << u << "][" << v << "] = " << w << "\n";
    cout << "  E = " << edgeCount << (edgeCount == 1 ? " arc" : " arcs")
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
      if (weightG[u][v] != 0) edgeCount++;
    }
  }
}

static void removeArcInteractive() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }

  cout << "\nRemove an arc (V = " << n << ", so vertices 0.." << (n - 1)
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
      cout << "  Please re-enter this arc.\n";
      continue;
    }
    if (weightG[u][v] == 0) {
      cout << "  Rejected: " << arcLabel(u, v) << " is not in the graph.\n";
      cout << "    Arcs are ordered: " << v << " -> " << u << " is a separate"
           << " arc and is not removed.\n";
      cout << "  Please re-enter this arc.\n";
      continue;
    }

    int w = weightG[u][v];
    weightG[u][v] = 0;
    removeNeighbour(u, v);
    edgeCount--;

    cout << "  Removed " << arcLabel(u, v) << " (Weight was " << w << ")\n";
    cout << "    weightG[" << u << "][" << v << "] = 0\n";
    cout << "  E = " << edgeCount << (edgeCount == 1 ? " arc" : " arcs")
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
    weightG[fresh][i] = 0;
    weightG[i][fresh] = 0;
  }
  adj[fresh].clear();
  n++;

  cout << "\nAdded V" << fresh << " with no arcs.\n";
  cout << "  V = " << n << ", E = " << edgeCount << ".\n";
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
      kept[na][nb] = weightG[a][b];
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
    for (int b = 0; b < MAXV; b++) weightG[a][b] = 0;
  }
  for (int a = 0; a < n; a++) {
    adj[a] = keptAdj[a];
    for (int b = 0; b < n; b++) weightG[a][b] = kept[a][b];
  }
  recountEdges();

  cout << "\nRemoved V" << v << ".\n";
  if (v < n) cout << "  Vertices above V" << v << " shift down by one.\n";
  cout << "  Arcs lost: " << before - edgeCount;
  cout << " (Recounted from the matrix.)\n";
  cout << "  V = " << n << ", E = " << edgeCount << ".\n";
}

static void clearAllArcs() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }
  if (edgeCount == 0) {
    cout << "\nThe graph already has no arcs.\n";
    return;
  }

  int before = edgeCount;
  for (int u = 0; u < n; u++) {
    adj[u].clear();
    for (int w = 0; w < n; w++) weightG[u][w] = 0;
  }
  edgeCount = 0;

  cout << "\nRemoved all " << before
       << (before == 1 ? " arc.\n" : " arcs.\n");
  cout << "  V = " << n << ", E = 0. The vertices are still there.\n";
}

static void graphOperationsMenu() {
  while (true) {
    cout << "\n--- Graph operations ---\n";
    cout << "Graph: Directed weighted | V = " << n << ", E = " << edgeCount
         << "\n";
    cout << "1. Add a weighted arc to the current graph\n";
    cout << "2. Remove an arc from the current graph\n";
    cout << "3. Add a vertex to the current graph\n";
    cout << "4. Remove a vertex from the current graph\n";
    cout << "5. Clear all arcs\n";
    cout << "0. Back to the main menu\n";

    int choice = readInt("Select: ");

    if (choice == 1) {
      addArcInteractive();
    } else if (choice == 2) {
      removeArcInteractive();
    } else if (choice == 3) {
      addVertexInteractive();
    } else if (choice == 4) {
      removeVertexInteractive();
    } else if (choice == 5) {
      clearAllArcs();
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
    cout << "\n=== G13: DAG Shortest and Longest Paths ===\n";
    cout << "Graph: Directed weighted | V = " << n << ", E = " << edgeCount
         << "\n";
    cout << "1. Build a weighted DAG\n";
    cout << "2. Load weighted demo (7 vertices)\n";
    cout << "3. Load cyclic demo (6 vertices)\n";
    cout << "4. Graph operations\n";
    cout << "5. Topological order\n";
    cout << "6. Shortest paths\n";
    cout << "7. Longest paths\n";
    cout << "8. Shortest by vertex number (Wrong order)\n";
    cout << "9. Show weighted adjacency list\n";
    cout << "10. Exit\n";

    int choice = readInt("Select: ");

    if (choice == 1) {
      buildGraph();
    } else if (choice == 2) {
      loadDemo();
    } else if (choice == 3) {
      loadCyclicDemo();
    } else if (choice == 4) {
      graphOperationsMenu();
    } else if (choice == 5) {
      if (n == 0) {
        cout << "Build the graph first.\n";
      } else if (sortOrExplain()) {
        cout << "Topological order:";
        for (int v : topo) cout << " " << v;
        cout << "\n";
      }
    } else if (choice == 6) {
      runDp(false);
    } else if (choice == 7) {
      runDp(true);
    } else if (choice == 8) {
      runOrderCompare();
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
