// 19a. Kruskal, sorted edge list: Take the lightest edge that joins two
// different sets, and skip the rest, so the work is one sort and one find per
// edge.
// Demo: The seven-vertex graph from the Prim files gives the same six edges in
// weight order, and the pass stops early once the tree is complete.

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
const int MAX_PATHS = 8;
const int INF = 1000000;

static int n = 0;
static int edgeCount = 0;
static int wG[MAXV][MAXV];
static int aSide[MAXE];
static int bSide[MAXE];
static int wEdge[MAXE];
static vector<pair<int, int>> adj[MAXV];

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
  cout << "Demo: 7 vertices, 11 edges.\n";
  cout << "  0 - 1 (7), 0 - 3 (5), 1 - 2 (8), 1 - 3 (9), 1 - 4 (7),\n";
  cout << "  2 - 4 (5), 3 - 4 (15), 3 - 5 (6), 4 - 5 (8), 4 - 6 (9),\n";
  cout << "  5 - 6 (11).\n";
  cout << "  Light edges come first here, so the tree fills up in six keeps\n";
  cout << "  and the two heaviest edges are never needed.\n\n";

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
  cout << "  Four edges of the same weight, so any three of them span the\n";
  cout << "  square. Kruskal takes the first three edges it meets, which is a\n";
  cout << "  different tree from Prim's, and the same total weight.\n\n";

  addEdge(0, 1, 1);
  addEdge(1, 2, 1);
  addEdge(2, 3, 1);
  addEdge(3, 0, 1);

  cout << "Demo loaded: " << n << " vertices, " << edgeCount << " edges.\n";
}

static int parentOf[MAXV];
static int sizeOf[MAXV];
static int groupCount = 0;

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

static bool lighterFirst(int a, int b) {
  if (wEdge[a] != wEdge[b]) return wEdge[a] < wEdge[b];
  return a < b;
}

static void printSortedGroups(const vector<int> &order) {
  int i = 0;
  while (i < (int)order.size()) {
    int w = wEdge[order[i]];
    cout << setw(4) << w << ":";
    int j = i;
    while (j < (int)order.size() && wEdge[order[j]] == w) {
      cout << " E" << order[j];
      j++;
    }
    cout << "\n";
    i = j;
  }
}

static void printCyclePath(const vector<int> &keptIds, int from, int to,
                           int w) {
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
    if (u == to) break;
    for (int i = 0; i < (int)fadj[u].size(); i++) {
      int v = fadj[u][i];
      if (seen[v]) continue;
      seen[v] = true;
      prev[v] = u;
      stack.push_back(v);
    }
  }

  vector<int> path;
  int cur = to;
  while (cur != from && cur != -1) {
    path.push_back(cur);
    cur = prev[cur];
  }
  path.push_back(from);

  cout << "   Kept path";
  for (int i = (int)path.size() - 1; i >= 0; i--) {
    cout << (i == (int)path.size() - 1 ? " " : " - ") << path[i];
  }
  cout << ", and this edge closes it into a cycle.\n";

  int heaviest = 0;
  for (int i = (int)path.size() - 1; i > 0; i--) {
    int a = path[i];
    int b = path[i - 1];
    if (wG[a][b] > heaviest) heaviest = wG[a][b];
  }
  cout << "   Those edges came earlier in the order, so none is heavier than "
       << w << ".\n";
  if (heaviest == w) {
    cout << "   One of them ties at " << w << ", so some minimum tree uses this\n";
    cout << "   edge and some other minimum tree does not.\n";
  }
}
static void runKruskal() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }
  if (edgeCount == 0) {
    cout << "\nNo edges, so there is nothing to keep.\n";
    return;
  }

  vector<int> order;
  for (int id = 0; id < edgeCount; id++) order.push_back(id);
  sort(order.begin(), order.end(), lighterFirst);

  cout << "\n--- Kruskal: Sort the edges, then one pass with disjoint sets ---\n";
  cout << "Sorted " << edgeCount << " edges by weight. Equal weights keep "
       << "their input order.\n";
  printSortedGroups(order);
  cout << "A group is every edge of one weight, and the pass below takes them\n";
  cout << "in that order.\n";

  resetDsu();
  int kept = 0;
  int rejected = 0;
  int pathsShown = 0;
  int examined = 0;
  int total = 0;
  vector<int> keptIds;

  for (int pos = 0; pos < (int)order.size(); pos++) {
    if (kept == n - 1) break;

    int id = order[pos];
    examined++;
    int u = aSide[id];
    int v = bSide[id];
    int ru = rootOf(u);
    int rv = rootOf(v);

    cout << "\n" << (pos + 1) << ". " << edgeLabelOf(id) << "\n";

    if (ru == rv) {
      rejected++;
      cout << "   Both ends answer to root " << ru
           << " already, so a kept path joins them.\n";
      if (pathsShown < MAX_PATHS) {
        printCyclePath(keptIds, u, v, wEdge[id]);
        pathsShown++;
      } else if (pathsShown == MAX_PATHS) {
        pathsShown++;
        cout << "   Later rejections close cycles the same way, so only the\n";
        cout << "   first " << MAX_PATHS << " paths are printed.\n";
      }
      cout << "   Total stays " << total << ", edges still " << kept << " of "
           << (n - 1) << ".\n";
      continue;
    }

    int top = joinRoots(ru, rv);
    kept++;
    total += wEdge[id];
    keptIds.push_back(id);
    cout << "   Roots " << ru << " and " << rv << " differ, so keep it.\n";
    cout << "   parent[" << ((top == ru) ? rv : ru) << "] = " << top << ". Total "
         << total << ", edges " << kept << " of " << (n - 1) << ".\n";
  }

  cout << "\n--- Result ---\n";
  cout << "Kept edges, lightest first:\n";
  for (int i = 0; i < (int)keptIds.size(); i++) {
    int id = keptIds[i];
    cout << "  " << aSide[id] << " - " << bSide[id] << "   weight "
         << wEdge[id] << "\n";
  }
  cout << "Total weight: " << total << ".\n";

  int leftOver = edgeCount - examined;
  cout << "Examined " << examined << " of " << edgeCount
       << " edges: " << kept << " kept, " << rejected << " rejected";
  if (leftOver > 0) cout << ", " << leftOver << " left over";
  cout << ".\n";

  if (leftOver > 0) {
    cout << "The pass stopped as soon as " << (n - 1)
         << " edges were kept, because every\n";
    cout << "spanning tree has exactly V - 1 edges. The " << leftOver
         << " heaviest edges were\n";
    cout << "never looked at.\n";
  }

  if (groupCount == 1) {
    cout << "\nOne set now holds every vertex, so this is a minimum spanning\n";
    cout << "tree.\n";
  } else {
    cout << "\nThe graph falls into " << groupCount
         << " pieces, so no tree can join them all.\n";
    cout << "Kept " << kept << " edges for " << n
         << " vertices: That is a spanning forest, one\n";
    cout << "tree per piece.\n";
  }

  int levels = 0;
  int reach = 1;
  while (reach < edgeCount) {
    reach *= 2;
    levels++;
  }
  cout << "\nEach rejection above was a cycle: A kept path already joined the two\n";
  cout << "ends and nothing on it was heavier, which is the cycle property. The\n";
  cout << "keeps can be proved with the cut property instead, but that needs the\n";
  cout << "set of every edge left in the tree from this vertex, and the disjoint\n";
  cout << "sets never print one.\n";
  cout << "\nSorting " << edgeCount << " edges costs about " << edgeCount << " x "
       << levels << " = " << edgeCount * levels << " comparisons,\n";
  cout << "and the pass then costs one find per edge. Those finds stay near\n";
  cout << "constant time because union by size keeps both trees shallow.\n";
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
  cout << "This list is what Kruskal sorts, so the ids also show the input\n";
  cout << "order that breaks a tie between equal weights.\n";
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
  cout << "  A lone vertex has no edge to offer, so Prim can never reach it.\n";
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
  cout << "  With no edges, there is nothing for Prim to grow.\n";
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
    cout << "\n=== G26: Kruskal's Algorithm (Sorted Edge List) ===\n";
    cout << "Graph: Undirected, weighted | V = " << n << ", E = " << edgeCount
         << "\n";
    cout << "1. Build a weighted graph\n";
    cout << "2. Load demo graph (7 vertices)\n";
    cout << "3. Load tie demo (4 vertices)\n";
    cout << "4. Graph operations\n";
    cout << "5. Run Kruskal and show every decision\n";
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
      runKruskal();
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
