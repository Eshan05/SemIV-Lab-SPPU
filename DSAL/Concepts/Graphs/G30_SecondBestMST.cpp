// 21. Second-best MST: Run Kruskal once to get a minimum tree, then offer
// every skipped edge as a single swap. Adding one skipped edge closes one
// cycle, so the cheapest repair drops the heaviest edge on that cycle, and
// the cheapest repair over all skipped edges is the second-best tree.
// Demo: The seven-vertex graph from the MST files has a minimum of 39 and a
// second best of 40, reached by two different swaps.

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

static int n = 0;
static int edgeCount = 0;
static int wG[MAXV][MAXV];
static int aSide[MAXE];
static int bSide[MAXE];
static int wEdge[MAXE];
static vector<pair<int, int>> adj[MAXV];

static int parentOf[MAXV];
static int sizeOf[MAXV];
static int groupCount = 0;

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
    for (int j = 0; j < MAXV; j++) wG[i][j] = 0;
  }
  n = 0;
  edgeCount = 0;
}

static int addEdge(int u, int v, int w) {
  if (u > v) {
    int t = u;
    u = v;
    v = t;
  }
  int id = edgeCount;
  aSide[id] = u;
  bSide[id] = v;
  wEdge[id] = w;
  wG[u][v] = w;
  wG[v][u] = w;
  adj[u].push_back(make_pair(v, w));
  adj[v].push_back(make_pair(u, w));
  edgeCount++;
  return id;
}

static string edgeLabelOf(int id) {
  return "E" + to_string(id) + " (" + to_string(aSide[id]) + " - " +
         to_string(bSide[id]) + ", weight " + to_string(wEdge[id]) + ")";
}

static void dropEdgeId(int removed) {
  for (int id = removed; id < edgeCount - 1; id++) {
    aSide[id] = aSide[id + 1];
    bSide[id] = bSide[id + 1];
    wEdge[id] = wEdge[id + 1];
  }
  edgeCount--;
}

static void removeNeighbour(int u, int v) {
  for (int i = 0; i < (int)adj[u].size(); i++) {
    if (adj[u][i].first != v) continue;
    adj[u].erase(adj[u].begin() + i);
    return;
  }
}

static void resetDsu() {
  for (int i = 0; i < MAXV; i++) {
    parentOf[i] = i;
    sizeOf[i] = (i < n) ? 1 : 0;
  }
  groupCount = n;
}

static int rootOf(int x) {
  while (parentOf[x] != x) x = parentOf[x];
  return x;
}

static int joinRoots(int ru, int rv) {
  if (ru == rv) return -1;

  int top;
  int under;
  if (sizeOf[ru] > sizeOf[rv]) {
    top = ru;
    under = rv;
  } else if (sizeOf[rv] > sizeOf[ru]) {
    top = rv;
    under = ru;
  } else {
    top = (ru < rv) ? ru : rv;
    under = (top == ru) ? rv : ru;
  }

  parentOf[under] = top;
  sizeOf[top] += sizeOf[under];
  groupCount--;
  return top;
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
  cout << "Undirected weighted edges. Weights are whole numbers in 1..99.\n";
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
      cout << "  Rejected: A self-loop never helps a spanning tree.\n";
      e--;
      continue;
    }
    int w = readInt("  Weight (1..99): ");
    if (w < 1 || w > 99) {
      cout << "  Rejected: Weight must be in 1..99.\n";
      e--;
      continue;
    }
    if (wG[u][v] != 0) {
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
  n = 7;
  cout << "Demo: 7 vertices, 11 edges, the graph from the Prim and Kruskal files.\n";
  cout << "  0 - 1 (7), 0 - 3 (5), 1 - 2 (8), 1 - 3 (9), 1 - 4 (7),\n";
  cout << "  2 - 4 (5), 3 - 4 (15), 3 - 5 (6), 4 - 5 (8), 4 - 6 (9),\n";
  cout << "  5 - 6 (11).\n";
  cout << "  The minimum tree weighs 39. Every skipped edge offers one swap, and\n";
  cout << "  the cheapest of those swaps is the second-best tree.\n\n";

  addEdge(0, 3, 5);
  addEdge(0, 1, 7);
  addEdge(1, 2, 8);
  addEdge(1, 3, 9);
  addEdge(1, 4, 7);
  addEdge(2, 4, 5);
  addEdge(3, 4, 15);
  addEdge(3, 5, 6);
  addEdge(4, 5, 8);
  addEdge(4, 6, 9);
  addEdge(5, 6, 11);

  cout << "Demo loaded: " << n << " vertices, " << edgeCount << " edges.\n";
}

static void loadTieDemo() {
  resetGraph();
  n = 4;
  cout << "Tie demo: 4 vertices in a square, every edge weight 1.\n";
  cout << "  Any three of them form a minimum tree, so the fourth can swap in\n";
  cout << "  for the same weight and the cheapest swap costs nothing.\n\n";

  addEdge(0, 1, 1);
  addEdge(1, 2, 1);
  addEdge(2, 3, 1);
  addEdge(3, 0, 1);

  cout << "Demo loaded: " << n << " vertices, " << edgeCount << " edges.\n";
}

static bool cheaperEdge(int a, int b) {
  if (wEdge[a] != wEdge[b]) return wEdge[a] < wEdge[b];
  return a < b;
}

static bool findTreePath(const vector<int> &keptIds, int from, int to,
                         vector<int> &path) {
  vector<vector<int>> fadj(MAXV);
  for (int k = 0; k < (int)keptIds.size(); k++) {
    int id = keptIds[k];
    fadj[aSide[id]].push_back(bSide[id]);
    fadj[bSide[id]].push_back(aSide[id]);
  }

  vector<int> prev(MAXV, -1);
  vector<bool> seen(MAXV, false);
  vector<int> stack;
  stack.push_back(from);
  seen[from] = true;
  while (!stack.empty()) {
    int u = stack.back();
    stack.pop_back();
    for (int i = 0; i < (int)fadj[u].size(); i++) {
      int v = fadj[u][i];
      if (seen[v]) continue;
      seen[v] = true;
      prev[v] = u;
      stack.push_back(v);
    }
  }
  if (!seen[to]) return false;

  path.clear();
  int cur = to;
  while (cur != from) {
    path.push_back(cur);
    cur = prev[cur];
  }
  path.push_back(from);
  reverse(path.begin(), path.end());
  return true;
}

static void runSecondBest() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }
  if (n < 2) {
    cout << "\nA spanning tree needs at least two vertices, so there is nothing\n";
    cout << "to compare.\n";
    return;
  }
  if (edgeCount == 0) {
    cout << "\nNo edges, so there is no spanning tree and no second-best one.\n";
    return;
  }

  cout << "\nWe want the lightest spanning tree that is not a minimum one. Every\n";
  cout << "candidate is one swap away from a minimum tree, so the search runs\n";
  cout << "over single swaps: Add a skipped edge, and drop the heaviest edge on\n";
  cout << "the cycle it closes.\n";

  cout << "\n--- Step 1: Kruskal, to get one minimum tree ---\n";
  vector<int> order;
  for (int id = 0; id < edgeCount; id++) order.push_back(id);
  sort(order.begin(), order.end(), cheaperEdge);

  resetDsu();
  vector<int> keptIds;
  int total = 0;
  for (int i = 0; i < (int)order.size(); i++) {
    int id = order[i];
    int ru = rootOf(aSide[id]);
    int rv = rootOf(bSide[id]);
    if (ru == rv) {
      cout << "  Skip E" << id << " (" << aSide[id] << " - " << bSide[id]
           << ", weight " << wEdge[id] << "), both ends already joined.\n";
      continue;
    }
    joinRoots(ru, rv);
    keptIds.push_back(id);
    total += wEdge[id];
    cout << "  Keep E" << id << " (" << aSide[id] << " - " << bSide[id]
         << ", weight " << wEdge[id] << ").\n";
  }

  if ((int)keptIds.size() < n - 1) {
    cout << "\nThe tree stopped at " << keptIds.size()
         << (keptIds.size() == 1 ? " edge, so the graph is\n"
                                 : " edges, so the graph is\n");
    cout << "not connected. With no spanning tree at all, there is no second-best\n";
    cout << "one either.\n";
    return;
  }

  cout << "Minimum tree: " << keptIds.size() << " edges, total " << total << ".\n";

  vector<int> skipped;
  for (int id = 0; id < edgeCount; id++) {
    bool inside = false;
    for (int k = 0; k < (int)keptIds.size(); k++) {
      if (keptIds[k] == id) inside = true;
    }
    if (!inside) skipped.push_back(id);
  }

  if (skipped.empty()) {
    cout << "\nEvery edge is in the tree, so nothing can be swapped in. The graph\n";
    cout << "is itself a tree, so it has exactly one spanning tree and no\n";
    cout << "second-best one.\n";
    return;
  }

  cout << "\n--- Step 2: Offer every skipped edge as one swap ---\n";

  int bestTotal = -1;
  int bestId = -1;
  int bestFrom = -1;
  int bestTo = -1;
  int bestDropW = -1;
  vector<int> bestTies;

  for (int k = 0; k < (int)skipped.size(); k++) {
    int id = skipped[k];
    vector<int> path;
    findTreePath(keptIds, aSide[id], bSide[id], path);

    cout << "\nE" << id << " (" << aSide[id] << " - " << bSide[id] << ", weight "
         << wEdge[id] << ") is not in the tree.\n";
    cout << "  Tree path:";
    for (int i = 0; i < (int)path.size(); i++) {
      cout << (i == 0 ? " " : " - ") << path[i];
    }
    cout << "\n";

    int worstFrom = path[0];
    int worstTo = path[1];
    int worstW = wG[path[0]][path[1]];
    for (int i = 0; i + 1 < (int)path.size(); i++) {
      int a = path[i];
      int b = path[i + 1];
      if (wG[a][b] > worstW) {
        worstW = wG[a][b];
        worstFrom = a;
        worstTo = b;
      }
    }
    cout << "  Heaviest edge on it: " << worstFrom << " - " << worstTo << " at "
         << worstW << ".\n";

    int candidate = total - worstW + wEdge[id];
    cout << "  Swap it in: " << total << " - " << worstW << " + " << wEdge[id]
         << " = " << candidate << ".\n";

    if (bestTotal == -1 || candidate < bestTotal) {
      bestTotal = candidate;
      bestId = id;
      bestFrom = worstFrom;
      bestTo = worstTo;
      bestDropW = worstW;
      bestTies.clear();
      bestTies.push_back(id);
    } else if (candidate == bestTotal) {
      bestTies.push_back(id);
    }
  }

  cout << "\n--- Step 3: The cheapest swap ---\n";
  cout << "The best swap takes E" << bestId << " and reaches " << bestTotal
       << ".\n";
  if (bestTies.size() > 1) {
    cout << "It is not alone:";
    for (int i = 0; i < (int)bestTies.size(); i++) {
      if (i == 0) {
        cout << " E" << bestTies[i];
      } else if (i + 1 == (int)bestTies.size()) {
        cout << " and E" << bestTies[i];
      } else {
        cout << ", E" << bestTies[i];
      }
    }
    cout << (bestTies.size() == 2 ? " reach the same weight.\n"
                                  : " reach the same weight.\n");
  }

  int gap = bestTotal - total;
  if (gap == 0) {
    cout << "\nA swap costs nothing, so the second-best weight is " << total
         << " too.\n";
    cout << "Another tree ties the minimum, so the minimum spanning tree is not\n";
    cout << "unique.\n";
  } else {
    cout << "\nNothing cheaper is possible, so the second-best weight is "
         << bestTotal << ",\n";
    cout << "which is " << gap << " more than the minimum of " << total << ".\n";
    cout << "A positive gap means no other tree ties the minimum, so the minimum\n";
    cout << "spanning tree is unique.\n";
  }

  cout << "\nDrop " << bestFrom << " - " << bestTo << " at " << bestDropW
       << " and add " << aSide[bestId] << " - " << bSide[bestId] << " at "
       << wEdge[bestId] << ":\n";
  int sum = 0;
  for (int k = 0; k < (int)keptIds.size(); k++) {
    int id = keptIds[k];
    bool dropped = false;
    if ((aSide[id] == bestFrom && bSide[id] == bestTo) ||
        (aSide[id] == bestTo && bSide[id] == bestFrom)) {
      dropped = true;
    }
    if (dropped) continue;
    cout << "  " << aSide[id] << " - " << bSide[id] << "   weight " << wEdge[id]
         << "\n";
    sum += wEdge[id];
  }
  cout << "  " << aSide[bestId] << " - " << bSide[bestId] << "   weight "
       << wEdge[bestId] << "\n";
  sum += wEdge[bestId];
  cout << "Total weight: " << sum << ".\n";
  cout << "Edges: " << keptIds.size() << ", exactly V - 1 for a tree, so it is\n";
  cout << "still a spanning tree.\n";

  cout << "\nWhy one swap is enough: Take any spanning tree and add a skipped\n";
  cout << "edge. That closes one cycle, and the heaviest edge on the cycle is\n";
  cout << "never needed, so trading it out keeps the tree and never makes it\n";
  cout << "heavier. Repeating that walk always reaches a minimum tree, so the\n";
  cout << "lightest tree that is not the minimum is one trade away from one.\n";
}

static void showMatrix() {
  cout << "\n--- Weight Matrix ---\n";
  if (n == 0) {
    cout << "(Empty)\n";
    return;
  }
  cout << "     ";
  for (int j = 0; j < n; j++) cout << setw(4) << j;
  cout << "\n";
  for (int i = 0; i < n; i++) {
    cout << setw(4) << i << " ";
    for (int j = 0; j < n; j++) {
      if (wG[i][j] == 0) cout << setw(4) << "-";
      else cout << setw(4) << wG[i][j];
    }
    cout << "\n";
  }
  cout << "A dash means no edge. The matrix is symmetric, so each weight\n";
  cout << "appears twice.\n";
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
  cout << "The minimum tree comes from the first edges Kruskal keeps, and\n";
  cout << "every other edge here is one swap to try.\n";
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
      cout << "  Rejected: A self-loop never helps a spanning tree.\n";
      cout << "  Please re-enter this edge.\n";
      continue;
    }
    if (wG[u][v] != 0) {
      cout << "  Rejected: " << u << " - " << v << " already exists.\n";
      cout << "    Undirected edges are unordered: " << v << " - " << u
           << " is the same edge as " << u << " - " << v << ".\n";
      cout << "  Please re-enter this edge.\n";
      continue;
    }

    int w = readInt("  Weight (1..99): ");
    if (w < 1 || w > 99) {
      cout << "  Rejected: Weight must be in 1..99.\n";
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
    if (wG[u][v] == 0) {
      cout << "  Rejected: " << u << " - " << v << " is not in the graph.\n";
      cout << "  Please re-enter this edge.\n";
      continue;
    }

    int found = -1;
    for (int id = 0; id < edgeCount; id++) {
      if ((aSide[id] == u && bSide[id] == v) ||
          (aSide[id] == v && bSide[id] == u)) {
        found = id;
        break;
      }
    }

    wG[u][v] = 0;
    wG[v][u] = 0;
    removeNeighbour(u, v);
    removeNeighbour(v, u);

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
  }
  adj[fresh].clear();
  n++;

  cout << "\nAdded V" << fresh << " with no edges.\n";
  cout << "  V = " << n << ", E = " << edgeCount << ".\n";
  cout << "  A lone vertex is cut off, so no tree can span every vertex.\n";
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

  vector<int> keptA;
  vector<int> keptB;
  vector<int> keptW;
  for (int id = 0; id < edgeCount; id++) {
    if (aSide[id] == v || bSide[id] == v) continue;
    keptA.push_back(aSide[id] < v ? aSide[id] : aSide[id] - 1);
    keptB.push_back(bSide[id] < v ? bSide[id] : bSide[id] - 1);
    keptW.push_back(wEdge[id]);
  }

  int before = edgeCount;
  n--;

  for (int a = 0; a < MAXV; a++) {
    adj[a].clear();
    for (int b = 0; b < MAXV; b++) wG[a][b] = 0;
  }

  edgeCount = (int)keptA.size();
  for (int id = 0; id < edgeCount; id++) {
    aSide[id] = keptA[id];
    bSide[id] = keptB[id];
    wEdge[id] = keptW[id];
    wG[aSide[id]][bSide[id]] = wEdge[id];
    wG[bSide[id]][aSide[id]] = wEdge[id];
    adj[aSide[id]].push_back(make_pair(bSide[id], wEdge[id]));
    adj[bSide[id]].push_back(make_pair(aSide[id], wEdge[id]));
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
    for (int w = 0; w < n; w++) wG[u][w] = 0;
  }
  edgeCount = 0;

  cout << "\nRemoved all " << before
       << (before == 1 ? " edge.\n" : " edges.\n");
  cout << "  V = " << n << ", E = 0. The vertices are still there.\n";
  cout << "  With no edges, there is no spanning tree to compare.\n";
}

static void operationsMenu() {
  while (true) {
    cout << "\n--- Graph operations ---\n";
    cout << "Graph: Undirected, weighted | V = " << n << ", E = " << edgeCount
         << "\n";
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
    cout << "\n=== G30: The Second-Best Minimum Spanning Tree ===\n";
    cout << "Graph: Undirected, weighted | V = " << n << ", E = " << edgeCount
         << "\n";
    cout << "1. Build a weighted graph\n";
    cout << "2. Load demo graph (7 vertices)\n";
    cout << "3. Load tie demo (4 vertices)\n";
    cout << "4. Graph operations\n";
    cout << "5. Run the swap search\n";
    cout << "6. Show the weight matrix\n";
    cout << "7. Show the edge list\n";
    cout << "8. Exit\n";

    int choice = readInt("Select: ");

    if (choice == 1) {
      buildGraph();
    } else if (choice == 2) {
      loadDemo();
    } else if (choice == 3) {
      loadTieDemo();
    } else if (choice == 4) {
      operationsMenu();
    } else if (choice == 5) {
      runSecondBest();
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
