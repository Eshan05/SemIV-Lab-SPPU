// 26. Assume we have two input and two output tapes to perform the sorting. The internal memory can hold and sort m records at a time. Write a program in java for external sorting. Find out time complexity.

#include <fstream>
#include <iostream>
#include <string>
using namespace std;

static const char *INPUT_FILE = "F26_Input.txt";
static const char *T1 = "F26_T1.txt";
static const char *T2 = "F26_T2.txt";
static const char *T3 = "F26_T3.txt";
static const char *T4 = "F26_T4.txt";
static const char *OUTPUT_FILE = "F26_Sorted.txt";

void clearLine() {
  char c;
  while (cin.get(c)) {
    if (c == '\n') break;
  }
}

int readInt(const string &prompt) {
  int x;
  while (true) {
    cout << prompt;
    if (cin >> x) return x;
    cout << "Invalid number. Try again.\n";
    cin.clear();
    clearLine();
  }
}

bool fileHasAnyInt(const char *path) {
  ifstream in(path);
  int x;
  return static_cast<bool>(in >> x);
}

int countRecords(const char *path) {
  ifstream in(path);
  int x;
  int n = 0;
  while (in >> x) n++;
  return n;
}

void displayFile(const char *path, const string &title) {
  ifstream in(path);
  cout << "\n--- " << title << " ---\n";
  if (!in) {
    cout << "(file not found)\n";
    return;
  }
  int x;
  int shown = 0;
  while (in >> x) {
    cout << x << " ";
    shown++;
    if (shown % 20 == 0) cout << "\n";
  }
  if (shown == 0) cout << "(empty)";
  cout << "\nTotal records: " << shown << "\n";
}

void insertionSort(int arr[], int n) {
  for (int i = 1; i < n; ++i) {
    int key = arr[i];
    int j = i - 1;
    while (j >= 0 && arr[j] > key) {
      arr[j + 1] = arr[j];
      j--;
    }
    arr[j + 1] = key;
  }
}

void createInputFile() {
  int n = readInt("Enter number of records (integers): ");
  if (n < 0) n = 0;
  ofstream out(INPUT_FILE, ios::trunc);
  if (!out) {
    cout << "Unable to write input file.\n";
    return;
  }
  cout << "Enter " << n << " integers:\n";
  for (int i = 0; i < n; ++i) {
    int x = readInt("  value: ");
    out << x << "\n";
  }
  cout << "Input file created: " << INPUT_FILE << "\n";
}

void truncateFile(const char *path) {
  ofstream out(path, ios::trunc);
}

void distributeRuns(int m) {
  ifstream in(INPUT_FILE);
  if (!in) {
    cout << "Input file not found. Create it first.\n";
    return;
  }
  ofstream out1(T1, ios::trunc);
  ofstream out2(T2, ios::trunc);
  if (!out1 || !out2) {
    cout << "Unable to create tape files.\n";
    return;
  }

  int *buf = new int[m];
  bool toggle = true;
  while (true) {
    int k = 0;
    int x;
    while (k < m && (in >> x)) {
      buf[k++] = x;
    }
    if (k == 0) break;
    insertionSort(buf, k);
    ostream &out = toggle ? (ostream &)out1 : (ostream &)out2;
    for (int i = 0; i < k; ++i) out << buf[i] << "\n";
    toggle = !toggle;
    if (!in) break;
  }
  delete[] buf;
}

bool readNextInRun(ifstream &in, int runSize, int &countRead, int &value) {
  if (countRead >= runSize) return false;
  if (!(in >> value)) return false;
  countRead++;
  return true;
}

void mergePass(const char *inAPath, const char *inBPath, const char *outAPath, const char *outBPath, int runSize) {
  ifstream inA(inAPath);
  ifstream inB(inBPath);
  ofstream outA(outAPath, ios::trunc);
  ofstream outB(outBPath, ios::trunc);
  if (!outA || !outB) {
    cout << "Unable to open output tapes.\n";
    return;
  }
  bool toggle = true;

  while (true) {
    int countA = 0, countB = 0;
    int aVal = 0, bVal = 0;
    bool hasA = readNextInRun(inA, runSize, countA, aVal);
    bool hasB = readNextInRun(inB, runSize, countB, bVal);
    if (!hasA && !hasB) break;

    ostream &out = toggle ? (ostream &)outA : (ostream &)outB;
    while (hasA || hasB) {
      if (!hasB || (hasA && aVal <= bVal)) {
        out << aVal << "\n";
        hasA = readNextInRun(inA, runSize, countA, aVal);
      } else {
        out << bVal << "\n";
        hasB = readNextInRun(inB, runSize, countB, bVal);
      }
    }
    toggle = !toggle;
  }
}

void copyFile(const char *src, const char *dst) {
  ifstream in(src);
  ofstream out(dst, ios::trunc);
  int x;
  while (in >> x) out << x << "\n";
}

void externalSortMenu() {
  int n = countRecords(INPUT_FILE);
  if (n <= 0) {
    cout << "Input file is empty or missing.\n";
    return;
  }
  int m = readInt("Enter m (records that fit in memory): ");
  if (m <= 0) {
    cout << "m must be positive.\n";
    return;
  }

  cout << "\nExternal Sort (2 input + 2 output tapes)\n";
  cout << "Records n = " << n << ", Memory m = " << m << "\n";

  // 1) Initial distribution of sorted runs (size <= m) onto two input tapes.
  distributeRuns(m);

  int runSize = m;
  int mergePasses = 0;
  const char *inA = T1;
  const char *inB = T2;
  const char *outA = T3;
  const char *outB = T4;

  while (runSize < n) {
    mergePass(inA, inB, outA, outB, runSize);
    // swap tapes
    const char *tmpA = inA;
    inA = outA;
    outA = tmpA;
    const char *tmpB = inB;
    inB = outB;
    outB = tmpB;
    // clear old output (now used as output in next pass)
    truncateFile(outA);
    truncateFile(outB);
    runSize *= 2;
    mergePasses++;
  }

  const char *finalTape = fileHasAnyInt(inA) ? inA : inB;
  copyFile(finalTape, OUTPUT_FILE);
  cout << "\nSorted output written to: " << OUTPUT_FILE << "\n";

  int totalPasses = 1 + mergePasses; // initial distribution + merge passes
  cout << "Merge passes: " << mergePasses << ", Total passes (including initial): " << totalPasses << "\n";
  cout << "Time complexity (approx): O(n * log2(ceil(n/m))) merges, and O(n * passes) I/O.\n";
}

int main() {
  while (true) {
    cout << "\n===== Group F26 (External Sorting) =====\n";
    cout << "1. Create/overwrite input file\n";
    cout << "2. Display input file\n";
    cout << "3. Perform external sort\n";
    cout << "4. Display sorted output file\n";
    cout << "5. Exit\n";
    int ch = readInt("Enter choice: ");
    if (ch == 1) createInputFile();
    else if (ch == 2) displayFile(INPUT_FILE, "Input File");
    else if (ch == 3) externalSortMenu();
    else if (ch == 4) displayFile(OUTPUT_FILE, "Sorted Output");
    else if (ch == 5) break;
    else cout << "Invalid choice.\n";
  }
  return 0;
}
