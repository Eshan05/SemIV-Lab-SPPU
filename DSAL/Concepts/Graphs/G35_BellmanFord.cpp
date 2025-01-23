// 24. Bellman-Ford: Relax every edge, over and over, until a whole round
// changes nothing. Nothing is ever settled, so a negative weight cannot break
// the method, and V - 1 rounds are enough because no shortest path repeats a
// vertex. The same walk then answers a second question for free: If an edge
// still improves after those rounds, its ends sit on a cycle that gets cheaper
// with every lap. Demos: A negative edge that Dijkstra would mishandle, and a
// graph whose answer does not exist at all.

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

static int dist[MAXV];
static int parentOf[MAXV];

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

static string distText(int v) {
  if (dist[v] == INF) return "-";
  return to_string(dist[v]);
}

static void printDistRow() {
  cout << "  Vertex :";
  for (int v = 0; v < n; v++) cout << setw(4) << v;
  cout << "\n  Dist   :";
  for (int v = 0; v < n; v++) cout << setw(4) << distText(v);
  cout << "\n  Parent :";
  for (int v = 0; v < n; v++) {
    if (parentOf[v] == -1) cout << setw(4) << "-";
    else cout << setw(4) << parentOf[v];
  }
  cout << "\n";
}

static void printPath(int v, int start) {
  vector<int> path;
  int cur = v;
  while (cur != -1) {
    path.push_back(cur);
    cur = parentOf[cur];
  }
  for (int i = (int)path.size() - 1; i >= 0; i--) {
    cout << (i == (int)path.size() - 1 ? "" : " - ") << path[i];
  }
  if (path.empty() || path.back() != start) cout << "?";
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
  cout << "Directed edges with weights -99..99. A negative weight is allowed\n";
  cout << "here, which is exactly why this method is not Dijkstra.\n";
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
  n = 8;
  cout << "Demo: 8 vertices, 8 edges, one negative weight, no negative cycle.\n";
  cout << "  The chain 0 -> 1 -> 2 -> 3 -> 4 -> 5 -> 6 -> 7 is listed in exactly\n";
  cout << "  the wrong order, from the far end backwards, so each round can only\n";
  cout << "  push the frontier one hop further. That is how the V - 1 bound gets\n";
  cout << "  used up, and it is why the round counter is worth watching.\n";
  cout << "  The extra edge 4 -> 7 has weight -6, so V7 ends up cheaper than V4\n";
  cout << "  even though reaching V4 costs 5. No distance order survives that.\n\n";

  addEdge(6, 7, 1);
  addEdge(5, 6, 1);
  addEdge(4, 5, 3);
  addEdge(3, 4, 2);
  addEdge(2, 3, 1);
  addEdge(1, 2, 1);
  addEdge(0, 1, 1);
  addEdge(4, 7, -6);

  cout << "Demo loaded: " << n << " vertices, " << edgeCount << " edges.\n";
}

static void loadCycleDemo() {
  resetGraph();
  n = 4;
  cout << "Negative-cycle demo: 4 vertices, and V1 -> V2 -> V1 costs -4.\n";
  cout << "  0 -> 1 (3), 1 -> 2 (-5), 2 -> 1 (1), 2 -> 3 (2).\n";
  cout << "  Every lap of that cycle is four cheaper than the last, so the walk\n";
  cout << "  from V0 through V1 has no cheapest form at all. Watch the distances\n";
  cout << "  keep dropping below anything a path could reasonably cost.\n\n";

  addEdge(0, 1, 3);
  addEdge(1, 2, -5);
  addEdge(2, 1, 1);
  addEdge(2, 3, 2);

  cout << "Demo loaded: " << n << " vertices, " << edgeCount << " edges.\n";
}

static void loadKillerDemo() {
  resetGraph();
  n = 3;
  cout << "Dijkstra-killer demo: 3 vertices where greedy settling gives the\n";
  cout << "wrong answer.\n";
  cout << "  0 -> 1 (1), 0 -> 2 (2), 2 -> 1 (-2).\n";
  cout << "  Dijkstra settles V1 at 1, because 1 is the nearest vertex it can see.\n";
  cout << "  Only then does it meet the -2 edge from V2, and V1 is already closed.\n";
  cout << "  The true distance is 0. Bellman-Ford simply offers that edge again.\n\n";

  addEdge(0, 1, 1);
  addEdge(0, 2, 2);
  addEdge(2, 1, -2);

  cout << "Demo loaded: " << n << " vertices, " << edgeCount << " edges.\n";
}

static void runBellmanFord() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }

  cout << "\nStart from which vertex?\n";
  int start = readRange("  Source vertex: ", 0, n - 1);

  for (int v = 0; v < n; v++) {
    dist[v] = INF;
    parentOf[v] = -1;
  }
  dist[start] = 0;

  cout << "\n--- Bellman-Ford from V" << start << " ---\n";
  cout << "One round walks the whole edge list once and offers every edge to its\n";
  cout << "target. After k rounds every distance that needs at most k edges is\n";
  cout << "final, so V - 1 rounds cover any shortest path, since a shortest path\n";
  cout << "never repeats a vertex. Here V - 1 = " << (n - 1) << ".\n";
  cout << "An edge whose source is still unreachable is skipped, and a negative\n";
  cout << "weight is no trouble at all. That is the whole point of the method.\n";

  long long offers = 0;
  int improvements = 0;
  int roundsUsed = 0;
  bool stoppedEarly = false;

  for (int round = 1; round <= n - 1; round++) {
    bool verbose = (round == 1);
    int changes = 0;
    roundsUsed = round;

    cout << "\n--- Round " << round << " of " << (n - 1) << " ---\n";
    if (verbose) {
      cout << "  Every edge is checked this round, so each verdict is printed.\n";
    } else {
      cout << "  Only edges that improve something are printed from here on.\n";
    }

    for (int id = 0; id < edgeCount; id++) {
      int u = fromSide[id];
      int v = toSide[id];
      int w = wEdge[id];
      if (dist[u] == INF) {
        if (verbose) {
          cout << "    " << edgeLabelOf(id) << ": V" << u
               << " is still unreachable, so skip.\n";
        }
        continue;
      }

      offers++;
      int offer = dist[u] + w;
      if (offer < dist[v]) {
        cout << "    " << edgeLabelOf(id) << ": " << dist[u]
             << (w < 0 ? " - " : " + ") << abs(w) << " = " << offer
             << ", so Dist[" << v << "] = " << offer << " through V" << u << ".\n";
        dist[v] = offer;
        parentOf[v] = u;
        changes++;
        improvements++;
      } else if (verbose) {
        cout << "    " << edgeLabelOf(id) << ": " << dist[u]
             << (w < 0 ? " - " : " + ") << abs(w) << " = " << offer
             << ", but Dist[" << v << "] is " << distText(v) << " already.\n";
      }
    }

    cout << "  After round " << round << ":\n";
    printDistRow();

    if (changes == 0) {
      stoppedEarly = true;
      cout << "  Nothing changed, so nothing is left to fix.\n";
      if (round + 1 == n - 1) {
        cout << "  Round " << (round + 1) << " is skipped, which costs nothing.\n";
      } else if (round < n - 1) {
        cout << "  Rounds " << (round + 1) << ".." << (n - 1)
             << " are skipped, which costs nothing.\n";
      } else {
        cout << "  This was the last round of the bound, so the run would end here\n";
        cout << "  anyway.\n";
      }
      break;
    }
    cout << "  Improvements: " << changes << ".\n";
  }

  if (n == 1) {
    cout << "\nV - 1 = 0, so there are no rounds at all. The source is the only\n";
    cout << "vertex, and it is already at distance 0.\n";
  }

  cout << "\n--- Looking for a negative cycle ---\n";
  cout << "One more pass over the edge list. After V - 1 rounds nothing should\n";
  cout << "improve, so an improvement now means a cycle is still getting cheaper.\n";

  int badId = -1;
  int badOffer = 0;
  for (int id = 0; id < edgeCount; id++) {
    int u = fromSide[id];
    int v = toSide[id];
    if (dist[u] == INF) continue;
    if (dist[u] + wEdge[id] < dist[v]) {
      badId = id;
      badOffer = dist[u] + wEdge[id];
      break;
    }
  }

  int reached = 0;
  for (int v = 0; v < n; v++) {
    if (dist[v] != INF) reached++;
  }

  if (badId == -1) {
    cout << "  Every edge was offered again and none improved, so no negative\n";
    cout << "  cycle can be reached from V" << start << ". The distances above are\n";
    cout << "  the answers.\n";
    if (reached < n) {
      cout << "  Out of reach from V" << start << ":";
      for (int v = 0; v < n; v++) {
        if (dist[v] != INF) continue;
        cout << " V" << v;
      }
      cout << "\n  A cycle among those vertices is still not a path from V" << start
           << ", so it\n  cannot change an answer here.\n";
    }

    cout << "\n--- Distances from V" << start << " ---\n";
    printDistRow();

    cout << "\nPaths:\n";
    for (int v = 0; v < n; v++) {
      if (dist[v] == INF) continue;
      cout << "  V" << v << " at " << dist[v] << "   ";
      printPath(v, start);
      cout << "\n";
    }
    for (int v = 0; v < n; v++) {
      if (dist[v] != INF) continue;
      cout << "  V" << v << " at (Unreachable)\n";
    }

    cout << "\nReached " << reached << " of " << n << " vertices. Rounds used: "
         << roundsUsed;
    if (stoppedEarly && roundsUsed < n - 1) {
      cout << ", and the rest were skipped.\n";
    } else {
      cout << ", which is the whole bound.\n";
    }
    cout << "Edges offered: " << offers << ", improvements: " << improvements
         << ".\n";

    cout << "\nNo round ever settles a vertex, so no weight can arrive too late.\n";
    cout << "That is the difference from Dijkstra, which is why the negative edge\n";
    cout << "above was accepted without a second thought. The price is the round\n";
    cout << "count: V - 1 rounds of E edges is O(V x E), so it loses to Dijkstra\n";
    cout << "on any graph whose weights are all non-negative.\n";
    return;
  }

  int badFrom = fromSide[badId];
  int badTo = toSide[badId];
  cout << "  " << edgeLabelOf(badId) << " still improves: " << dist[badFrom]
       << (wEdge[badId] < 0 ? " - " : " + ") << abs(wEdge[badId]) << " = "
       << badOffer << ", which beats " << dist[badTo] << ".\n";
  cout << "  After V - 1 rounds that cannot happen unless a cycle keeps shrinking.\n";

  int cur = badTo;
  for (int i = 0; i < n && cur != -1; i++) cur = parentOf[cur];

  if (cur == -1) {
    cout << "  Following the parents ran out of vertices before it looped, which\n";
    cout << "  should be impossible at this point, so something is off.\n";
    return;
  }

  vector<int> cycle;
  int walk = cur;
  do {
    cycle.push_back(walk);
    walk = parentOf[walk];
  } while (walk != cur && walk != -1 && (int)cycle.size() <= n);
  reverse(cycle.begin(), cycle.end());

  cout << "\n  Walking parents from V" << badTo << " for " << n
       << " steps lands on V" << cur << ", which must be\n";
  cout << "  inside the cycle, since the walk cannot go on forever.\n";
  cout << "  The cycle is ";
  for (int i = 0; i < (int)cycle.size(); i++) {
    cout << (i == 0 ? "V" : " -> V") << cycle[i];
  }
  cout << " -> V" << cycle.front() << ".\n";

  int total = 0;
  for (int i = 0; i < (int)cycle.size(); i++) {
    int a = cycle[i];
    int b = cycle[(i + 1) % cycle.size()];
    total += wG[a][b];
  }
  cout << "  One lap costs ";
  for (int i = 0; i < (int)cycle.size(); i++) {
    int a = cycle[i];
    int b = cycle[(i + 1) % cycle.size()];
    cout << (i == 0 ? "" : " + ") << "(" << wG[a][b] << ")";
  }
  cout << " = " << total << ".\n";

  cout << "\n--- Distances from V" << start << " ---\n";
  printDistRow();
  cout << "\nReached " << reached << " of " << n << " vertices. Rounds used: "
       << roundsUsed << ", which is the whole bound.\n";
  cout << "Edges offered: " << offers << ", improvements: " << improvements
       << ".\n";

  cout << "\nThose numbers are not shortest paths, and no amount of extra rounds\n";
  cout << "would make them so. Every lap of the cycle adds " << total
       << " to a path, so a\n";
  cout << "cheaper route always exists and the question has no smallest answer.\n";
  cout << "Bellman-Ford is the method that tells you so instead of answering\n";
  cout << "anyway, and it costs one extra pass to find out.\n";
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
  cout << "  With no edges, only the source can be reached.\n";
}

static void operationsMenu() {
  while (true) {
    cout << "\n--- Graph operations ---\n";
    cout << "Graph: Directed, weighted -99..99 | V = " << n << ", E = "
         << edgeCount << "\n";
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
    cout << "\n=== G35: Bellman-Ford ===\n";
    cout << "Graph: Directed, weighted -99..99 | V = " << n << ", E = "
         << edgeCount << "\n";
    cout << "1. Build a graph with weights -99..99\n";
    cout << "2. Load demo graph (8 vertices, one negative edge)\n";
    cout << "3. Load negative-cycle demo (4 vertices)\n";
    cout << "4. Load Dijkstra-killer demo (3 vertices)\n";
    cout << "5. Graph operations\n";
    cout << "6. Run Bellman-Ford from a vertex\n";
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
      loadKillerDemo();
    } else if (choice == 5) {
      operationsMenu();
    } else if (choice == 6) {
      runBellmanFord();
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
