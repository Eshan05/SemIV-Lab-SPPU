// 27. Johnson: Reweight the graph once so every weight is non-negative, then run
// Dijkstra from each vertex. One Bellman-Ford pass from a helper vertex gives
// the potentials h[v], and the arc u -> v of weight w becomes w + h[u] - h[v],
// which can never be negative. Every route keeps its order, so the Dijkstra
// results turn back into real distances with one subtraction. Demo: Six vertices
// where a -4 edge would break Dijkstra, six runs instead of one O(V^3) table,
// and a 3-vertex negative cycle where Johnson stops before step 2.

#include <algorithm>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
using namespace std;

const int MAXV = 15;
const int MAXE = 120;
const int INF = 1000000;

static int n = 0;
static int edgeCount = 0;
static int wG[MAXV][MAXV];
static bool hasArc[MAXV][MAXV];
static int fromSide[MAXE];
static int toSide[MAXE];
static int wEdge[MAXE];
static vector<pair<int, int>> adj[MAXV];

static int d[MAXV][MAXV];
static int nextHop[MAXV][MAXV];
static int dPrime[MAXV][MAXV];
static bool tableValid = false;
static bool negCycle = false;

static int pot[MAXV];
static int dist[MAXV];
static int prevOf[MAXV];
static bool doneV[MAXV];
static int settledOrder[MAXV];
static int settledCount = 0;

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
    for (int j = 0; j < MAXV; j++) {
      wG[i][j] = 0;
      hasArc[i][j] = false;
    }
  }
  n = 0;
  edgeCount = 0;
  tableValid = false;
  negCycle = false;
}

static int addEdge(int u, int v, int w) {
  int id = edgeCount;
  fromSide[id] = u;
  toSide[id] = v;
  wEdge[id] = w;
  wG[u][v] = w;
  hasArc[u][v] = true;
  adj[u].push_back(make_pair(v, w));
  edgeCount++;
  return id;
}

static string edgeLabelOf(int id) {
  return "E" + to_string(id) + " (" + to_string(fromSide[id]) + " -> " +
         to_string(toSide[id]) + ", weight " + to_string(wEdge[id]) + ")";
}

static void dropEdgeId(int removed) {
  for (int id = removed; id < edgeCount - 1; id++) {
    fromSide[id] = fromSide[id + 1];
    toSide[id] = toSide[id + 1];
    wEdge[id] = wEdge[id + 1];
  }
  edgeCount--;
}

static const char *tableState() {
  if (negCycle) return "No table";
  if (!tableValid) return "Out of date";
  return "Up to date";
}

static string signedTerm(int x) {
  if (x < 0) return "(" + to_string(x) + ")";
  return to_string(x);
}


static void printDistGrid() {
  int w = 6;
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      if (d[i][j] >= INF) continue;
      int len = (int)to_string(d[i][j]).size();
      if (len + 2 > w) w = len + 2;
    }
  }
  cout << "      ";
  for (int j = 0; j < n; j++) cout << setw(w) << j;
  cout << "\n";
  for (int i = 0; i < n; i++) {
    cout << setw(4) << i << "  ";
    for (int j = 0; j < n; j++) {
      if (d[i][j] >= INF) cout << setw(w) << "-";
      else cout << setw(w) << d[i][j];
    }
    cout << "\n";
  }
}


static void printRoute(int u, int v) {
  if (d[u][v] >= INF) {
    cout << "(None)";
    return;
  }
  cout << u;
  int cur = u;
  int guard = 0;
  while (cur != v && guard <= n) {
    int nx = nextHop[cur][v];
    if (nx == -1) {
      cout << " ?";
      return;
    }
    cout << " - " << nx;
    cur = nx;
    guard++;
  }
}


static void printTable() {
  cout << "\n--- Distance matrix ---\n";
  printDistGrid();

  cout << "\n--- Routes ---\n";
  int pairs = 0;
  int missing = 0;
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      if (i == j) continue;
      if (d[i][j] >= INF) {
        missing++;
        continue;
      }
      pairs++;
      cout << "  V" << i << " -> V" << j << " at " << setw(4) << d[i][j] << "   ";
      printRoute(i, j);
      cout << "\n";
    }
  }
  if (pairs == 0) cout << "  (None)\n";
  if (missing > 0) {
    cout << "  Ordered pairs with no route at all: " << missing << ".\n";
  }
}

static bool bellmanFordPotentials() {
  for (int v = 0; v < n; v++) pot[v] = 0;

  cout << "\n--- Step 1: Bellman-Ford from a helper vertex ---\n";
  cout << "Add a helper q with a 0-weight arc to every vertex. q is not part of\n";
  cout << "the graph and never appears in a route; it only gives Bellman-Ford a\n";
  cout << "single source that reaches everything, and it makes every h start\n";
  cout << "at 0.\n";
  cout << "  V = " << n << ", so " << (n - 1)
       << (n - 1 == 1 ? " round" : " rounds")
       << " settle every h, and one more round\n";
  cout << "  that still relaxes an edge is proof of a negative cycle.\n";

  bool settled = false;
  bool cycle = false;
  for (int round = 1; round <= n && !settled && !cycle; round++) {
    cout << "\n  Round " << round << " of " << n << ":\n";
    int improved = 0;
    for (int id = 0; id < edgeCount; id++) {
      int u = fromSide[id];
      int v = toSide[id];
      int cand = pot[u] + wEdge[id];
      if (cand >= pot[v]) continue;
      cout << "    h[" << v << "] = " << signedTerm(pot[u]) << " + "
           << signedTerm(wEdge[id]) << " = " << cand << " through " << u
           << " -> " << v << ", down from " << pot[v] << ".\n";
      pot[v] = cand;
      improved++;
    }
    if (improved == 0) {
      cout << "    Nothing relaxes, so every h is final.\n";
      settled = true;
    } else if (round == n) {
      cout << "    Round " << n << " still relaxes, so some route keeps getting\n";
      cout << "    cheaper however many rounds it is given.\n";
      cycle = true;
    }
  }

  if (cycle) return true;

  cout << "\n  Potentials after the pass:\n";
  for (int v = 0; v < n; v++) cout << "    h[" << v << "] = " << pot[v] << "\n";
  cout << "  h[v] is the cheapest cost of reaching Vv from q, so h[v] is at most\n";
  cout << "  h[u] + w for every arc u -> v. Rearranged, that reads\n";
  cout << "  w + h[u] - h[v] >= 0, which is exactly the promise Johnson needs.\n";
  return false;
}

static void dijkstraFrom(int s) {
  for (int v = 0; v < n; v++) {
    dist[v] = INF;
    prevOf[v] = -1;
    doneV[v] = false;
  }
  dist[s] = 0;
  settledCount = 0;

  for (int step = 0; step < n; step++) {
    int best = -1;
    for (int v = 0; v < n; v++) {
      if (doneV[v] || dist[v] >= INF) continue;
      if (best == -1 || dist[v] < dist[best]) best = v;
    }
    if (best == -1) break;
    doneV[best] = true;
    settledOrder[settledCount++] = best;

    for (int i = 0; i < (int)adj[best].size(); i++) {
      int v = adj[best][i].first;
      int newW = adj[best][i].second + pot[best] - pot[v];
      int cand = dist[best] + newW;
      if (cand < dist[v]) {
        dist[v] = cand;
        prevOf[v] = best;
      }
    }
  }
}

static void recordRoute(int s, int v) {
  nextHop[s][v] = -1;
  if (v == s || dist[v] >= INF || prevOf[v] == -1) return;
  int cur = v;
  int guard = 0;
  while (prevOf[cur] != -1 && prevOf[cur] != s && guard <= n) {
    cur = prevOf[cur];
    guard++;
  }
  if (prevOf[cur] == s) nextHop[s][v] = cur;
}

static void runJohnson() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }

  cout << "\n--- Johnson's algorithm ---\n";
  cout << "Dijkstra needs non-negative weights. One negative weight is enough to\n";
  cout << "break its argument, because a settled vertex could still be beaten by a\n";
  cout << "route that turns up later. Johnson does not change the graph, it\n";
  cout << "changes the numbers, in a way that keeps every route in the same order.\n";

  if (bellmanFordPotentials()) {
    tableValid = false;
    negCycle = true;
    cout << "\nJohnson stops here, before any Dijkstra run. Potentials exist only\n";
    cout << "when the graph has no negative cycle, and here some h keeps falling on\n";
    cout << "every extra round. A pair that can reach the cycle and leave it again\n";
    cout << "has no shortest route at all, so there is nothing to reweight and\n";
    cout << "nothing to print. Floyd-Warshall reports the same thing on the\n";
    cout << "diagonal, one table of arithmetic later.\n";
    return;
  }

  cout << "\n--- Step 2: Reweight every arc ---\n";
  cout << "Each arc u -> v of weight w becomes w + h[u] - h[v]. Nothing is clamped\n";
  cout << "or thrown away; the potentials shift the cost, and Step 4 undoes the\n";
  cout << "shift exactly.\n\n";
  if (edgeCount == 0) {
    cout << "  The graph has no edges, so there is nothing to reweight.\n\n";
  }
  for (int id = 0; id < edgeCount; id++) {
    int u = fromSide[id];
    int v = toSide[id];
    cout << "  E" << id << " (" << u << " -> " << v << "): "
         << signedTerm(wEdge[id]) << " + " << signedTerm(pot[u]) << " - "
         << signedTerm(pot[v]) << " = "
         << wEdge[id] + pot[u] - pot[v] << "\n";
  }

  cout << "\n--- Step 3: Dijkstra from every vertex, on the new weights ---\n";
  cout << "Every weight is now at least 0, so the settled set of G33 is safe\n";
  cout << "again: The cheapest unsettled cost is final, and no later route can\n";
  cout << "undercut it.\n";

  int runs = 0;
  for (int s = 0; s < n; s++) {
    dijkstraFrom(s);
    runs++;
    cout << "\n  Source V" << s << ":\n";
    for (int t = 0; t < settledCount; t++) {
      int v = settledOrder[t];
      cout << "    Settle V" << v << " at " << dist[v];
      if (prevOf[v] == -1) {
        cout << ", the source itself.\n";
      } else {
        cout << ", through V" << prevOf[v] << ".\n";
      }
    }
    int unreached = n - settledCount;
    if (unreached > 0) {
      cout << "    Never settled: " << unreached
           << (unreached == 1 ? " vertex" : " vertices")
           << " with no route from V" << s << ".\n";
    }
    for (int v = 0; v < n; v++) {
      dPrime[s][v] = dist[v];
      recordRoute(s, v);
    }
  }

  cout << "\n--- Step 4: Turn every distance back ---\n";
  cout << "Each cell turns back as d[u][v] = d'[u][v] - h[u] + h[v]. A route\n";
  cout << "from Vu to Vv pays h[u] once when it starts and gets h[v] back when it\n";
  cout << "ends, and every vertex in between cancels out. So the shift adds the\n";
  cout << "same amount to every route with the same endpoints, and the shortest\n";
  cout << "route in the new weights is the shortest route in the old ones.\n\n";

  for (int u = 0; u < n; u++) {
    for (int v = 0; v < n; v++) {
      if (u == v) {
        d[u][v] = 0;
        continue;
      }
      if (dPrime[u][v] >= INF) {
        d[u][v] = INF;
        continue;
      }
      d[u][v] = dPrime[u][v] - pot[u] + pot[v];
      cout << "  V" << u << " -> V" << v << ": " << dPrime[u][v] << " - "
           << signedTerm(pot[u]) << " + " << signedTerm(pot[v]) << " = "
           << d[u][v] << "\n";
    }
  }

  tableValid = true;
  negCycle = false;
  printTable();

  cout << "\nDijkstra runs: " << runs << ", one per vertex.\n";
  cout << "Johnson's bill: One Bellman-Ford pass of V - 1 rounds over E edges, plus\n";
  cout << "one more round to look for a negative cycle, and then V runs of\n";
  cout << "Dijkstra. With the array Dijkstra of G33 that is O(V x E + V^3); with\n";
  cout << "the heap of G34 it is O(V x E + V x E x log V). On a sparse graph that\n";
  cout << "beats the O(V^3) of Floyd-Warshall, which pays for every cell even when\n";
  cout << "most cells are empty. On a dense graph the two are the same order, and\n";
  cout << "Floyd is the shorter program.\n";
}

static void buildGraph() {
  int fresh = readRange("Number of vertices (0.." + to_string(MAXV) + "): ", 0,
                        MAXV);
  resetGraph();
  n = fresh;
  tableValid = false;

  if (n == 0) {
    cout << "Build complete: 0 vertices, 0 edges.\n";
    return;
  }
  cout << "Directed edges with weights -99..99. Negative weights are allowed, as\n";
  cout << "long as no cycle adds up to less than 0.\n";
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
      cout << "  Rejected: A self-loop is a one-vertex cycle, so it never helps.\n";
      e--;
      continue;
    }
    int w = readInt("  Weight (-99..99): ");
    if (w < -99 || w > 99) {
      cout << "  Rejected: Weights must be in -99..99.\n";
      e--;
      continue;
    }
    if (hasArc[u][v]) {
      cout << "  Rejected: That edge already exists.\n";
      e--;
      continue;
    }
    addEdge(u, v, w);
  }

  cout << "\nBuild complete: " << n
       << (n == 1 ? " vertex, " : " vertices, ") << edgeCount
       << (edgeCount == 1 ? " edge.\n" : " edges.\n");
}

static void loadDemo() {
  resetGraph();
  n = 6;
  tableValid = false;
  cout << "Demo: 6 vertices, 8 edges, one -4 edge, no negative cycle.\n";
  cout << "  0 -> 1 (5), 0 -> 2 (2), 1 -> 2 (-4), 1 -> 3 (6), 2 -> 3 (3),\n";
  cout << "  3 -> 4 (1), 2 -> 4 (9), 4 -> 5 (2).\n";
  cout << "  Dijkstra cannot start here: The -4 arc means a settled vertex could\n";
  cout << "  still be beaten. Johnson gives that arc a potential that lifts it to\n";
  cout << "  0, runs Dijkstra six times on the lifted weights, and subtracts the\n";
  cout << "  lift again at the end.\n\n";

  addEdge(0, 1, 5);
  addEdge(0, 2, 2);
  addEdge(1, 2, -4);
  addEdge(1, 3, 6);
  addEdge(2, 3, 3);
  addEdge(3, 4, 1);
  addEdge(2, 4, 9);
  addEdge(4, 5, 2);

  cout << "Demo loaded: " << n << " vertices, " << edgeCount << " edges.\n";
}

static void loadCycleDemo() {
  resetGraph();
  n = 3;
  tableValid = false;
  cout << "Negative-cycle demo: 3 vertices, and V1 -> V2 -> V1 costs -2.\n";
  cout << "  0 -> 1 (2), 1 -> 2 (-3), 2 -> 1 (1).\n";
  cout << "  Bellman-Ford never settles, because h[1] and h[2] drop by 2 on every\n";
  cout << "  extra round, so no potentials exist and Johnson stops before any\n";
  cout << "  Dijkstra run. Nothing downstream would be meaningful.\n\n";

  addEdge(0, 1, 2);
  addEdge(1, 2, -3);
  addEdge(2, 1, 1);

  cout << "Demo loaded: " << n << " vertices, " << edgeCount << " edges.\n";
}

static void showTable() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }
  if (negCycle) {
    cout << "There is no table to show: The graph has a negative cycle, so some\n";
    cout << "pairs have no shortest route. Johnson stops at step 1 on this graph.\n";
    cout << "Change the graph, then run Johnson's algorithm again.\n";
    return;
  }
  if (!tableValid) {
    cout << "Run Johnson's algorithm first, so this table is up to date.\n";
    return;
  }
  printTable();
}

static void showMatrix() {
  cout << "\n--- Adjacency Matrix ---\n";
  if (n == 0) {
    cout << "(Empty)\n";
    return;
  }
  cout << "      ";
  for (int j = 0; j < n; j++) cout << setw(4) << j;
  cout << "\n";
  for (int i = 0; i < n; i++) {
    cout << setw(4) << i << "  ";
    for (int j = 0; j < n; j++) {
      if (!hasArc[i][j]) cout << setw(4) << "-";
      else cout << setw(4) << wG[i][j];
    }
    cout << "\n";
  }
  cout << "A dash means no edge. The matrix is not symmetric, because row i\n";
  cout << "lists the edges leaving Vi.\n";
}

static void showEdges() {
  cout << "\n--- Edge List ---\n";
  if (n == 0) {
    cout << "(Empty)\n";
    return;
  }
  if (edgeCount == 0) {
    cout << "(None)\n";
    cout << "Vertices are there, but no edge joins them.\n";
    return;
  }
  for (int id = 0; id < edgeCount; id++) {
    cout << "  " << edgeLabelOf(id) << "\n";
  }
  cout << "Each line reads from left to right, and the arrow only works one\n";
  cout << "way.\n";
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
      cout << "  Rejected: A self-loop is a one-vertex cycle, so it never helps.\n";
      cout << "  Please re-enter this edge.\n";
      continue;
    }
    if (hasArc[u][v]) {
      cout << "  Rejected: " << u << " -> " << v << " already exists.\n";
      cout << "    Directed edges are ordered: " << v << " -> " << u
           << " is a different edge.\n";
      cout << "  Please re-enter this edge.\n";
      continue;
    }

    int w = readInt("  Weight (-99..99): ");
    if (w < -99 || w > 99) {
      cout << "  Rejected: Weights must be in -99..99.\n";
      cout << "  Please re-enter this edge.\n";
      continue;
    }

    int id = addEdge(u, v, w);
    cout << "  Added " << edgeLabelOf(id) << "\n";
    cout << "  E = " << edgeCount << (edgeCount == 1 ? " edge" : " edges")
         << " now.\n";
    return;
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
    if (!hasArc[u][v]) {
      cout << "  Rejected: " << u << " -> " << v << " is not in the graph.\n";
      cout << "  Please re-enter this edge.\n";
      continue;
    }

    int found = -1;
    for (int id = 0; id < edgeCount; id++) {
      if (fromSide[id] == u && toSide[id] == v) {
        found = id;
        break;
      }
    }

    hasArc[u][v] = false;
    wG[u][v] = 0;
    for (int i = 0; i < (int)adj[u].size(); i++) {
      if (adj[u][i].first != v) continue;
      adj[u].erase(adj[u].begin() + i);
      break;
    }

    string label = edgeLabelOf(found);
    dropEdgeId(found);

    cout << "  Removed " << label << "\n";
    if (found < edgeCount) {
      cout << "  Edges above E" << found << " shift down by one.\n";
    }
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
    wG[fresh][i] = 0;
    wG[i][fresh] = 0;
    hasArc[fresh][i] = false;
    hasArc[i][fresh] = false;
  }
  adj[fresh].clear();
  n++;

  cout << "\nAdded V" << fresh << " with no edges.\n";
  cout << "  V = " << n << ", E = " << edgeCount << ".\n";
  cout << "  A lone vertex is unreachable until some edge points at it.\n";
}

static void removeVertexInteractive() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }

  cout << "\nRemove a vertex (V = " << n << ", so vertices 0.." << (n - 1)
       << ")\n";
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

  vector<int> keptFrom;
  vector<int> keptTo;
  vector<int> keptW;
  for (int id = 0; id < edgeCount; id++) {
    if (fromSide[id] == v || toSide[id] == v) continue;
    keptFrom.push_back(fromSide[id] < v ? fromSide[id] : fromSide[id] - 1);
    keptTo.push_back(toSide[id] < v ? toSide[id] : toSide[id] - 1);
    keptW.push_back(wEdge[id]);
  }

  int before = edgeCount;
  n--;

  for (int a = 0; a < MAXV; a++) {
    adj[a].clear();
    for (int b = 0; b < MAXV; b++) {
      wG[a][b] = 0;
      hasArc[a][b] = false;
    }
  }

  edgeCount = (int)keptFrom.size();
  for (int id = 0; id < edgeCount; id++) {
    fromSide[id] = keptFrom[id];
    toSide[id] = keptTo[id];
    wEdge[id] = keptW[id];
    wG[fromSide[id]][toSide[id]] = wEdge[id];
    hasArc[fromSide[id]][toSide[id]] = true;
    adj[fromSide[id]].push_back(make_pair(toSide[id], wEdge[id]));
  }

  cout << "\nRemoved V" << v << ".\n";
  if (v < n) cout << "  Vertices above V" << v << " shift down by one.\n";
  cout << "  Edges lost: " << before - edgeCount << ".\n";
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
    for (int w = 0; w < n; w++) {
      wG[u][w] = 0;
      hasArc[u][w] = false;
    }
  }
  edgeCount = 0;

  cout << "\nRemoved all " << before
       << (before == 1 ? " edge.\n" : " edges.\n");
  cout << "  V = " << n << ", E = 0. The vertices are still there.\n";
  cout << "  With no edges, no pair has a route, so the table is all dashes\n";
  cout << "  except the diagonal.\n";
}

static void operationsMenu() {
  while (true) {
    cout << "\n--- Graph operations ---\n";
    cout << "Graph: Directed, weighted -99..99 | V = " << n << ", E = " << edgeCount
         << " | Table: " << tableState() << "\n";
    cout << "1. Add an edge to the current graph\n";
    cout << "2. Remove an edge from the current graph\n";
    cout << "3. Add a vertex to the current graph\n";
    cout << "4. Remove a vertex from the current graph\n";
    cout << "5. Clear all edges\n";
    cout << "0. Back to the main menu\n";

    int beforeN = n;
    int beforeE = edgeCount;
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

    if (n != beforeN || edgeCount != beforeE) {
      tableValid = false;
      negCycle = false;
      cout << "  The all-pairs table is out of date, so run Johnson again.\n";
    }
  }
}

int main() {
  resetGraph();

  while (true) {
    cout << "\n=== G38: Johnson's Algorithm (Sparse Graphs) ===\n";
    cout << "Graph: Directed, weighted -99..99 | V = " << n << ", E = "
         << edgeCount << " | Table: " << tableState() << "\n";
    cout << "1. Build a graph from edges\n";
    cout << "2. Load demo graph (6 vertices)\n";
    cout << "3. Load negative-cycle demo (3 vertices)\n";
    cout << "4. Graph operations\n";
    cout << "5. Run Johnson's algorithm\n";
    cout << "6. Show the all-pairs table\n";
    cout << "7. Show the adjacency matrix\n";
    cout << "8. Show the edge list\n";
    cout << "9. Exit\n";

    int choice = readInt("Select: ");

    if (choice == 1) {
      buildGraph();
    } else if (choice == 2) {
      loadDemo();
    } else if (choice == 3) {
      loadCycleDemo();
    } else if (choice == 4) {
      operationsMenu();
    } else if (choice == 5) {
      runJohnson();
    } else if (choice == 6) {
      showTable();
    } else if (choice == 7) {
      showMatrix();
    } else if (choice == 8) {
      showEdges();
    } else if (choice == 9) {
      cout << "Exiting...\n";
      break;
    } else {
      cout << "Invalid option.\n";
    }
  }

  return 0;
}
