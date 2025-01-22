// 20. Boruvka: Every piece picks its own cheapest outgoing edge at the same
// time, so one round can add several edges and the number of pieces at least
// halves. Demo: The seven-vertex graph from the MST files finishes in two
// rounds with the same total, and a tree of eight needs the full three rounds.
// Picking one cheapest edge per piece is the cut property used on every cut
// at once, which is the one real difference from Prim.

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
static int bestFor[MAXV];

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

static string pieceLabel(int r) {
  string s = "{";
  for (int v = 0; v < n; v++) {
    if (rootOf(v) != r) continue;
    if (s.size() > 1) s += " ";
    s += to_string(v);
  }
  s += "}";
  return s;
}

static void printPieces() {
  cout << "  Pieces:";
  int col = 9;
  for (int r = 0; r < n; r++) {
    if (rootOf(r) != r) continue;
    string one = pieceLabel(r);
    if (col + 1 + (int)one.size() > 74) {
      cout << "\n   ";
      col = 3;
    } else {
      cout << " ";
      col += 1;
    }
    cout << one;
    col += (int)one.size();
  }
  cout << "\n";
}

static bool cheaperPick(int a, int b) {
  if (wEdge[a] != wEdge[b]) return wEdge[a] < wEdge[b];
  return a < b;
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
  cout << "  Round one pairs the vertices up, round two joins the two halves.\n\n";

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
  cout << "  Every piece has two edges of weight 1 to choose from, and the tie\n";
  cout << "  goes to the lower edge id, so one round adds three of them.\n\n";

  addEdge(0, 1, 1);
  addEdge(1, 2, 1);
  addEdge(2, 3, 1);
  addEdge(3, 0, 1);

  cout << "Demo loaded: " << n << " vertices, " << edgeCount << " edges.\n";
}

static void loadRoundsDemo() {
  resetGraph();
  n = 8;
  cout << "Rounds demo: 8 vertices, 7 edges, and it needs the full three rounds.\n";
  cout << "  0 - 1 (1), 2 - 3 (1), 4 - 5 (1), 6 - 7 (1), 1 - 2 (5),\n";
  cout << "  5 - 6 (5), 3 - 4 (9).\n";
  cout << "  Round one pairs up four neighbours, round two makes two groups of\n";
  cout << "  four, and round three joins them with the one heavy edge left.\n\n";

  addEdge(0, 1, 1);
  addEdge(2, 3, 1);
  addEdge(4, 5, 1);
  addEdge(6, 7, 1);
  addEdge(1, 2, 5);
  addEdge(5, 6, 5);
  addEdge(3, 4, 9);

  cout << "Demo loaded: " << n << " vertices, " << edgeCount << " edges.\n";
}

static void runBoruvka() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }
  if (edgeCount == 0) {
    cout << "\nNo edges, so every vertex is its own piece and nothing can be\n";
    cout << "joined. The forest is empty.\n";
    return;
  }

  resetDsu();

  cout << "\n--- Boruvka: Every piece takes its own cheapest edge ---\n";
  cout << "All pieces choose at the same time, so one round can add several\n";
  cout << "edges and the number of pieces at least halves every round.\n";

  vector<int> takenA;
  vector<int> takenB;
  vector<int> takenW;
  int total = 0;
  int round = 0;

  while (groupCount > 1) {
    for (int r = 0; r < MAXV; r++) bestFor[r] = -1;
    for (int id = 0; id < edgeCount; id++) {
      int ru = rootOf(aSide[id]);
      int rv = rootOf(bSide[id]);
      if (ru == rv) continue;
      if (bestFor[ru] == -1 || cheaperPick(id, bestFor[ru])) bestFor[ru] = id;
      if (bestFor[rv] == -1 || cheaperPick(id, bestFor[rv])) bestFor[rv] = id;
    }

    vector<int> choosers;
    int silent = 0;
    for (int r = 0; r < n; r++) {
      if (rootOf(r) != r) continue;
      if (bestFor[r] == -1) silent++;
      else choosers.push_back(r);
    }

    if (choosers.empty()) {
      cout << "\nNo piece has an edge leaving it, so nothing more can be joined.\n";
      break;
    }

    round++;
    cout << "\nRound " << round << "\n";
    if (round == 1) printPieces();

    cout << "  Each piece takes the lightest edge leaving it:\n";
    vector<int> picks;
    for (int i = 0; i < (int)choosers.size(); i++) {
      int r = choosers[i];
      cout << "    " << pieceLabel(r) << " takes " << edgeLabelOf(bestFor[r])
           << ".\n";
      bool seen = false;
      for (int j = 0; j < (int)picks.size(); j++) {
        if (picks[j] == bestFor[r]) seen = true;
      }
      if (!seen) picks.push_back(bestFor[r]);
    }

    if (silent > 0) {
      cout << "  " << silent << (silent == 1 ? " piece has" : " pieces have")
           << " no edge leaving it, so " << (silent == 1 ? "it stays" : "they stay")
           << (silent == 1 ? " as it is.\n" : " as they are.\n");
    }

    sort(picks.begin(), picks.end());
    int before = groupCount;
    vector<int> added;
    for (int i = 0; i < (int)picks.size(); i++) {
      int id = picks[i];
      int ru = rootOf(aSide[id]);
      int rv = rootOf(bSide[id]);
      if (ru == rv) continue;
      joinRoots(ru, rv);
      takenA.push_back(aSide[id]);
      takenB.push_back(bSide[id]);
      takenW.push_back(wEdge[id]);
      total += wEdge[id];
      added.push_back(id);
    }

    cout << "  An edge chosen from both ends is added once, so " << added.size();
    if (added.size() == 1) {
      cout << " edge goes in: E" << added[0] << ".\n";
    } else {
      cout << " edges go in:\n   ";
      for (int i = 0; i < (int)added.size(); i++) {
        if (i > 0) {
          cout << (i + 1 == (int)added.size() ? " and " : ", ");
        }
        cout << "E" << added[i];
      }
      cout << ".\n";
    }
    cout << "  Total weight " << total << " now.\n";
    printPieces();

    if (groupCount == before) break;
  }

  cout << "\n--- Result ---\n";
  cout << "Edges, in the order the rounds added them:\n";
  for (int i = 0; i < (int)takenA.size(); i++) {
    cout << "  " << takenA[i] << " - " << takenB[i] << "   weight " << takenW[i]
         << "\n";
  }
  cout << "Total weight: " << total << ".\n";
  cout << "Edges taken: " << takenA.size() << ". ";

  int needed = 0;
  int reach = 1;
  while (reach < n) {
    reach *= 2;
    needed++;
  }

  if (groupCount == 1) {
    cout << "A spanning tree of " << n << " vertices needs exactly " << n - 1
         << ".\n";
    cout << "That is what came out, so this is a minimum spanning tree.\n";
    cout << "Rounds: " << round
         << ". Every round at least halves the pieces, so " << n
         << " vertices\nneed at most " << needed << ".\n";
  } else {
    cout << "The graph stays in " << groupCount
         << " pieces, so no tree can join\n";
    cout << "them all. What came out is a minimum spanning forest.\n";
    cout << "Rounds: " << round << ". The run stopped because the pieces left had\n";
    cout << "no edge joining them.\n";
  }

  cout << "\nEach pick was the lightest edge leaving its own piece, and a piece\n";
  cout << "with its outgoing edges is exactly one side of a cut. So every pick\n";
  cout << "is the cut property at work, but on all the cuts at once instead of\n";
  cout << "on one growing tree, and that is why a round can add several edges.\n";
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
  cout << "Boruvka reads every one of these in every round to find each\n";
  cout << "piece's cheapest way out.\n";
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
  cout << "  A lone vertex has no edge leaving it, so it stays its own piece.\n";
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
    cout << "\n=== G29: Boruvka's Algorithm ===\n";
    cout << "Graph: Undirected, weighted | V = " << n << ", E = " << edgeCount
         << "\n";
    cout << "1. Build a weighted graph\n";
    cout << "2. Load demo graph (7 vertices)\n";
    cout << "3. Load tie demo (4 vertices)\n";
    cout << "4. Load rounds demo (8 vertices)\n";
    cout << "5. Graph operations\n";
    cout << "6. Run Boruvka and show every round\n";
    cout << "7. Show the weight matrix\n";
    cout << "8. Show the edge list\n";
    cout << "9. Exit\n";

    int choice = readInt("Select: ");

    if (choice == 1) {
      buildGraph();
    } else if (choice == 2) {
      loadDemo();
    } else if (choice == 3) {
      loadTieDemo();
    } else if (choice == 4) {
      loadRoundsDemo();
    } else if (choice == 5) {
      operationsMenu();
    } else if (choice == 6) {
      runBoruvka();
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
