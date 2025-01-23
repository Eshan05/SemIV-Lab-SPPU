// 21. Minimum spanning forest: When the graph comes in pieces, no tree can
// cover every vertex, so the best answer is a minimum spanning tree inside
// each piece. Prim is run once per piece and Kruskal once over every edge,
// and the two must agree. Demo: Eight vertices in four pieces, one of them a
// lone vertex, give four edges and a total of 16.

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

static bool cheaperEdge(int a, int b) {
  if (wEdge[a] != wEdge[b]) return wEdge[a] < wEdge[b];
  return a < b;
}

static int idOf(int u, int v) {
  for (int id = 0; id < edgeCount; id++) {
    if ((aSide[id] == u && bSide[id] == v) ||
        (aSide[id] == v && bSide[id] == u)) {
      return id;
    }
  }
  return -1;
}

static void findPieces(vector<vector<int>> &pieces) {
  vector<bool> seen(MAXV, false);
  pieces.clear();
  for (int s = 0; s < n; s++) {
    if (seen[s]) continue;
    vector<int> one;
    vector<int> stack;
    stack.push_back(s);
    seen[s] = true;
    while (!stack.empty()) {
      int u = stack.back();
      stack.pop_back();
      one.push_back(u);
      for (int i = 0; i < (int)adj[u].size(); i++) {
        int v = adj[u][i].first;
        if (seen[v]) continue;
        seen[v] = true;
        stack.push_back(v);
      }
    }
    sort(one.begin(), one.end());
    pieces.push_back(one);
  }
}

static void printPiece(const vector<int> &comp) {
  cout << "{";
  for (int i = 0; i < (int)comp.size(); i++) {
    cout << (i == 0 ? "" : " ") << comp[i];
  }
  cout << "}";
}

static void primOnPiece(const vector<int> &comp, vector<int> &ids,
                        int &total) {
  bool inTree[MAXV];
  int key[MAXV];
  int par[MAXV];
  for (int i = 0; i < MAXV; i++) {
    inTree[i] = false;
    key[i] = -1;
    par[i] = -1;
  }

  int start = comp[0];
  key[start] = 0;
  total = 0;

  for (int step = 0; step < (int)comp.size(); step++) {
    int best = -1;
    for (int i = 0; i < (int)comp.size(); i++) {
      int v = comp[i];
      if (inTree[v] || key[v] < 0) continue;
      if (best == -1 || key[v] < key[best]) best = v;
    }
    if (best == -1) break;

    inTree[best] = true;
    if (par[best] != -1) {
      ids.push_back(idOf(par[best], best));
      total += key[best];
      cout << "  Take V" << best << " at " << key[best] << " through V"
           << par[best] << ".\n";
    }

    for (int i = 0; i < (int)adj[best].size(); i++) {
      int v = adj[best][i].first;
      int w = adj[best][i].second;
      if (inTree[v]) continue;
      if (key[v] == -1 || w < key[v]) {
        key[v] = w;
        par[v] = best;
      }
    }
  }
}

static void loadForestDemo() {
  resetGraph();
  n = 8;
  cout << "Forest demo: 8 vertices in 4 pieces, and one piece is a lone vertex.\n";
  cout << "  0 - 1 (2), 1 - 2 (3), 0 - 2 (9), 3 - 4 (7), 5 - 6 (4).\n";
  cout << "  The pieces are {0 1 2}, {3 4}, {5 6} and {7}, so four of the five\n";
  cout << "  edges are kept.\n\n";

  addEdge(0, 1, 2);
  addEdge(1, 2, 3);
  addEdge(0, 2, 9);
  addEdge(3, 4, 7);
  addEdge(5, 6, 4);

  cout << "Demo loaded: " << n << " vertices, " << edgeCount << " edges.\n";
}

static void loadConnectedDemo() {
  resetGraph();
  n = 7;
  cout << "Connected demo: 7 vertices, 11 edges, the graph from the MST files.\n";
  cout << "  0 - 1 (7), 0 - 3 (5), 1 - 2 (8), 1 - 3 (9), 1 - 4 (7),\n";
  cout << "  2 - 4 (5), 3 - 4 (15), 3 - 5 (6), 4 - 5 (8), 4 - 6 (9),\n";
  cout << "  5 - 6 (11).\n";
  cout << "  It is one piece, so the forest is a full spanning tree of 39.\n\n";

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

static void runForest() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }
  if (edgeCount == 0) {
    cout << "\nNo edges, so every vertex is its own piece and the forest is\n";
    cout << "empty. That is " << n << (n == 1 ? " piece" : " pieces")
         << " and no edges at all.\n";
    return;
  }

  vector<vector<int>> pieces;
  findPieces(pieces);

  cout << "\n--- Step 1: Find the pieces ---\n";
  for (int i = 0; i < (int)pieces.size(); i++) {
    cout << "  Piece " << (i + 1) << ": ";
    printPiece(pieces[i]);
    cout << "\n";
  }
  cout << pieces.size() << (pieces.size() == 1 ? " piece, " : " pieces, ");
  if (pieces.size() == 1) {
    cout << "so the forest is a full spanning tree.\n";
  } else {
    cout << "so no spanning tree can cover every vertex.\n";
    cout << "The best that can be done is a minimum spanning tree inside each\n";
    cout << "piece, and the result is called a minimum spanning forest.\n";
  }

  cout << "\n--- Step 2: Prim, once per piece ---\n";
  vector<int> primIds;
  int primTotal = 0;
  for (int i = 0; i < (int)pieces.size(); i++) {
    cout << "\nPrim on piece " << (i + 1) << ": ";
    printPiece(pieces[i]);
    cout << "\n";
    if ((int)pieces[i].size() == 1) {
      cout << "  One vertex and nothing to join, so this piece adds no edge.\n";
      continue;
    }
    cout << "  Start at V" << pieces[i][0] << " with total weight 0.\n";

    vector<int> ids;
    int pieceTotal = 0;
    primOnPiece(pieces[i], ids, pieceTotal);
    for (int k = 0; k < (int)ids.size(); k++) primIds.push_back(ids[k]);
    primTotal += pieceTotal;

    cout << "  Piece " << (i + 1) << ": " << ids.size()
         << (ids.size() == 1 ? " edge, total " : " edges, total ")
         << pieceTotal << ".\n";
  }
  cout << "\nPrim over every piece: " << primIds.size() << " edges, total "
       << primTotal << ".\n";

  cout << "\n--- Step 3: Kruskal over every edge ---\n";
  vector<int> order;
  for (int id = 0; id < edgeCount; id++) order.push_back(id);
  sort(order.begin(), order.end(), cheaperEdge);

  resetDsu();
  vector<int> kruskalIds;
  int kruskalTotal = 0;
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
    kruskalIds.push_back(id);
    kruskalTotal += wEdge[id];
    cout << "  Keep E" << id << " (" << aSide[id] << " - " << bSide[id]
         << ", weight " << wEdge[id] << ").\n";
  }
  cout << "Kruskal over every edge: " << kruskalIds.size() << " edges, total "
       << kruskalTotal << ".\n";

  cout << "\n--- Step 4: Do the two agree? ---\n";
  cout << "Prim: " << primIds.size() << " edges, total " << primTotal << ".\n";
  cout << "Kruskal: " << kruskalIds.size() << " edges, total " << kruskalTotal
       << ".\n";

  vector<int> a = primIds;
  vector<int> b = kruskalIds;
  sort(a.begin(), a.end());
  sort(b.begin(), b.end());

  if (primTotal != kruskalTotal) {
    cout << "The totals differ, which a correct forest can never do.\n";
  } else if (a == b) {
    cout << "Same edges, same weight: The two constructions agree exactly.\n";
  } else {
    cout << "Same weight but not the same edges, which happens when weights tie.\n";
    cout << "Each run is still a minimum spanning forest.\n";
  }

  cout << "\nA forest with " << pieces.size()
       << (pieces.size() == 1 ? " piece has " : " pieces has ") << n << " - "
       << pieces.size() << " = " << n - (int)pieces.size()
       << " edges, one fewer\n";
  cout << "than vertices in every piece. That is what both runs produced.\n";

  cout << "\nPrim's argument works one piece at a time: Inside a piece the\n";
  cout << "lightest edge across the cut is safe, and no edge leaves the piece,\n";
  cout << "so nothing outside it can improve that piece. Kruskal needs no change\n";
  cout << "at all: The disjoint sets stop each piece when it is complete, so the\n";
  cout << "leftover edges are exactly the ones that would close a cycle.\n";
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
  cout << "Kruskal reads every one of these, while Prim only ever looks at\n";
  cout << "the vertices inside one piece.\n";
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
  cout << "  A lone vertex is a piece by itself, so the forest just skips it.\n";
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
  cout << "  With no edges, every vertex is its own piece.\n";
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
    cout << "\n=== G31: The Minimum Spanning Forest ===\n";
    cout << "Graph: Undirected, weighted | V = " << n << ", E = " << edgeCount
         << "\n";
    cout << "1. Build a graph that may come in pieces\n";
    cout << "2. Load forest demo (8 vertices, 4 pieces)\n";
    cout << "3. Load connected demo (7 vertices)\n";
    cout << "4. Graph operations\n";
    cout << "5. Run Prim per piece and Kruskal over everything\n";
    cout << "6. Show the weight matrix\n";
    cout << "7. Show the edge list\n";
    cout << "8. Exit\n";

    int choice = readInt("Select: ");

    if (choice == 1) {
      buildGraph();
    } else if (choice == 2) {
      loadForestDemo();
    } else if (choice == 3) {
      loadConnectedDemo();
    } else if (choice == 4) {
      operationsMenu();
    } else if (choice == 5) {
      runForest();
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
