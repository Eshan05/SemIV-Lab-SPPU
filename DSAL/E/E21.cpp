// 21. Implement the Heap/Shell sort algorithm implemented in Java demonstrating heap/shell data structure with modularity of programming language

#include <iostream>
#include <string>
using namespace std;

const int MAXN = 200;

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

void copyArray(const int src[], int dst[], int n) {
  for (int i = 0; i < n; ++i) dst[i] = src[i];
}

void displayArray(const int a[], int n) {
  if (n <= 0) {
    cout << "(empty)\n";
    return;
  }
  for (int i = 0; i < n; ++i) {
    cout << a[i];
    if (i + 1 < n) cout << " ";
  }
  cout << "\n";
}

void heapifyMax(int a[], int n, int i) {
  int largest = i;
  int l = 2 * i + 1;
  int r = 2 * i + 2;

  if (l < n && a[l] > a[largest]) largest = l;
  if (r < n && a[r] > a[largest]) largest = r;

  if (largest != i) {
    int t = a[i];
    a[i] = a[largest];
    a[largest] = t;
    heapifyMax(a, n, largest);
  }
}

void heapSortAsc(int a[], int n) {
  // Build max-heap
  for (int i = n / 2 - 1; i >= 0; --i) heapifyMax(a, n, i);
  // Extract max one by one
  for (int end = n - 1; end > 0; --end) {
    int t = a[0];
    a[0] = a[end];
    a[end] = t;
    heapifyMax(a, end, 0);
  }
}

void shellSortAsc(int a[], int n) {
  for (int gap = n / 2; gap > 0; gap /= 2) {
    for (int i = gap; i < n; ++i) {
      int temp = a[i];
      int j = i;
      while (j >= gap && a[j - gap] > temp) {
        a[j] = a[j - gap];
        j -= gap;
      }
      a[j] = temp;
    }
  }
}

int main() {
  int data[MAXN];
  int n = 0;
  bool loaded = false;

  while (true) {
    cout << "\n===== Group E21 (Heap Sort / Shell Sort) =====\n";
    cout << "1. Input array\n";
    cout << "2. Display array\n";
    cout << "3. Heap sort (ascending)\n";
    cout << "4. Shell sort (ascending)\n";
    cout << "5. Exit\n";

    int ch = readInt("Enter choice: ");
    if (ch == 1) {
      n = readInt("Enter number of elements (1-200): ");
      if (n < 1) {
        cout << "Need at least 1 element.\n";
        n = 0;
        loaded = false;
        continue;
      }
      if (n > MAXN) {
        cout << "Clamping to MAXN=" << MAXN << "\n";
        n = MAXN;
      }
      for (int i = 0; i < n; ++i) {
        data[i] = readInt("a[" + to_string(i) + "] = ");
      }
      loaded = true;
      cout << "Array loaded.\n";
    } else if (ch == 2) {
      if (!loaded) cout << "Input array first.\n";
      else displayArray(data, n);
    } else if (ch == 3) {
      if (!loaded) {
        cout << "Input array first.\n";
        continue;
      }
      int work[MAXN];
      copyArray(data, work, n);
      heapSortAsc(work, n);
      cout << "Sorted (Heap sort): ";
      displayArray(work, n);
      cout << "Time complexity: O(n log n)\n";
    } else if (ch == 4) {
      if (!loaded) {
        cout << "Input array first.\n";
        continue;
      }
      int work[MAXN];
      copyArray(data, work, n);
      shellSortAsc(work, n);
      cout << "Sorted (Shell sort): ";
      displayArray(work, n);
      cout << "Typical complexity: depends on gap sequence; often ~O(n^(3/2)) (worst-case O(n^2))\n";
    } else if (ch == 5) {
      break;
    } else {
      cout << "Invalid choice.\n";
    }
  }

  return 0;
}
