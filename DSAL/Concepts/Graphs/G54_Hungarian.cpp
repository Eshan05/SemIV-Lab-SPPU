// 34e. The Hungarian method for the assignment problem: Give every row a
// potential u and every column a potential v, keep every reduced cost
// cost[i][j] - u[i] - v[j] non-negative, and let one row at a time grow an
// alternating tree of columns. When a free column appears, the pairs on the
// tree flip and one more row is assigned.
// Demo: A three-by-three matrix where taking the cheapest free column in each
// row costs 10, while the best assignment costs 9.

#include <algorithm>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

const int HMAX = 8;
const int INF = 1000000;
const int TRACE_LIMIT = 90;

static int n = 0;
static int costM[HMAX + 1][HMAX + 1];

static int u[HMAX + 1];
static int v[HMAX + 1];
static int p[HMAX + 1];
static int way[HMAX + 1];
static int minv[HMAX + 1];
static bool used[HMAX + 1];

static int assignRow[HMAX + 1];
static int totalCost = 0;
static bool solved = false;
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

static void say(const string &line) {
  if (traceShown >= TRACE_LIMIT) return;
  traceShown++;
  cout << line << "\n";
}

static void clearSolution() {
  for (int i = 0; i <= HMAX; i++) {
    u[i] = 0;
    v[i] = 0;
    p[i] = 0;
    way[i] = 0;
    minv[i] = 0;
    used[i] = false;
    assignRow[i] = 0;
  }
  totalCost = 0;
  solved = false;
}

static void resetMatrix() {
  for (int i = 0; i <= HMAX; i++) {
    for (int j = 0; j <= HMAX; j++) costM[i][j] = 0;
  }
  n = 0;
  clearSolution();
}

static void setMatrix(const int rows[HMAX][HMAX], int size) {
  resetMatrix();
  n = size;
  for (int i = 1; i <= n; i++) {
    for (int j = 1; j <= n; j++) costM[i][j] = rows[i - 1][j - 1];
  }
}

static int greedyTotal() {
  bool taken[HMAX + 1] = {false};
  int total = 0;
  for (int i = 1; i <= n; i++) {
    int best = -1;
    for (int j = 1; j <= n; j++) {
      if (taken[j]) continue;
      if (best == -1 || costM[i][j] < costM[i][best]) best = j;
    }
    taken[best] = true;
    total += costM[i][best];
  }
  return total;
}

static int bruteOptimum() {
  vector<int> cols(n);
  for (int i = 0; i < n; i++) cols[i] = i + 1;
  int best = INF;
  do {
    int total = 0;
    for (int i = 0; i < n; i++) total += costM[i + 1][cols[i]];
    if (total < best) best = total;
  } while (next_permutation(cols.begin(), cols.end()));
  return best;
}

static int smallestReduced() {
  int best = INF;
  for (int i = 1; i <= n; i++) {
    for (int j = 1; j <= n; j++) {
      int reduced = costM[i][j] - u[i] - v[j];
      if (reduced < best) best = reduced;
    }
  }
  return best;
}

static string matchingText() {
  string pairs;
  for (int j = 1; j <= n; j++) {
    if (p[j] == 0) continue;
    if (!pairs.empty()) pairs += ", ";
    pairs += "R" + to_string(p[j]) + " - C" + to_string(j);
  }
  return pairs;
}

static void runHungarian() {
  if (n == 0) {
    cout << "Build a cost matrix first.\n";
    return;
  }

  clearSolution();
  traceShown = 0;

  cout << "\n--- Hungarian method, one row at a time ---\n";
  cout << "Every row keeps a potential u and every column a potential v. The\n";
  cout << "reduced cost of a cell is cost - u[row] - v[column], and the method\n";
  cout << "keeps every reduced cost at zero or above. Adding a row grows a tree\n";
  cout << "of columns, and the smallest reduced cost in the tree is subtracted\n";
  cout << "from every column, which pulls one more column into reach.\n";

  for (int i = 1; i <= n; i++) {
    say("Add row R" + to_string(i) + ".");
    p[0] = i;
    int j0 = 0;
    for (int j = 0; j <= n; j++) {
      minv[j] = INF;
      used[j] = false;
    }

    do {
      used[j0] = true;
      int i0 = p[j0];
      int delta = INF;
      int j1 = 0;

      string reduced;
      for (int j = 1; j <= n; j++) {
        if (used[j]) continue;
        int cur = costM[i0][j] - u[i0] - v[j];
        if (cur < minv[j]) {
          minv[j] = cur;
          way[j] = j0;
        }
        if (minv[j] < delta) {
          delta = minv[j];
          j1 = j;
        }
        if (!reduced.empty()) reduced += ", ";
        reduced += "C" + to_string(j) + "=" + to_string(cur);
      }
      say("  Reduced costs from R" + to_string(i0) + ": " + reduced + ".");

      for (int j = 0; j <= n; j++) {
        if (used[j]) {
          u[p[j]] += delta;
          v[j] -= delta;
        } else {
          minv[j] -= delta;
        }
      }

      say("  Delta = " + to_string(delta) + " at C" + to_string(j1) +
          ", so tree rows gain it and tree columns lose it.");
      if (p[j1] != 0) {
        say("  C" + to_string(j1) + " is held by R" + to_string(p[j1]) +
            ", so that row joins the tree.");
      } else {
        say("  C" + to_string(j1) + " is free, so the walk can stop here.");
      }
      j0 = j1;
    } while (p[j0] != 0);

    do {
      int j1 = way[j0];
      p[j0] = p[j1];
      j0 = j1;
    } while (j0 != 0);

    say("  Matching now: " + matchingText() + ".");
  }

  for (int j = 1; j <= n; j++) assignRow[p[j]] = j;
  totalCost = 0;
  for (int i = 1; i <= n; i++) totalCost += costM[i][assignRow[i]];

  cout << "\n--- Assignment ---\n";
  for (int i = 1; i <= n; i++) {
    cout << "  R" << i << " - C" << assignRow[i] << " costs "
         << costM[i][assignRow[i]] << ".\n";
  }
  cout << "Total cost: " << totalCost << ".\n";

  int greedy = greedyTotal();
  int brute = bruteOptimum();
  int perms = 1;
  for (int k = 2; k <= n; k++) perms *= k;
  cout << "Greedy, cheapest free column per row: " << greedy << ".\n";
  cout << "Brute force over " << plural(perms, "permutation", "permutations")
       << ": " << brute << ".\n";
  if (greedy == totalCost && brute == totalCost) {
    cout << "Greedy also found the best here, and the Hungarian total matches\n";
    cout << "the brute-force optimum.\n";
  } else if (brute == totalCost) {
    cout << "The Hungarian total matches the brute-force optimum, and greedy\n";
    cout << "would have paid " << (greedy - totalCost) << " more.\n";
  } else {
    cout << "The Hungarian total differs from the brute-force optimum, which\n";
    cout << "would mean a bug in the method.\n";
  }

  int smallest = smallestReduced();
  cout << "Smallest reduced cost: " << smallest << ".";
  if (smallest >= 0) {
    cout << " Every reduced cost is zero or above.\n";
  } else {
    cout << " A negative value would mean the potentials are wrong.\n";
  }
  cout << "The potentials make each tree step safe: A column becomes free\n";
  cout << "exactly when no cheaper reassignment is left, so the assignment is\n";
  cout << "a minimum-cost perfect matching.\n";
  if (traceShown >= TRACE_LIMIT) {
    cout << "(Trace stops after " << TRACE_LIMIT
         << " lines. The method itself ran to the end.)\n";
  }
  solved = true;
}

static void showMatrix() {
  cout << "\n--- Cost matrix ---\n";
  if (n == 0) {
    cout << "(Empty)\n";
    return;
  }
  cout << "      ";
  for (int j = 1; j <= n; j++) cout << setw(5) << ("C" + to_string(j));
  cout << "\n";
  for (int i = 1; i <= n; i++) {
    cout << setw(5) << ("R" + to_string(i)) << " ";
    for (int j = 1; j <= n; j++) cout << setw(5) << costM[i][j];
    cout << "\n";
  }
  cout << "Rows are workers and columns are jobs, or the other way round. A\n";
  cout << "complete assignment uses every row and every column once.\n";
}

static void showAssignment() {
  cout << "\n--- Current assignment ---\n";
  if (!solved) {
    cout << "(Not solved yet)\n";
    cout << "Run the Hungarian method first, or the listing would be stale.\n";
    return;
  }
  for (int i = 1; i <= n; i++) {
    cout << "  R" << i << " - C" << assignRow[i] << " costs "
         << costM[i][assignRow[i]] << ".\n";
  }
  cout << "Total cost: " << totalCost << ".\n";
}

static void buildMatrix() {
  int fresh = readRange("Number of rows and columns (1..8): ", 1, HMAX);
  resetMatrix();
  n = fresh;

  cout << "Enter " << n << " costs per row, each 0..99.\n";
  for (int i = 1; i <= n; i++) {
    cout << "Row R" << i << "\n";
    for (int j = 1; j <= n; j++) {
      costM[i][j] = readRange("  Cost for C" + to_string(j) + " (0..99): ",
                              0, 99);
    }
  }

  cout << "\nBuild complete: " << n << " rows, " << n << " columns.\n";
}

static void loadGreedyTrap() {
  static const int cells[HMAX][HMAX] = {
      {1, 5, 3},
      {2, 1, 9},
      {9, 4, 8},
  };
  setMatrix(cells, 3);
  cout << "Greedy trap demo: 3 rows, 3 columns.\n";
  cout << "  1 5 3 / 2 1 9 / 9 4 8, read row by row.\n";
  cout << "  Taking the cheapest free column per row gives R1-C1, R2-C2 and\n";
  cout << "  then R3-C3 for 10. The method instead moves R1 to C3, so R2 takes\n";
  cout << "  C1 and R3 takes C2, for 9.\n\n";

  cout << "Demo loaded: 3 rows, 3 columns.\n";
}

static void loadFourDemo() {
  static const int cells[HMAX][HMAX] = {
      {7, 4, 9, 2},
      {5, 8, 3, 6},
      {9, 6, 1, 7},
      {3, 7, 4, 8},
  };
  setMatrix(cells, 4);
  cout << "Four-by-four demo: 4 rows, 4 columns.\n";
  cout << "  7 4 9 2 / 5 8 3 6 / 9 6 1 7 / 3 7 4 8, read row by row.\n";
  cout << "  The cheapest free column in each row happens to be part of the\n";
  cout << "  best assignment here, so greedy and the method agree. The trace\n";
  cout << "  still shows the trees and the potential changes.\n\n";

  cout << "Demo loaded: 4 rows, 4 columns.\n";
}

static void changeCost() {
  if (n == 0) {
    cout << "Build a cost matrix first.\n";
    return;
  }

  while (true) {
    int row = readInt("  Row (1.." + to_string(n) + ", -1 to cancel): ");
    if (row == -1) {
      cout << "  Cancelled.\n";
      return;
    }
    if (row < 1 || row > n) {
      cout << "  Rejected: Rows must be in 1.." << n << ".\n";
      continue;
    }
    int col = readInt("  Column (1.." + to_string(n) + ", -1 to cancel): ");
    if (col == -1) {
      cout << "  Cancelled.\n";
      return;
    }
    if (col < 1 || col > n) {
      cout << "  Rejected: Columns must be in 1.." << n << ".\n";
      continue;
    }
    int fresh = readInt("  New cost (0..99, -1 to cancel): ");
    if (fresh == -1) {
      cout << "  Cancelled.\n";
      return;
    }
    if (fresh < 0 || fresh > 99) {
      cout << "  Rejected: Costs must be in 0..99.\n";
      continue;
    }

    int before = costM[row][col];
    costM[row][col] = fresh;
    clearSolution();

    cout << "\nChanged R" << row << "-C" << col << " from " << before << " to "
         << fresh << ".\n";
    cout << "  Run the method again, because every total may have moved.\n";
    return;
  }
}

static void zeroMatrix() {
  if (n == 0) {
    cout << "Build a cost matrix first.\n";
    return;
  }
  for (int i = 1; i <= n; i++) {
    for (int j = 1; j <= n; j++) costM[i][j] = 0;
  }
  clearSolution();

  cout << "\nEvery cost is now zero.\n";
  cout << "  Any assignment costs nothing, so every permutation is optimal.\n";
  cout << "  The method will still build its trees, and the potentials stay\n";
  cout << "  at zero because every reduced cost is already zero.\n";
}

static void costMenu() {
  while (true) {
    cout << "\n--- Cost operations ---\n";
    cout << "Matrix: " << n << " rows, " << n << " columns\n";
    cout << "1. Change one cost\n";
    cout << "2. Set every cost to zero\n";
    cout << "0. Back to the main menu\n";

    int choice = readInt("Select: ");

    if (choice == 1) {
      changeCost();
    } else if (choice == 2) {
      zeroMatrix();
    } else if (choice == 0) {
      return;
    } else {
      cout << "Invalid option.\n";
    }
  }
}

int main() {
  resetMatrix();

  while (true) {
    cout << "\n=== G54: The Hungarian Method (Assignment Problem) ===\n";
    cout << "Matrix: " << n << " rows, " << n << " columns\n";
    cout << "1. Build a cost matrix\n";
    cout << "2. Load the greedy trap demo (3 by 3)\n";
    cout << "3. Load the four-by-four demo\n";
    cout << "4. Cost operations\n";
    cout << "5. Run the Hungarian method\n";
    cout << "6. Show the cost matrix\n";
    cout << "7. Show the current assignment\n";
    cout << "8. Exit\n";

    int choice = readInt("Select: ");

    if (choice == 1) {
      buildMatrix();
    } else if (choice == 2) {
      loadGreedyTrap();
    } else if (choice == 3) {
      loadFourDemo();
    } else if (choice == 4) {
      costMenu();
    } else if (choice == 5) {
      runHungarian();
    } else if (choice == 6) {
      showMatrix();
    } else if (choice == 7) {
      showAssignment();
    } else if (choice == 8) {
      cout << "Exiting...\n";
      break;
    } else {
      cout << "Invalid option.\n";
    }
  }

  return 0;
}
