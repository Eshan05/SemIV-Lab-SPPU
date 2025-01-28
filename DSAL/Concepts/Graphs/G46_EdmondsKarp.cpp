// 32. Maximum flow, found by Edmonds-Karp: Repeatedly find a shortest path
// from source to sink in the residual network, push its bottleneck, and add
// the reverse arc so an earlier choice can be undone. Demo: A four-vertex
// network whose last path is one arc longer than the first two, and whose
// final value equals the capacity of the cut around the source.

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

const int MAXV = 15;
const int TRACE_LIMIT = 60;

static int n = 0;
static int arcCount = 0;
static int capM[MAXV][MAXV];
static vector<int> outAdj[MAXV];

static int resM[MAXV][MAXV];
static int flowM[MAXV][MAXV];
static int level[MAXV];
static int parent[MAXV];
static bool reach[MAXV];
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

static string arcText(int u, int v) {
  return "(" + to_string(u) + " -> " + to_string(v) + ")";
}

static string setText(bool marked[MAXV]) {
  string text = "{";
  bool first = true;
  for (int i = 0; i < n; i++) {
    if (!marked[i]) continue;
    if (!first) text += " ";
    text += to_string(i);
    first = false;
  }
  return text + "}";
}

static void say(const string &line) {
  if (traceShown >= TRACE_LIMIT) return;
  traceShown++;
  cout << line << "\n";
}

static string pathText(const vector<int> &path) {
  string text;
  for (int i = 0; i < (int)path.size(); i++) {
    if (i > 0) text += " -> ";
    text += "V" + to_string(path[i]);
  }
  return text;
}

static void resetNetwork() {
  for (int i = 0; i < MAXV; i++) {
    outAdj[i].clear();
    for (int j = 0; j < MAXV; j++) capM[i][j] = 0;
  }
  n = 0;
  arcCount = 0;
}

static void addArc(int u, int v, int capacity) {
  capM[u][v] = capacity;
  outAdj[u].push_back(v);
  arcCount++;
}

static bool hasArc(int u, int v) { return capM[u][v] > 0; }

static void bfsAugment(int s, int t) {
  for (int i = 0; i < n; i++) {
    level[i] = -1;
    parent[i] = -1;
  }

  int queue[MAXV];
  int head = 0;
  int tail = 0;
  queue[tail++] = s;
  level[s] = 0;

  string order = "V" + to_string(s);
  while (head < tail && level[t] == -1) {
    int u = queue[head++];
    for (int v = 0; v < n; v++) {
      if (resM[u][v] <= 0) continue;
      if (level[v] != -1) continue;
      level[v] = level[u] + 1;
      parent[v] = u;
      queue[tail++] = v;
      order += ", V" + to_string(v) + " at " +
               plural(level[v], "step", "steps");
    }
  }

  say("  BFS order: " + order + ".");
  if (level[t] == -1) {
    say("  V" + to_string(t) + " is not reachable in the residual, so no");
    say("  augmenting path is left.");
  }
}

static void runEdmondsKarp() {
  if (n == 0) {
    cout << "Build the network first.\n";
    return;
  }

  int s;
  int t;
  while (true) {
    s = readInt("  Source (0.." + to_string(n - 1) + ", -1 to cancel): ");
    if (s == -1) {
      cout << "  Cancelled.\n";
      return;
    }
    if (s >= 0 && s < n) break;
    cout << "  Rejected: Source must be in 0.." << (n - 1) << ".\n";
  }
  while (true) {
    t = readInt("  Sink (0.." + to_string(n - 1) + ", -1 to cancel): ");
    if (t == -1) {
      cout << "  Cancelled.\n";
      return;
    }
    if (t >= 0 && t < n) break;
    cout << "  Rejected: Sink must be in 0.." << (n - 1) << ".\n";
  }
  if (s == t) {
    cout << "  Rejected: Source and sink must be different vertices.\n";
    return;
  }

  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      resM[i][j] = capM[i][j];
      flowM[i][j] = 0;
    }
  }
  traceShown = 0;

  cout << "\n--- Edmonds-Karp from V" << s << " to V" << t << " ---\n";
  cout << "The residual network starts as the original arcs. A path is found by\n";
  cout << "breadth-first search, so it uses the fewest arcs, and the smallest\n";
  cout << "capacity left on it is pushed. Every push also adds a reverse arc of\n";
  cout << "the same size, which is how a later path can cancel an earlier one.\n";

  long long total = 0;
  int rounds = 0;

  while (true) {
    bfsAugment(s, t);
    if (level[t] == -1) break;

    vector<int> path;
    for (int v = t; v != -1; v = parent[v]) path.push_back(v);
    for (int i = 0; i < (int)path.size() / 2; i++) {
      int tmp = path[i];
      path[i] = path[path.size() - 1 - i];
      path[path.size() - 1 - i] = tmp;
    }

    int push = 1 << 29;
    for (int i = 0; i + 1 < (int)path.size(); i++) {
      int u = path[i];
      int v = path[i + 1];
      if (resM[u][v] < push) push = resM[u][v];
    }

    rounds++;
    say("  Path " + to_string(rounds) + ": " + pathText(path) + ".");
    string caps;
    for (int i = 0; i + 1 < (int)path.size(); i++) {
      int u = path[i];
      int v = path[i + 1];
      if (i > 0) caps += ", ";
      caps += to_string(resM[u][v]);
    }
    say("  Residual room on the arcs: " + caps + ". Bottleneck " +
        to_string(push) + ".");

    for (int i = 0; i + 1 < (int)path.size(); i++) {
      int u = path[i];
      int v = path[i + 1];
      int before = resM[u][v];
      int backBefore = resM[v][u];
      resM[u][v] -= push;
      resM[v][u] += push;
      if (capM[u][v] > 0) flowM[u][v] += push;
      else flowM[v][u] -= push;
      say("    " + arcText(u, v) + ": Residual " + to_string(before) + " -> " +
          to_string(resM[u][v]) + ", reverse " + to_string(backBefore) +
          " -> " + to_string(resM[v][u]) + ".");
    }

    total += push;
    cout << "  Pushed " << push << ", so the flow is now " << total << ".\n";
  }

  cout << "\n--- Flow on each arc ---\n";
  for (int u = 0; u < n; u++) {
    for (int v = 0; v < n; v++) {
      if (!hasArc(u, v)) continue;
      cout << "  " << arcText(u, v) << ": " << flowM[u][v] << " of "
           << capM[u][v] << ".\n";
    }
  }

  for (int i = 0; i < n; i++) reach[i] = false;
  int queue[MAXV];
  int head = 0;
  int tail = 0;
  queue[tail++] = s;
  reach[s] = true;
  while (head < tail) {
    int u = queue[head++];
    for (int v = 0; v < n; v++) {
      if (resM[u][v] <= 0 || reach[v]) continue;
      reach[v] = true;
      queue[tail++] = v;
    }
  }

  cout << "\n--- Minimum cut ---\n";
  cout << "Reachable from V" << s << " in the residual: " << setText(reach)
       << ".\n";
  cout << "Every arc from that set to the rest is cut:";
  long long cutCapacity = 0;
  bool anyCut = false;
  string cutText;
  for (int u = 0; u < n; u++) {
    if (!reach[u]) continue;
    for (int v = 0; v < n; v++) {
      if (reach[v] || !hasArc(u, v)) continue;
      if (anyCut) cutText += " + ";
      cutText += arcText(u, v) + " " + to_string(capM[u][v]);
      cutCapacity += capM[u][v];
      anyCut = true;
    }
  }
  if (anyCut) {
    cout << " " << cutText << " = " << cutCapacity << ".\n";
  } else {
    cout << " none, so the cut carries 0.\n";
  }

  cout << "\nMax flow = " << total << ". Min cut capacity = " << cutCapacity
       << ".\n";
  cout << "The two numbers always agree: Nothing can cross a cut more than its\n";
  cout << "capacity, and the residual proves this flow cannot grow, so this cut\n";
  cout << "is as small as the flow is large.\n";
  cout << "Augmenting paths used: " << plural(rounds, "path", "paths") << ".\n";
  cout << "Edmonds-Karp finds a shortest path each round, so the number of\n";
  cout << "rounds stays small even when capacities are large.\n";
  if (traceShown >= TRACE_LIMIT) {
    cout << "(Trace stops after " << TRACE_LIMIT
         << " lines. The flow itself ran to the end.)\n";
  }
}

static void showCapacityMatrix() {
  cout << "\n--- Capacity matrix ---\n";
  if (n == 0) {
    cout << "(Empty)\n";
    return;
  }
  cout << "      ";
  for (int j = 0; j < n; j++) cout << setw(5) << j;
  cout << "\n";
  for (int i = 0; i < n; i++) {
    cout << setw(4) << i << "  ";
    for (int j = 0; j < n; j++) cout << setw(5) << capM[i][j];
    cout << "\n";
  }
  cout << "A zero means no arc. The table is not symmetric, because an arc has\n";
  cout << "a direction, and the reverse pair would be a separate arc.\n";
}

static void showArcs() {
  cout << "\n--- Arc list ---\n";
  if (n == 0) {
    cout << "(Empty)\n";
    return;
  }
  int shown = 0;
  for (int u = 0; u < n; u++) {
    for (int v = 0; v < n; v++) {
      if (!hasArc(u, v)) continue;
      shown++;
      cout << "  A" << (shown - 1) << ": " << arcText(u, v) << " capacity "
           << capM[u][v] << "\n";
    }
  }
  if (shown == 0) cout << "(Empty)\n";
  cout << "Every arc leaves its own vertex, so the source is the one that sends\n";
  cout << "flow and the sink is the one that receives it.\n";
}

static void buildNetwork() {
  int fresh = readRange("Number of vertices (0.." + to_string(MAXV) + "): ", 0,
                        MAXV);
  resetNetwork();
  n = fresh;

  if (n == 0) {
    cout << "Build complete: 0 vertices, 0 arcs.\n";
    return;
  }

  int limit = n * (n - 1);
  cout << "Directed arcs only, vertices 0.." << (n - 1) << ".\n";
  cout << "A capacity is the most that may cross an arc, from 1 to 99.\n";

  while (true) {
    cout << "\nArc " << (arcCount + 1) << " (Limit " << limit
         << ", -1 to stop)\n";
    int u = readInt("  From vertex (0.." + to_string(n - 1) +
                    ", -1 to stop): ");
    if (u == -1) break;
    int v = readInt("  To vertex (0.." + to_string(n - 1) + ", -1 to stop): ");
    if (v == -1) break;
    if (u < 0 || u >= n || v < 0 || v >= n) {
      cout << "  Rejected: Vertices must be in 0.." << (n - 1) << ".\n";
      continue;
    }
    if (u == v) {
      cout << "  Rejected: An arc cannot leave and enter the same vertex.\n";
      continue;
    }
    if (hasArc(u, v)) {
      cout << "  Rejected: " << arcText(u, v) << " already exists.\n";
      continue;
    }
    if (hasArc(v, u)) {
      cout << "  Rejected: The reverse " << arcText(v, u)
           << " already exists, and the residual builds its own reverse arc.\n";
      continue;
    }
    int capacity = readRange("  Capacity (1..99): ", 1, 99);
    addArc(u, v, capacity);
    cout << "  Added A" << (arcCount - 1) << " " << arcText(u, v)
         << " capacity " << capacity << "\n";
    cout << "  Arcs = " << arcCount << " now.\n";
  }

  cout << "\nBuild complete: " << n
       << (n == 1 ? " vertex, " : " vertices, ") << arcCount
       << (arcCount == 1 ? " arc.\n" : " arcs.\n");
}

static void loadDiamondDemo() {
  resetNetwork();
  n = 4;
  cout << "Diamond demo: 4 vertices, 5 arcs.\n";
  cout << "  0 -> 1 (3), 0 -> 2 (2), 1 -> 2 (1), 1 -> 3 (2), 2 -> 3 (3).\n";
  cout << "  The two short paths carry four units. The third path is one arc\n";
  cout << "  longer and uses the last unit on 1 -> 2, while every push adds a\n";
  cout << "  reverse arc that a later run could use to cancel flow.\n\n";

  addArc(0, 1, 3);
  addArc(0, 2, 2);
  addArc(1, 2, 1);
  addArc(1, 3, 2);
  addArc(2, 3, 3);

  cout << "Demo loaded: " << n << " vertices, " << arcCount << " arcs.\n";
}

static void loadLayeredDemo() {
  resetNetwork();
  n = 6;
  cout << "Layered demo: 6 vertices, 6 arcs.\n";
  cout << "  0 -> 1 (4), 0 -> 2 (3), 1 -> 3 (4), 2 -> 3 (3),\n";
  cout << "  3 -> 4 (10), 4 -> 5 (10).\n";
  cout << "  Both short paths share the wide middle arc, so the flow is 7.\n\n";

  addArc(0, 1, 4);
  addArc(0, 2, 3);
  addArc(1, 3, 4);
  addArc(2, 3, 3);
  addArc(3, 4, 10);
  addArc(4, 5, 10);

  cout << "Demo loaded: " << n << " vertices, " << arcCount << " arcs.\n";
}

static void addArcInteractive() {
  if (n == 0) {
    cout << "Build the network first.\n";
    return;
  }
  if (arcCount == n * (n - 1)) {
    cout << "The network is already complete in both directions, so every\n";
    cout << "ordered pair of vertices has an arc.\n";
    return;
  }

  cout << "\nAdd an arc (V = " << n << ", so vertices 0.." << (n - 1) << ")\n";
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
      cout << "  Please re-enter this arc.\n";
      continue;
    }
    if (u == v) {
      cout << "  Rejected: An arc cannot leave and enter the same vertex.\n";
      cout << "  Please re-enter this arc.\n";
      continue;
    }
    if (hasArc(u, v)) {
      cout << "  Rejected: " << arcText(u, v) << " already exists.\n";
      cout << "  Please re-enter this arc.\n";
      continue;
    }
    if (hasArc(v, u)) {
      cout << "  Rejected: The reverse " << arcText(v, u) << " already exists,\n";
      cout << "  and the residual builds its own reverse arc.\n";
      cout << "  Please re-enter this arc.\n";
      continue;
    }
    int capacity = readRange("  Capacity (1..99): ", 1, 99);
    addArc(u, v, capacity);
    cout << "  Added A" << (arcCount - 1) << " " << arcText(u, v)
         << " capacity " << capacity << "\n";
    cout << "    capM[" << u << "][" << v << "] = " << capacity << "\n";
    cout << "    outAdj[" << u << "] gained V" << v << ".\n";
    cout << "  Arcs = " << arcCount << " now.\n";
    return;
  }
}

static void removeArcInteractive() {
  if (n == 0) {
    cout << "Build the network first.\n";
    return;
  }

  cout << "\nRemove an arc (V = " << n << ", so vertices 0.." << (n - 1)
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
      cout << "  Please re-enter this arc.\n";
      continue;
    }
    if (!hasArc(u, v)) {
      cout << "  Rejected: " << arcText(u, v) << " is not in the network.\n";
      cout << "  Please re-enter this arc.\n";
      continue;
    }

    capM[u][v] = 0;
    for (int i = 0; i < (int)outAdj[u].size(); i++) {
      if (outAdj[u][i] != v) continue;
      outAdj[u].erase(outAdj[u].begin() + i);
      break;
    }
    arcCount--;

    cout << "  Removed " << arcText(u, v) << "\n";
    cout << "    capM[" << u << "][" << v << "] = 0\n";
    cout << "    outAdj[" << u << "] lost V" << v << ".\n";
    cout << "  Arcs = " << arcCount << " now.\n";
    return;
  }
}

static void changeCapacityInteractive() {
  if (n == 0) {
    cout << "Build the network first.\n";
    return;
  }

  cout << "\nChange a capacity (V = " << n << ", so vertices 0.." << (n - 1)
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
      cout << "  Please re-enter this arc.\n";
      continue;
    }
    if (!hasArc(u, v)) {
      cout << "  Rejected: " << arcText(u, v) << " is not in the network.\n";
      cout << "  Please re-enter this arc.\n";
      continue;
    }
    int capacity = readRange("  New capacity (1..99): ", 1, 99);
    int before = capM[u][v];
    capM[u][v] = capacity;
    cout << "  " << arcText(u, v) << " goes from " << before << " to "
         << capacity << ".\n";
    cout << "    capM[" << u << "][" << v << "] holds " << capacity << " now.\n";
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
    capM[fresh][i] = 0;
    capM[i][fresh] = 0;
  }
  outAdj[fresh].clear();
  n++;

  cout << "\nAdded V" << fresh << " with no arcs.\n";
  cout << "  V = " << n << ", arcs = " << arcCount << ".\n";
  cout << "  It can be a source or a sink, but nothing reaches or leaves it\n";
  cout << "  until an arc is added.\n";
}

static void removeVertexInteractive() {
  if (n == 0) {
    cout << "Build the network first.\n";
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
      kept[na][nb] = capM[a][b];
    }
  }

  vector<int> keptAdj[MAXV];
  for (int a = 0; a < n; a++) {
    if (a == v) continue;
    int na = (a < v) ? a : a - 1;
    for (int w : outAdj[a]) {
      if (w == v) continue;
      keptAdj[na].push_back((w < v) ? w : w - 1);
    }
  }

  int before = arcCount;
  n--;
  for (int a = 0; a < MAXV; a++) {
    outAdj[a].clear();
    for (int b = 0; b < MAXV; b++) capM[a][b] = 0;
  }
  for (int a = 0; a < n; a++) {
    outAdj[a] = keptAdj[a];
    for (int b = 0; b < n; b++) capM[a][b] = kept[a][b];
  }

  arcCount = 0;
  for (int a = 0; a < n; a++) {
    for (int b = 0; b < n; b++) {
      if (capM[a][b] > 0) arcCount++;
    }
  }

  cout << "\nRemoved V" << v << ".\n";
  if (v < n) cout << "  Vertices above V" << v << " shift down by one.\n";
  cout << "  Arcs lost: " << before - arcCount;
  cout << " (Recounted from the matrix.)\n";
  cout << "  V = " << n << ", arcs = " << arcCount << ".\n";
}

static void clearArcs() {
  if (n == 0) {
    cout << "Build the network first.\n";
    return;
  }
  if (arcCount == 0) {
    cout << "\nThe network already has no arcs.\n";
    return;
  }

  int before = arcCount;
  for (int u = 0; u < n; u++) {
    outAdj[u].clear();
    for (int w = 0; w < n; w++) capM[u][w] = 0;
  }
  arcCount = 0;

  cout << "\nRemoved all " << before
       << (before == 1 ? " arc.\n" : " arcs.\n");
  cout << "  V = " << n << ", arcs = 0. The vertices are still there.\n";
  cout << "  With no arcs the flow is 0 and the cut is empty.\n";
}

static void operationsMenu() {
  while (true) {
    cout << "\n--- Network operations ---\n";
    cout << "Network: Directed | V = " << n << ", arcs = " << arcCount << "\n";
    cout << "1. Add an arc to the current network\n";
    cout << "2. Remove an arc from the current network\n";
    cout << "3. Change an arc capacity\n";
    cout << "4. Add a vertex to the current network\n";
    cout << "5. Remove a vertex from the current network\n";
    cout << "6. Clear all arcs\n";
    cout << "0. Back to the main menu\n";

    int choice = readInt("Select: ");

    if (choice == 1) {
      addArcInteractive();
    } else if (choice == 2) {
      removeArcInteractive();
    } else if (choice == 3) {
      changeCapacityInteractive();
    } else if (choice == 4) {
      addVertexInteractive();
    } else if (choice == 5) {
      removeVertexInteractive();
    } else if (choice == 6) {
      clearArcs();
    } else if (choice == 0) {
      return;
    } else {
      cout << "Invalid option.\n";
    }
  }
}

int main() {
  resetNetwork();

  while (true) {
    cout << "\n=== G46: Maximum Flow (Edmonds-Karp) ===\n";
    cout << "Network: Directed | V = " << n << ", arcs = " << arcCount << "\n";
    cout << "1. Build a directed network\n";
    cout << "2. Load the diamond demo (4 vertices)\n";
    cout << "3. Load the layered demo (6 vertices)\n";
    cout << "4. Network operations\n";
    cout << "5. Run Edmonds-Karp\n";
    cout << "6. Show the capacity matrix\n";
    cout << "7. Show the arc list\n";
    cout << "8. Exit\n";

    int choice = readInt("Select: ");

    if (choice == 1) {
      buildNetwork();
    } else if (choice == 2) {
      loadDiamondDemo();
    } else if (choice == 3) {
      loadLayeredDemo();
    } else if (choice == 4) {
      operationsMenu();
    } else if (choice == 5) {
      runEdmondsKarp();
    } else if (choice == 6) {
      showCapacityMatrix();
    } else if (choice == 7) {
      showArcs();
    } else if (choice == 8) {
      cout << "Exiting...\n";
      break;
    } else {
      cout << "Invalid option.\n";
    }
  }

  return 0;
}
