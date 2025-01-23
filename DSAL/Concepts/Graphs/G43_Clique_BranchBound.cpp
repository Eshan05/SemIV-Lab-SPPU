// 30b. The maximum clique, found by branch and bound: Put the busiest vertices
// first, grow a clique one vertex at a time, and give up on a branch as soon as
// even taking every remaining candidate cannot beat the best clique so far.
// Demo: 8 vertices whose largest clique has 4 vertices, followed by a brute
// force that tests every subset and reaches the same answer the slow way.

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

const int MAXV = 15;
const int TRACE_LIMIT = 80;

static int n = 0;
static int edgeCount = 0;
static bool adjM[MAXV][MAXV];
static vector<int> adj[MAXV];

static int order[MAXV];
static int degreeOf[MAXV];

static vector<int> stackClique;
static int bestSize = 0;
static vector<int> bestClique;
static int nodes = 0;
static int leaves = 0;
static int maximalLeaves = 0;
static int prunedByBound = 0;
static int deepest = 0;
static int shown = 0;

static int bruteBest = 0;
static int bruteCount = 0;
static int bruteMask = 0;
static long long bruteCliques = 0;
static bool bruteDone = false;
static bool searchDone = false;

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
    for (int j = 0; j < MAXV; j++) adjM[i][j] = false;
  }
  n = 0;
  edgeCount = 0;
  bestSize = 0;
  bestClique.clear();
  bruteDone = false;
  searchDone = false;
}

static void addEdge(int u, int v) {
  adjM[u][v] = true;
  adjM[v][u] = true;
  adj[u].push_back(v);
  adj[v].push_back(u);
  edgeCount++;
}

static string edgeLabel(int u, int v) {
  return "(" + to_string(u) + " - " + to_string(v) + ")";
}

static string plural(int count, const string &one, const string &many) {
  return to_string(count) + " " + (count == 1 ? one : many);
}

static string listText(const vector<int> &items) {
  string text = "{";
  for (int i = 0; i < (int)items.size(); i++) {
    if (i > 0) text += " ";
    text += to_string(items[i]);
  }
  return text + "}";
}

static int degreeInGraph(int v) {
  int total = 0;
  for (int w = 0; w < n; w++) {
    if (adjM[v][w]) total++;
  }
  return total;
}

static bool isMaximalClique(const vector<int> &clique) {
  for (int v = 0; v < n; v++) {
    bool inside = false;
    for (int u : clique) {
      if (u == v) inside = true;
    }
    if (inside) continue;

    bool joinedToAll = true;
    for (int u : clique) {
      if (!adjM[u][v]) {
        joinedToAll = false;
        break;
      }
    }
    if (joinedToAll) return false;
  }
  return true;
}

static void say(const string &line) {
  if (shown >= TRACE_LIMIT) return;
  shown++;
  cout << line << "\n";
}

static void recordBest(int depth) {
  if ((int)stackClique.size() <= bestSize) return;
  bestSize = (int)stackClique.size();
  bestClique = stackClique;
  say(string(depth * 2, ' ') + "  New best: " + listText(bestClique) +
      " (Size " + to_string(bestSize) + ")");
}

static void expand(const vector<int> &cand, int depth) {
  nodes++;
  if (depth > deepest) deepest = depth;
  string pad(depth * 2, ' ');

  say(pad + "Node " + to_string(nodes) + ": R = " + listText(stackClique) +
      " (Size " + to_string(stackClique.size()) + "), candidates = " +
      listText(cand));
  recordBest(depth);

  if (cand.empty()) {
    leaves++;
    bool maximal = isMaximalClique(stackClique);
    if (maximal) maximalLeaves++;
    if (maximal) {
      say(pad + "  No later candidate can join, and no vertex outside R is");
      say(pad + "  joined to all of it, so R = " + listText(stackClique) +
          " is maximal.");
    } else {
      say(pad + "  No later candidate can join, but an earlier vertex is");
      say(pad + "  joined to all of R, so R = " + listText(stackClique) +
          " is not maximal.");
    }
    return;
  }

  for (int i = 0; i < (int)cand.size(); i++) {
    int reachable = (int)stackClique.size() + (int)cand.size() - i;
    if (reachable <= bestSize) {
      prunedByBound++;
      say(pad + "  Bound: " + to_string(stackClique.size()) + " + " +
          to_string((int)cand.size() - i) + " = " + to_string(reachable) +
          ", which cannot beat " + to_string(bestSize) + ", so prune the "
          "rest.");
      return;
    }

    int v = cand[i];
    vector<int> next;
    for (int j = i + 1; j < (int)cand.size(); j++) {
      if (adjM[v][cand[j]]) next.push_back(cand[j]);
    }

    say(pad + "  Take V" + to_string(v) + ": " + to_string(next.size()) +
        " of the later candidates are joined to it.");
    stackClique.push_back(v);
    expand(next, depth + 1);
    stackClique.pop_back();
  }
}

static void computeOrder() {
  for (int v = 0; v < n; v++) degreeOf[v] = degreeInGraph(v);
  for (int v = 0; v < n; v++) order[v] = v;

  for (int i = 1; i < n; i++) {
    int key = order[i];
    int j = i - 1;
    while (j >= 0 && degreeOf[order[j]] < degreeOf[key]) {
      order[j + 1] = order[j];
      j--;
    }
    order[j + 1] = key;
  }
}

static void showOrder() {
  cout << "\n--- Vertex order ---\n";
  cout << "Busiest first, and a tie keeps the smaller index first:\n";
  cout << "  Position:";
  for (int i = 0; i < n; i++) cout << setw(4) << i;
  cout << "\n";
  cout << "  Vertex  :";
  for (int i = 0; i < n; i++) cout << setw(4) << order[i];
  cout << "\n";
  cout << "  Degree  :";
  for (int i = 0; i < n; i++) cout << setw(4) << degreeOf[order[i]];
  cout << "\n";
  cout << "A high degree means many possible partners, so cliques tend to appear\n";
  cout << "early when the busiest vertices come first, and a good answer early is\n";
  cout << "what makes the bound bite.\n";
}

static void runBranchBound() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }

  computeOrder();

  bestSize = 0;
  bestClique.clear();
  nodes = 0;
  leaves = 0;
  maximalLeaves = 0;
  prunedByBound = 0;
  deepest = 0;
  shown = 0;
  stackClique.clear();

  cout << "\n--- Branch and bound for the maximum clique ---\n";
  cout << "R is the clique being built and the candidates are the vertices that\n";
  cout << "can still join it. The bound is simple: Even if every remaining\n";
  cout << "candidate joined R, the clique would only reach a certain size, and if\n";
  cout << "that size does not beat the best one found, the whole branch is dead.\n";

  showOrder();
  cout << "\n";

  vector<int> all;
  for (int i = 0; i < n; i++) all.push_back(order[i]);
  expand(all, 0);
  searchDone = true;

  if (shown >= TRACE_LIMIT) {
    cout << "\n(Trace stops after " << TRACE_LIMIT
         << " lines. The search itself ran to the end.)\n";
  }

  cout << "\n--- Result ---\n";
  cout << "Maximum clique: " << listText(bestClique) << " (Size " << bestSize
       << ")\n";
  cout << "Nodes visited: " << nodes << ", deepest node at depth " << deepest
       << ".\n";
  cout << "Leaves reached: " << plural(leaves, "branch end", "branch ends")
       << ", " << maximalLeaves << " of them maximal.\n";
  cout << "Branches cut by the bound: " << prunedByBound << ".\n";
  cout << "\nThe cut is why the walk reaches only some of the graph's maximal\n";
  cout << "cliques: A branch that cannot win is dropped before its leaves are\n";
  cout << "reached, so the walk shows some maximal cliques and skips the rest.\n";

  if (bestSize == 1) {
    cout << "\nSize 1 means no two vertices are joined, so every vertex is a\n";
    cout << "maximum clique on its own.\n";
  } else if (bestSize == n) {
    cout << "\nThe clique holds every vertex, so the graph is complete and the\n";
    cout << "search ends almost at once.\n";
  } else {
    cout << "\nEvery maximal clique has at most " << bestSize
         << " vertices, and the bound used\n";
    cout << "that fact against the branches that could not get there.\n";
  }
  cout << "Without the bound the same walk visits every clique in the graph, and\n";
  cout << "a complete graph on n vertices has 2 to the n of them.\n";
}

static void runBruteForce() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }
  if (n > 22) {
    cout << "Too many vertices for the subset loop.\n";
    return;
  }

  long long total = 1LL << n;
  bruteBest = 0;
  bruteCount = 0;
  bruteMask = 0;
  bruteCliques = 0;

  cout << "\n--- Brute force over all subsets ---\n";
  cout << "Every non-empty subset is tested once: " << total - 1
       << " subsets for n = " << n << ".\n";
  cout << "A subset is a clique when every pair inside it is joined.\n";

  for (long long mask = 1; mask < total; mask++) {
    int size = 0;
    for (int v = 0; v < n; v++) {
      if (mask & (1LL << v)) size++;
    }

    bool isClique = true;
    for (int i = 0; i < n && isClique; i++) {
      if (!(mask & (1LL << i))) continue;
      for (int j = i + 1; j < n; j++) {
        if (!(mask & (1LL << j))) continue;
        if (!adjM[i][j]) {
          isClique = false;
          break;
        }
      }
    }
    if (!isClique) continue;

    bruteCliques++;
    if (size > bruteBest) {
      bruteBest = size;
      bruteCount = 1;
      bruteMask = (int)mask;
    } else if (size == bruteBest) {
      bruteCount++;
    }
  }

  bruteDone = true;

  cout << "\n--- Result ---\n";
  cout << "Cliques found: " << bruteCliques << " out of " << total - 1
       << " subsets.\n";
  cout << "Largest: Size " << bruteBest << ", held by "
       << plural(bruteCount, "subset", "subsets") << ".\n";
  if (bruteBest > 0) {
    cout << "One of them: ";
    vector<int> members;
    for (int v = 0; v < n; v++) {
      if (bruteMask & (1 << v)) members.push_back(v);
    }
    cout << listText(members) << "\n";
  }
  cout << "\nThe loop tests " << total - 1
       << " subsets no matter how the graph\n";
  cout << "looks, so its work doubles each time a vertex is added.\n";
}

static void compareResults() {
  if (!searchDone) {
    cout << "Run the branch and bound first, so there is something to compare.\n";
    return;
  }
  if (!bruteDone) {
    cout << "Run the brute force first, so there is something to compare.\n";
    return;
  }

  cout << "\n--- Comparison ---\n";
  cout << "Branch and bound: Size " << bestSize << " " << listText(bestClique)
       << "\n";
  cout << "Brute force     : Size " << bruteBest << ", held by "
       << bruteCount << " of the subsets\n";

  if (bestSize != bruteBest) {
    cout << "\nThe two methods disagree, which means one of them is wrong.\n";
    return;
  }
  cout << "\nBoth agree, so the bound threw away only branches that could not\n";
  cout << "win.\n";
  cout << "Work done: " << nodes << " nodes and " << prunedByBound
       << " cuts, against " << ((1LL << n) - 1) << " subsets.\n";

  if (bruteCount > 1) {
    cout << "\nThere are " << bruteCount << " maximum cliques, so the answer is\n";
    cout << "not unique. The search stopped at the first one it found, which is\n";
    cout << "enough, because only the size matters.\n";
  } else {
    cout << "\nThe maximum clique is unique here.\n";
  }
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
    for (int j = 0; j < n; j++) cout << setw(4) << (adjM[i][j] ? 1 : 0);
    cout << "\n";
  }
  cout << "The table is symmetric, and the diagonal is 0 because a self-loop is\n";
  cout << "rejected: A clique needs distinct vertices.\n";
}

static void showAdjacency() {
  cout << "\n--- Adjacency list ---\n";
  if (n == 0) {
    cout << "(Empty)\n";
    return;
  }
  for (int i = 0; i < n; i++) {
    cout << setw(3) << i << ":";
    if (adj[i].empty()) cout << " (None)";
    else
      for (int v : adj[i]) cout << " " << v;
    cout << "   (Degree " << degreeInGraph(i) << ")\n";
  }
  cout << "Every line lists one vertex and the vertices joined to it.\n";
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
  cout << "Undirected edges only, vertices 0.." << (n - 1) << ".\n";

  while (true) {
    int limit = n * (n - 1) / 2;
    cout << "\nEdge " << (edgeCount + 1) << " (Limit " << limit
         << ", -1 to stop)\n";
    if (edgeCount == limit) {
      cout << "The graph is complete, so every pair is already joined.\n";
      break;
    }
    int u = readInt("  From vertex (0.." + to_string(n - 1) + ", -1 to stop): ");
    if (u == -1) break;
    int v = readInt("  To vertex (0.." + to_string(n - 1) + ", -1 to stop): ");
    if (v == -1) break;
    if (u < 0 || u >= n || v < 0 || v >= n) {
      cout << "  Rejected: Vertices must be in 0.." << (n - 1) << ".\n";
      continue;
    }
    if (u == v) {
      cout << "  Rejected: A vertex cannot be joined to itself in a clique.\n";
      continue;
    }
    if (adjM[u][v]) {
      cout << "  Rejected: " << edgeLabel(u, v) << " already exists.\n";
      continue;
    }
    addEdge(u, v);
    cout << "  Added E" << (edgeCount - 1) << " " << edgeLabel(u, v) << "\n";
  }

  cout << "\nBuild complete: " << n
       << (n == 1 ? " vertex, " : " vertices, ") << plural(edgeCount, "edge", "edges")
       << ".\n";
}

static void loadMainDemo() {
  resetGraph();
  n = 8;
  cout << "Mixed demo: 8 vertices, 12 edges.\n";
  cout << "  A K4 on {0, 1, 2, 3}, two triangles sharing the edge 4 - 5, and a\n";
  cout << "  loose edge 6 - 7. The two triangles share V4 and V5 but not V6, so\n";
  cout << "  nothing grows past 4.\n\n";

  addEdge(0, 1);
  addEdge(0, 2);
  addEdge(0, 3);
  addEdge(1, 2);
  addEdge(1, 3);
  addEdge(2, 3);
  addEdge(3, 4);
  addEdge(3, 5);
  addEdge(4, 5);
  addEdge(4, 6);
  addEdge(5, 6);
  addEdge(6, 7);

  cout << "Demo loaded: " << n << " vertices, " << edgeCount << " edges.\n";
}

static void loadCompleteDemo() {
  resetGraph();
  n = 6;
  cout << "Complete demo: 6 vertices, 15 edges.\n";
  cout << "  Every pair is joined, so the answer is the whole vertex set and\n";
  cout << "  the bound kills every other branch at once.\n\n";

  for (int u = 0; u < n; u++) {
    for (int v = u + 1; v < n; v++) addEdge(u, v);
  }

  cout << "Demo loaded: " << n << " vertices, " << edgeCount << " edges.\n";
}

static void loadCycleDemo() {
  resetGraph();
  n = 8;
  cout << "Cycle demo: 8 vertices, 8 edges, the ring 0 - 1 - ... - 7 - 0.\n";
  cout << "  No triangle exists, so every edge is a maximum clique of size 2 and\n";
  cout << "  there are eight of them. The size alone hides that tie.\n\n";

  for (int v = 0; v < n; v++) addEdge(v, (v + 1) % n);

  cout << "Demo loaded: " << n << " vertices, " << edgeCount << " edges.\n";
}

static void addEdgeInteractive() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }
  if (edgeCount == n * (n - 1) / 2) {
    cout << "The graph is already complete, so every pair is joined.\n";
    return;
  }

  cout << "\nAdd edge (V = " << n << ", so vertices 0.." << (n - 1) << ")\n";
  while (true) {
    int u = readInt("  From vertex (0.." + to_string(n - 1) + ", -1 to cancel): ");
    if (u == -1) {
      cout << "  Cancelled.\n";
      return;
    }
    int v = readInt("  To vertex (0.." + to_string(n - 1) + ", -1 to cancel): ");
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
      cout << "  Rejected: A vertex cannot be joined to itself in a clique.\n";
      cout << "  Please re-enter this edge.\n";
      continue;
    }
    if (adjM[u][v]) {
      cout << "  Rejected: " << edgeLabel(u, v) << " already exists.\n";
      cout << "  Please re-enter this edge.\n";
      continue;
    }

    addEdge(u, v);
    bruteDone = false;
    searchDone = false;
    cout << "  Added E" << (edgeCount - 1) << " " << edgeLabel(u, v) << "\n";
    cout << "    matrix[" << u << "][" << v << "] = 1 and matrix[" << v << "]["
         << u << "] = 1\n";
    cout << "    adj[" << u << "] and adj[" << v << "] each gained a neighbour.\n";
    cout << "  E = " << plural(edgeCount, "edge", "edges") << " now.\n";
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

static void removeEdgeInteractive() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }

  cout << "\nRemove an edge (V = " << n << ", so vertices 0.." << (n - 1)
       << ")\n";
  while (true) {
    int u = readInt("  From vertex (0.." + to_string(n - 1) + ", -1 to cancel): ");
    if (u == -1) {
      cout << "  Cancelled.\n";
      return;
    }
    int v = readInt("  To vertex (0.." + to_string(n - 1) + ", -1 to cancel): ");
    if (v == -1) {
      cout << "  Cancelled.\n";
      return;
    }
    if (u < 0 || u >= n || v < 0 || v >= n) {
      cout << "  Rejected: Vertices must be in 0.." << (n - 1) << ".\n";
      cout << "  Please re-enter this edge.\n";
      continue;
    }
    if (!adjM[u][v]) {
      cout << "  Rejected: " << edgeLabel(u, v) << " is not in the graph.\n";
      cout << "  Please re-enter this edge.\n";
      continue;
    }

    adjM[u][v] = false;
    adjM[v][u] = false;
    removeNeighbour(u, v);
    removeNeighbour(v, u);
    edgeCount--;
    bruteDone = false;
    searchDone = false;

    cout << "  Removed " << edgeLabel(u, v) << "\n";
    cout << "    matrix[" << u << "][" << v << "] = 0 and matrix[" << v << "]["
         << u << "] = 0\n";
    cout << "    adj[" << u << "] and adj[" << v << "] each lost a neighbour.\n";
    cout << "  E = " << plural(edgeCount, "edge", "edges") << " now.\n";
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
    adjM[fresh][i] = false;
    adjM[i][fresh] = false;
  }
  adj[fresh].clear();
  n++;
  bruteDone = false;
  searchDone = false;

  cout << "\nAdded V" << fresh << " with no edges.\n";
  cout << "  V = " << n << ", E = " << edgeCount << ".\n";
  cout << "  A vertex with no edge is a maximal clique of size 1 by itself,\n";
  cout << "  though the maximum clique may be larger.\n";
}

static void removeVertexInteractive() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }

  int v;
  while (true) {
    v = readInt("  Vertex to remove (0.." + to_string(n - 1) + ", -1 to cancel): ");
    if (v == -1) {
      cout << "  Cancelled.\n";
      return;
    }
    if (v >= 0 && v < n) break;
    cout << "  Rejected: Vertex must be in 0.." << (n - 1) << ".\n";
  }

  vector<int> keptFrom;
  vector<int> keptTo;
  for (int u = 0; u < n; u++) {
    if (u == v) continue;
    for (int w = u + 1; w < n; w++) {
      if (w == v) continue;
      if (!adjM[u][w]) continue;
      keptFrom.push_back(u < v ? u : u - 1);
      keptTo.push_back(w < v ? w : w - 1);
    }
  }

  int before = edgeCount;
  int newN = n - 1;
  resetGraph();
  n = newN;

  for (int i = 0; i < (int)keptFrom.size(); i++) {
    addEdge(keptFrom[i], keptTo[i]);
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
    for (int w = 0; w < n; w++) adjM[u][w] = false;
  }
  edgeCount = 0;
  bruteDone = false;
  searchDone = false;

  cout << "\nRemoved all " << plural(before, "edge", "edges") << ".\n";
  cout << "  V = " << n << ", E = 0. The vertices are still there.\n";
  cout << "  With no edges the maximum clique is any single vertex.\n";
}

static void operationsMenu() {
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
    cout << "\n=== G43: Maximum Clique (Branch and Bound) ===\n";
    cout << "Graph: Undirected | V = " << n << ", E = " << edgeCount << "\n";
    cout << "1. Build an undirected graph\n";
    cout << "2. Load the mixed demo (8 vertices)\n";
    cout << "3. Load the complete demo (6 vertices)\n";
    cout << "4. Load the cycle demo (8 vertices)\n";
    cout << "5. Graph operations\n";
    cout << "6. Find the maximum clique by branch and bound\n";
    cout << "7. Find it by brute force, then compare\n";
    cout << "8. Show the adjacency matrix\n";
    cout << "9. Show the adjacency list\n";
    cout << "10. Exit\n";

    int choice = readInt("Select: ");

    if (choice == 1) {
      buildGraph();
    } else if (choice == 2) {
      loadMainDemo();
    } else if (choice == 3) {
      loadCompleteDemo();
    } else if (choice == 4) {
      loadCycleDemo();
    } else if (choice == 5) {
      operationsMenu();
    } else if (choice == 6) {
      runBranchBound();
    } else if (choice == 7) {
      runBruteForce();
      compareResults();
    } else if (choice == 8) {
      showMatrix();
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
