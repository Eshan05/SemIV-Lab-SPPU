// 31a. The travelling salesman, solved by Held-Karp: dp[S][v] is the cheapest
// way to start at city 0, visit every city in the set S once, and stop at city
// v. Each layer adds one city, so the table holds every partial tour and the
// answer is the best full one plus the road home. Demo: Four cities where the
// whole table fits on screen, then five cities where a far cheaper tour exists
// than the greedy walk finds.

#include <algorithm>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

const int MAXC = 10;
const long long INF = 1000000000000LL;
const int TABLE_ROWS = 24;

static int n = 0;
static int edgeCount = 0;
static long long distM[MAXC][MAXC];

static long long dp[1 << MAXC][MAXC];
static int parent[1 << MAXC][MAXC];

static long long hkCost = INF;
static vector<int> hkTour;
static bool hkDone = false;

static long long nnCost = INF;
static vector<int> nnTour;
static bool nnDone = false;

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

static void resetTSP() {
  for (int i = 0; i < MAXC; i++) {
    for (int j = 0; j < MAXC; j++) distM[i][j] = INF;
  }
  n = 0;
  edgeCount = 0;
  hkDone = false;
  nnDone = false;
  hkCost = INF;
  nnCost = INF;
  hkTour.clear();
  nnTour.clear();
}

static void addRoad(int u, int v, long long w) {
  distM[u][v] = w;
  distM[v][u] = w;
  edgeCount++;
}

static string roadText(int u, int v) {
  return "(" + to_string(u) + " - " + to_string(v) + ")";
}

static string plural(int count, const string &one, const string &many) {
  return to_string(count) + " " + (count == 1 ? one : many);
}

static string maskText(int mask) {
  string text = "{";
  bool first = true;
  for (int v = 0; v < n; v++) {
    if (!(mask & (1 << v))) continue;
    if (!first) text += " ";
    text += to_string(v);
    first = false;
  }
  return text + "}";
}

static string tourText(const vector<int> &tour) {
  string text;
  for (int i = 0; i < (int)tour.size(); i++) {
    if (i > 0) text += " - ";
    text += to_string(tour[i]);
  }
  return text;
}

static bool hasRoad(int u, int v) { return distM[u][v] < INF; }

static void showMatrix() {
  cout << "\n--- Distance matrix ---\n";
  if (n == 0) {
    cout << "(Empty)\n";
    return;
  }
  cout << "      ";
  for (int j = 0; j < n; j++) cout << setw(5) << j;
  cout << "\n";
  for (int i = 0; i < n; i++) {
    cout << setw(4) << i << "  ";
    for (int j = 0; j < n; j++) {
      if (i == j) {
        cout << setw(5) << "-";
      } else if (hasRoad(i, j)) {
        cout << setw(5) << distM[i][j];
      } else {
        cout << setw(5) << "..";
      }
    }
    cout << "\n";
  }
  cout << "A dash is the same city twice and two dots mean no road. The table is\n";
  cout << "symmetric, since a road costs the same both ways.\n";
}

static void showEdges() {
  cout << "\n--- Road list ---\n";
  if (n == 0) {
    cout << "(Empty)\n";
    return;
  }
  int id = 0;
  for (int u = 0; u < n; u++) {
    for (int v = u + 1; v < n; v++) {
      if (!hasRoad(u, v)) continue;
      cout << setw(3) << ("E" + to_string(id)) << ": " << roadText(u, v)
           << " costs " << distM[u][v] << "\n";
      id++;
    }
  }
  if (id == 0) cout << "(No roads)\n";
  cout << "V = " << n << ", E = " << edgeCount << ".\n";
}

static void buildTSP() {
  int fresh = readRange("Number of cities (0.." + to_string(MAXC) + "): ", 0,
                        MAXC);
  resetTSP();
  n = fresh;

  if (n == 0) {
    cout << "Build complete: 0 cities, 0 roads.\n";
    return;
  }

  int limit = n * (n - 1) / 2;
  cout << "Roads are undirected, and a missing pair simply has no road.\n";

  while (true) {
    cout << "\nRoad " << (edgeCount + 1) << " (Limit " << limit
         << ", -1 to stop)\n";
    if (edgeCount == limit) {
      cout << "Every pair already has a road, so the graph is complete.\n";
      break;
    }
    int u = readInt("  From city (0.." + to_string(n - 1) + ", -1 to stop): ");
    if (u == -1) break;
    int v = readInt("  To city (0.." + to_string(n - 1) + ", -1 to stop): ");
    if (v == -1) break;
    if (u < 0 || u >= n || v < 0 || v >= n) {
      cout << "  Rejected: Cities must be in 0.." << (n - 1) << ".\n";
      continue;
    }
    if (u == v) {
      cout << "  Rejected: A city needs no road to itself.\n";
      continue;
    }
    if (hasRoad(u, v)) {
      cout << "  Rejected: " << roadText(u, v) << " already exists.\n";
      continue;
    }
    int w = readRange("  Cost (1..99): ", 1, 99);
    addRoad(u, v, w);
    cout << "  Added E" << (edgeCount - 1) << " " << roadText(u, v)
         << " costing " << w << "\n";
  }

  cout << "\nBuild complete: " << plural(n, "city", "cities") << ", "
       << plural(edgeCount, "road", "roads") << ".\n";
}

static void loadSmallDemo() {
  resetTSP();
  n = 4;
  cout << "Small demo: 4 cities, 6 roads, every pair joined.\n";
  cout << "  0 - 1 costs 10, 0 - 2 costs 15, 0 - 3 costs 20,\n";
  cout << "  1 - 2 costs 35, 1 - 3 costs 25, 2 - 3 costs 30.\n";
  cout << "  Four cities mean only six tours to compare, and the table of\n";
  cout << "  partial tours fits on the screen.\n\n";

  addRoad(0, 1, 10);
  addRoad(0, 2, 15);
  addRoad(0, 3, 20);
  addRoad(1, 2, 35);
  addRoad(1, 3, 25);
  addRoad(2, 3, 30);

  cout << "Demo loaded: " << n << " cities, " << edgeCount << " roads.\n";
}

static void loadRingDemo() {
  resetTSP();
  n = 5;
  cout << "Ring demo: 5 cities, 10 roads, every pair joined.\n";
  cout << "  A ring of five cheap roads with dearer shortcuts across it, so the\n";
  cout << "  best tour follows the ring instead of cutting corners.\n\n";

  addRoad(0, 1, 10);
  addRoad(1, 2, 12);
  addRoad(2, 3, 9);
  addRoad(3, 4, 11);
  addRoad(4, 0, 14);
  addRoad(0, 2, 18);
  addRoad(0, 3, 19);
  addRoad(1, 3, 20);
  addRoad(1, 4, 21);
  addRoad(2, 4, 17);

  cout << "Demo loaded: " << n << " cities, " << edgeCount << " roads.\n";
}

static void loadTrapDemo() {
  resetTSP();
  n = 5;
  cout << "Map demo: 5 towns, 10 roads, every pair joined.\n";
  cout << "  Five towns on a map, with rounded straight-line distances:\n";
  cout << "  V0 (7, 14), V1 (3, 4), V2 (9, 7), V3 (19, 0), V4 (0, 18).\n";
  cout << "  Greed walks 0 - 2 - 1, then has to reach V3 from V4 and pays 26 for\n";
  cout << "  that one road. The best tour is 0 - 2 - 3 - 1 - 4 - 0, or the same\n";
  cout << "  loop the other way round: It reaches V3 from V2 for 12 and comes home\n";
  cout << "  from V4 for 8.\n\n";

  addRoad(0, 1, 11);
  addRoad(0, 2, 7);
  addRoad(0, 3, 18);
  addRoad(0, 4, 8);
  addRoad(1, 2, 7);
  addRoad(1, 3, 16);
  addRoad(1, 4, 14);
  addRoad(2, 3, 12);
  addRoad(2, 4, 14);
  addRoad(3, 4, 26);

  cout << "Demo loaded: " << n << " cities, " << edgeCount << " roads.\n";
}

static void runHeldKarp() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }

  hkDone = false;
  hkTour.clear();
  hkCost = INF;

  if (n == 1) {
    hkTour.push_back(0);
    hkCost = 0;
    hkDone = true;
    cout << "\n--- Held-Karp ---\n";
    cout << "One city needs no travelling: The tour is 0 and it costs 0.\n";
    return;
  }

  int full = 1 << n;
  int allMask = full - 1;
  for (int mask = 0; mask < full; mask++) {
    for (int v = 0; v < n; v++) {
      dp[mask][v] = INF;
      parent[mask][v] = -1;
    }
  }
  dp[1][0] = 0;

  cout << "\n--- Held-Karp ---\n";
  cout << "dp[S][v] is the cheapest tour that starts at 0, visits every city of\n";
  cout << "the set S exactly once, and stops at v. A single city is easy:\n";
  cout << "dp[{0}][0] = 0. To reach v through S, take a city u already sitting at\n";
  cout << "the end of a smaller set, then add the road u - v.\n\n";

  for (int mask = 1; mask < full; mask++) {
    if (!(mask & 1)) continue;
    for (int v = 0; v < n; v++) {
      if (!(mask & (1 << v))) continue;
      if (dp[mask][v] >= INF) continue;
      for (int u = 1; u < n; u++) {
        if (mask & (1 << u)) continue;
        if (!hasRoad(v, u)) continue;
        long long candidate = dp[mask][v] + distM[v][u];
        int nextMask = mask | (1 << u);
        if (candidate < dp[nextMask][u]) {
          dp[nextMask][u] = candidate;
          parent[nextMask][u] = v;
        }
      }
    }
  }

  cout << "--- Table, one layer at a time ---\n";
  int printed = 0;
  bool cut = false;
  for (int size = 2; size <= n && !cut; size++) {
    int inLayer = 0;
    for (int mask = 1; mask < full; mask++) {
      if (!(mask & 1)) continue;
      int bits = 0;
      for (int v = 0; v < n; v++) {
        if (mask & (1 << v)) bits++;
      }
      if (bits == size) inLayer++;
    }
    cout << "\nLayer " << size << " (" << plural(inLayer, "subset", "subsets")
         << " of " << size << " cities, city 0 always in):\n";

    for (int mask = 1; mask < full && !cut; mask++) {
      if (!(mask & 1)) continue;
      int bits = 0;
      for (int v = 0; v < n; v++) {
        if (mask & (1 << v)) bits++;
      }
      if (bits != size) continue;
      for (int v = 1; v < n; v++) {
        if (!(mask & (1 << v))) continue;
        if (dp[mask][v] >= INF) continue;
        if (printed >= TABLE_ROWS) {
          cut = true;
          break;
        }
        cout << "  " << maskText(mask) << " ending at V" << v << ": "
             << dp[mask][v] << " (Came from V" << parent[mask][v] << ")\n";
        printed++;
      }
    }
  }
  if (cut) {
    cout << "\n(Table stops after " << TABLE_ROWS
         << " rows. Every subset was computed.)\n";
  }

  hkCost = INF;
  int bestLast = -1;
  for (int v = 1; v < n; v++) {
    if (dp[allMask][v] >= INF) continue;
    if (!hasRoad(v, 0)) continue;
    long long total = dp[allMask][v] + distM[v][0];
    if (total < hkCost) {
      hkCost = total;
      bestLast = v;
    }
  }

  cout << "\n--- Closing the tour ---\n";
  if (bestLast == -1) {
    cout << "No full tour exists: Some city cannot be reached along a closed\n";
    cout << "route, so the graph is too disconnected for a travelling salesman.\n";
    return;
  }
  for (int v = 1; v < n; v++) {
    if (dp[allMask][v] >= INF || !hasRoad(v, 0)) continue;
    cout << "  Ending at V" << v << ": " << dp[allMask][v] << " + "
         << distM[v][0] << " to come home = " << dp[allMask][v] + distM[v][0]
         << "\n";
  }
  cout << "The cheapest is to end at V" << bestLast << ".\n";

  cout << "\n--- Reading the tour back ---\n";
  vector<int> reversedTour;
  int mask = allMask;
  int v = bestLast;
  while (true) {
    reversedTour.push_back(v);
    if (v == 0) break;
    int previous = parent[mask][v];
    int smaller = mask ^ (1 << v);
    cout << "  V" << v << " in " << maskText(mask) << " came from V" << previous
         << ": " << dp[mask][v] << " = " << dp[smaller][previous] << " + "
         << distM[previous][v] << " for the road " << roadText(previous, v)
         << "\n";
    mask = smaller;
    v = previous;
  }

  reverse(reversedTour.begin(), reversedTour.end());
  hkTour = reversedTour;
  hkTour.push_back(0);
  hkDone = true;

  cout << "\nTour: ";
  for (int i = 0; i < (int)hkTour.size(); i++) {
    if (i > 0) cout << " - ";
    cout << hkTour[i];
    if (i > 0) cout << " (" << distM[hkTour[i - 1]][hkTour[i]] << ")";
  }
  cout << "\nTotal cost: " << hkCost << ".\n";
  cout << "\nEvery partial tour in the table is reused by later layers, and that\n";
  cout << "sharing is the whole point: The table holds 2 to the n times n entries\n";
  cout << "instead of n factorial tours.\n";
}

static void runNearest() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }

  nnDone = false;
  nnTour.clear();
  nnCost = INF;

  vector<bool> seen(n, false);
  vector<int> tour;
  tour.push_back(0);
  seen[0] = true;

  cout << "\n--- Nearest neighbour ---\n";
  cout << "Stand at a city, drive to the nearest city not yet visited, and repeat\n";
  cout << "until none is left. No looking ahead, and no second thoughts.\n";

  if (n == 1) {
    nnTour.push_back(0);
    nnCost = 0;
    nnDone = true;
    cout << "\nOne city needs no driving, so the cost is 0.\n";
    return;
  }

  long long total = 0;
  int current = 0;
  for (int step = 1; step < n; step++) {
    int best = -1;
    for (int u = 0; u < n; u++) {
      if (seen[u]) continue;
      if (!hasRoad(current, u)) continue;
      if (best == -1 || distM[current][u] < distM[current][best]) best = u;
    }
    if (best == -1) {
      cout << "\nStuck at V" << current << ": Every city left has no road from\n";
      cout << "here, so greed finds no tour at all.\n";
      return;
    }
    cout << "  Step " << step << ": From V" << current << " the nearest city is"
         << " V" << best << " at " << distM[current][best]
         << ". Total is now " << total + distM[current][best] << ".\n";
    total += distM[current][best];
    current = best;
    seen[best] = true;
    tour.push_back(best);
  }

  if (!hasRoad(current, 0)) {
    cout << "\nThe last city is V" << current << ", and it has no road home, so\n";
    cout << "greed finds no closed tour.\n";
    return;
  }
  total += distM[current][0];
  tour.push_back(0);
  nnCost = total;
  nnTour = tour;
  nnDone = true;

  cout << "  Home: V" << current << " to V0 costs " << distM[current][0]
       << ".\n";
  cout << "\nTour: " << tourText(nnTour) << "\n";
  cout << "Total cost: " << nnCost << ".\n";
}

static void compareBoth() {
  if (!hkDone || !nnDone) {
    cout << "Run both methods first, so there is something to compare.\n";
    return;
  }

  cout << "\n--- Comparison ---\n";
  cout << "Held-Karp        : " << hkCost << " by " << tourText(hkTour) << "\n";
  cout << "Nearest neighbour: " << nnCost << " by " << tourText(nnTour) << "\n";

  if (nnCost == hkCost) {
    cout << "\nGreed matched the best tour here. That happens often, and it is\n";
    cout << "exactly what makes the method tempting.\n";
  } else {
    long long extra = nnCost - hkCost;
    cout << "\nGreed paid " << extra << " more, which is "
         << (extra * 100 / hkCost) << " percent above the best tour.\n";
    cout << "The cheap road it took first is the reason: A tour is a closed\n";
    cout << "loop, so a road that looks best now can force a dear one later.\n";
  }
  cout << "\nHeld-Karp always returns the best tour, and pays for that certainty\n";
  cout << "with a table of 2 to the n times n entries. Nearest neighbour makes no\n";
  cout << "such promise: On maps that obey the triangle inequality its tour can\n";
  cout << "still cost a logarithmic factor more than the best, and with general\n";
  cout << "road costs it can be worse still. Only an exact search, or a method\n";
  cout << "with a proof behind it, can promise how close the answer will be.\n";
}

static void addRoadInteractive() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }
  if (edgeCount == n * (n - 1) / 2) {
    cout << "Every pair already has a road.\n";
    return;
  }

  cout << "\nAdd a road (V = " << n << ", so cities 0.." << (n - 1) << ")\n";
  while (true) {
    int u = readInt("  From city (0.." + to_string(n - 1) + ", -1 to cancel): ");
    if (u == -1) {
      cout << "  Cancelled.\n";
      return;
    }
    int v = readInt("  To city (0.." + to_string(n - 1) + ", -1 to cancel): ");
    if (v == -1) {
      cout << "  Cancelled.\n";
      return;
    }
    if (u < 0 || u >= n || v < 0 || v >= n) {
      cout << "  Rejected: Cities must be in 0.." << (n - 1) << ".\n";
      cout << "  Please re-enter this road.\n";
      continue;
    }
    if (u == v) {
      cout << "  Rejected: A city needs no road to itself.\n";
      cout << "  Please re-enter this road.\n";
      continue;
    }
    if (hasRoad(u, v)) {
      cout << "  Rejected: " << roadText(u, v) << " already exists.\n";
      cout << "  Please re-enter this road.\n";
      continue;
    }

    int w = readRange("  Cost (1..99): ", 1, 99);
    addRoad(u, v, w);
    hkDone = false;
    nnDone = false;
    cout << "  Added E" << (edgeCount - 1) << " " << roadText(u, v)
         << " costing " << w << "\n";
    cout << "    distM[" << u << "][" << v << "] and distM[" << v << "][" << u
         << "] both cost " << w << ".\n";
    cout << "  E = " << plural(edgeCount, "road", "roads") << " now.\n";
    return;
  }
}

static void removeRoadInteractive() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }

  cout << "\nRemove a road (V = " << n << ", so cities 0.." << (n - 1) << ")\n";
  while (true) {
    int u = readInt("  From city (0.." + to_string(n - 1) + ", -1 to cancel): ");
    if (u == -1) {
      cout << "  Cancelled.\n";
      return;
    }
    int v = readInt("  To city (0.." + to_string(n - 1) + ", -1 to cancel): ");
    if (v == -1) {
      cout << "  Cancelled.\n";
      return;
    }
    if (u < 0 || u >= n || v < 0 || v >= n) {
      cout << "  Rejected: Cities must be in 0.." << (n - 1) << ".\n";
      cout << "  Please re-enter this road.\n";
      continue;
    }
    if (!hasRoad(u, v)) {
      cout << "  Rejected: " << roadText(u, v) << " is not there.\n";
      cout << "  Please re-enter this road.\n";
      continue;
    }

    distM[u][v] = INF;
    distM[v][u] = INF;
    edgeCount--;
    hkDone = false;
    nnDone = false;

    cout << "  Removed " << roadText(u, v) << "\n";
    cout << "    distM[" << u << "][" << v << "] and distM[" << v << "][" << u
         << "] are back to no road.\n";
    cout << "  E = " << plural(edgeCount, "road", "roads") << " now.\n";
    return;
  }
}

static void changeRoadInteractive() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }

  cout << "\nChange a road cost\n";
  while (true) {
    int u = readInt("  From city (0.." + to_string(n - 1) + ", -1 to cancel): ");
    if (u == -1) {
      cout << "  Cancelled.\n";
      return;
    }
    int v = readInt("  To city (0.." + to_string(n - 1) + ", -1 to cancel): ");
    if (v == -1) {
      cout << "  Cancelled.\n";
      return;
    }
    if (u < 0 || u >= n || v < 0 || v >= n) {
      cout << "  Rejected: Cities must be in 0.." << (n - 1) << ".\n";
      cout << "  Please re-enter this road.\n";
      continue;
    }
    if (!hasRoad(u, v)) {
      cout << "  Rejected: " << roadText(u, v) << " is not there.\n";
      cout << "  Please re-enter this road.\n";
      continue;
    }

    long long before = distM[u][v];
    int w = readRange("  New cost (1..99): ", 1, 99);
    distM[u][v] = w;
    distM[v][u] = w;
    hkDone = false;
    nnDone = false;

    cout << "  " << roadText(u, v) << " goes from " << before << " to " << w
         << ".\n";
    cout << "    distM[" << u << "][" << v << "] and distM[" << v << "][" << u
         << "] both hold " << w << " now.\n";
    return;
  }
}

static void addCityInteractive() {
  if (n >= MAXC) {
    cout << "City limit reached: MAXC = " << MAXC << ".\n";
    return;
  }

  int fresh = n;
  for (int i = 0; i <= fresh; i++) {
    distM[fresh][i] = INF;
    distM[i][fresh] = INF;
  }
  n++;
  hkDone = false;
  nnDone = false;

  cout << "\nAdded V" << fresh << " with no roads.\n";
  cout << "  V = " << n << ", E = " << edgeCount << ".\n";
  cout << "  An isolated city makes a full tour impossible until a road\n";
  cout << "  reaches it.\n";
}

static void removeCityInteractive() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }

  int v;
  while (true) {
    v = readInt("  City to remove (0.." + to_string(n - 1) + ", -1 to cancel): ");
    if (v == -1) {
      cout << "  Cancelled.\n";
      return;
    }
    if (v >= 0 && v < n) break;
    cout << "  Rejected: City must be in 0.." << (n - 1) << ".\n";
  }

  vector<int> keptFrom;
  vector<int> keptTo;
  vector<long long> keptCost;
  for (int u = 0; u < n; u++) {
    if (u == v) continue;
    for (int w = u + 1; w < n; w++) {
      if (w == v) continue;
      if (!hasRoad(u, w)) continue;
      keptFrom.push_back(u < v ? u : u - 1);
      keptTo.push_back(w < v ? w : w - 1);
      keptCost.push_back(distM[u][w]);
    }
  }

  int before = edgeCount;
  int newN = n - 1;
  resetTSP();
  n = newN;

  for (int i = 0; i < (int)keptFrom.size(); i++) {
    addRoad(keptFrom[i], keptTo[i], keptCost[i]);
  }

  cout << "\nRemoved V" << v << ".\n";
  if (v < n) cout << "  Cities above V" << v << " shift down by one.\n";
  cout << "  Roads lost: " << before - edgeCount << ".\n";
  cout << "  V = " << n << ", E = " << edgeCount << ".\n";
}

static void clearRoads() {
  if (n == 0) {
    cout << "Build the graph first.\n";
    return;
  }
  if (edgeCount == 0) {
    cout << "\nThe graph already has no roads.\n";
    return;
  }

  int before = edgeCount;
  for (int u = 0; u < n; u++) {
    for (int w = 0; w < n; w++) distM[u][w] = INF;
  }
  edgeCount = 0;
  hkDone = false;
  nnDone = false;

  cout << "\nRemoved all " << plural(before, "road", "roads") << ".\n";
  cout << "  V = " << n << ", E = 0. The cities are still there.\n";
  cout << "  With no roads no tour exists, and both methods will say so.\n";
}

static void operationsMenu() {
  while (true) {
    cout << "\n--- Graph operations ---\n";
    cout << "Graph: Undirected, weighted | V = " << n << ", E = " << edgeCount
         << "\n";
    cout << "1. Add a road\n";
    cout << "2. Remove a road\n";
    cout << "3. Change a road cost\n";
    cout << "4. Add a city\n";
    cout << "5. Remove a city\n";
    cout << "6. Clear all roads\n";
    cout << "0. Back to the main menu\n";

    int choice = readInt("Select: ");

    if (choice == 1) {
      addRoadInteractive();
    } else if (choice == 2) {
      removeRoadInteractive();
    } else if (choice == 3) {
      changeRoadInteractive();
    } else if (choice == 4) {
      addCityInteractive();
    } else if (choice == 5) {
      removeCityInteractive();
    } else if (choice == 6) {
      clearRoads();
    } else if (choice == 0) {
      return;
    } else {
      cout << "Invalid option.\n";
    }
  }
}

int main() {
  resetTSP();

  while (true) {
    cout << "\n=== G44: Travelling Salesman (Held-Karp) ===\n";
    cout << "Graph: Undirected, weighted | V = " << n << ", E = " << edgeCount
         << "\n";
    cout << "1. Build a connected set of cities\n";
    cout << "2. Load the small demo (4 cities)\n";
    cout << "3. Load the ring demo (5 cities)\n";
    cout << "4. Load the map demo (5 cities)\n";
    cout << "5. Graph operations\n";
    cout << "6. Run Held-Karp\n";
    cout << "7. Run nearest neighbour\n";
    cout << "8. Compare the two methods\n";
    cout << "9. Show the distance matrix\n";
    cout << "10. Show the road list\n";
    cout << "11. Exit\n";

    int choice = readInt("Select: ");

    if (choice == 1) {
      buildTSP();
    } else if (choice == 2) {
      loadSmallDemo();
    } else if (choice == 3) {
      loadRingDemo();
    } else if (choice == 4) {
      loadTrapDemo();
    } else if (choice == 5) {
      operationsMenu();
    } else if (choice == 6) {
      runHeldKarp();
    } else if (choice == 7) {
      runNearest();
    } else if (choice == 8) {
      compareBoth();
    } else if (choice == 9) {
      showMatrix();
    } else if (choice == 10) {
      showEdges();
    } else if (choice == 11) {
      cout << "Exiting...\n";
      break;
    } else {
      cout << "Invalid option.\n";
    }
  }

  return 0;
}
