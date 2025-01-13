// 22. Heaps
// Demo: binary MAX-heap in an array.
// Supports: insertion, deletion(extract max), heapify, build-heap, heap sort, display, height, #nodes.

#include <iostream>
using namespace std;

struct Heap {
  int *a;
  int n;
  int cap;

  Heap() : a(NULL), n(0), cap(0) {}
  ~Heap() { delete[] a; }

  void ensure(int need) {
    if (need <= cap) return;
    int nc = (cap == 0) ? 16 : cap;
    while (nc < need) nc *= 2;
    int *b = new int[nc];
    for (int i = 0; i < n; i++) b[i] = a[i];
    delete[] a;
    a = b;
    cap = nc;
  }

  void swap2(int &x, int &y) {
    int t = x;
    x = y;
    y = t;
  }

  void siftUp(int i) {
    while (i > 0) {
      int p = (i - 1) / 2;
      cout << "siftUp: compare child " << a[i] << " with parent " << a[p] << "\n";
      if (a[p] >= a[i]) break;
      cout << "siftUp: swap child " << a[i] << " with parent " << a[p] << "\n";
      swap2(a[p], a[i]);
      i = p;
    }
  }

  void siftDown(int i) {
    while (true) {
      int l = 2 * i + 1;
      int r = 2 * i + 2;
      int m = i;
      if (l < n) cout << "siftDown: compare " << a[i] << " with left child " << a[l] << "\n";
      if (l < n && a[l] > a[m]) m = l;
      if (r < n) cout << "siftDown: compare current largest " << a[m] << " with right child " << a[r] << "\n";
      if (r < n && a[r] > a[m]) m = r;
      if (m == i) break;
      cout << "siftDown: swap " << a[i] << " with larger child " << a[m] << "\n";
      swap2(a[i], a[m]);
      i = m;
    }
  }

  void insert(int x) {
    ensure(n + 1);
    a[n] = x;
    siftUp(n);
    n++;
  }

  bool extractMax(int &out) {
    if (n == 0) return false;
    out = a[0];
    cout << "remove root max " << out << "\n";
    n--;
    if (n > 0) {
      cout << "move last value " << a[n] << " to root, then siftDown\n";
      a[0] = a[n];
      siftDown(0);
    }
    return true;
  }

  void heapifyAt(int i) {
    if (i < 0 || i >= n) {
      cout << "Index out of range.\n";
      return;
    }
    siftDown(i);
  }

  void buildHeap() {
    for (int i = n / 2 - 1; i >= 0; i--) {
      cout << "heapify index " << i << "\n";
      siftDown(i);
    }
  }

  int height() const {
    // height in levels for n nodes: floor(log2(n))+1
    if (n == 0) return 0;
    int h = 0;
    int nodes = n;
    while (nodes > 0) {
      h++;
      nodes = nodes / 2;
    }
    return h;
  }

  void displayArray() const {
    cout << "Array: ";
    for (int i = 0; i < n; i++) cout << a[i] << " ";
    cout << "\n";
  }

  void displayLevels() const {
    if (n == 0) {
      cout << "(empty)\n";
      return;
    }
    cout << "\nHeap as levels:\n";
    int idx = 0;
    int levelCount = 1;
    int level = 0;
    while (idx < n) {
      level++;
      cout << "L" << level << ": ";
      for (int j = 0; j < levelCount && idx < n; j++) {
        cout << a[idx++] << " ";
      }
      cout << "\n";
      levelCount *= 2;
    }
  }

  void clear() {
    n = 0;
  }
};

static void clearInput() {
  cin.clear();
  cin.ignore(1024, '\n');
}

static int readInt(const string &prompt) {
  int x;
  while (true) {
    cout << prompt;
    if (cin >> x) return x;
    cout << "Invalid number.\n";
    clearInput();
  }
}

int main() {
  Heap hp;
  int choice = 0;
  while (choice != 10) {
    cout << "\n=== T25: MAX Heap (Array) ===\n";
    cout << "1. Insert\n";
    cout << "2. Extract max\n";
    cout << "3. Heapify at index\n";
    cout << "4. Build heap (from current array)\n";
    cout << "5. Heap sort (show result)\n";
    cout << "6. Display array\n";
    cout << "7. Display levels\n";
    cout << "8. Height / #Nodes\n";
    cout << "9. Clear\n";
    cout << "10. Exit\n";
    cout << "Select: ";
    cin >> choice;
    if (!cin) {
      clearInput();
      choice = 0;
    }

    if (choice == 1) {
      int x = readInt("Value: ");
      hp.insert(x);
      cout << "Inserted.\n";
    } else if (choice == 2) {
      int mx;
      if (hp.extractMax(mx)) cout << "Extracted max=" << mx << "\n";
      else cout << "Heap empty.\n";
    } else if (choice == 3) {
      int i = readInt("Index (0-based): ");
      hp.heapifyAt(i);
    } else if (choice == 4) {
      hp.buildHeap();
      cout << "Build-heap done.\n";
    } else if (choice == 5) {
      if (hp.n == 0) {
        cout << "Empty.\n";
        continue;
      }
      // Copy and sort
      Heap tmp;
      tmp.ensure(hp.n);
      tmp.n = hp.n;
      for (int i = 0; i < hp.n; i++) tmp.a[i] = hp.a[i];
      tmp.buildHeap();
      cout << "Heap sort (ascending): ";
      int m;
      // Extract max repeatedly -> descending, store then print reverse
      int *sorted = new int[tmp.n];
      int cnt = 0;
      while (tmp.extractMax(m)) sorted[cnt++] = m;
      cout << "Extracted max values are descending; print reverse for ascending.\n";
      for (int i = cnt - 1; i >= 0; i--) cout << sorted[i] << " ";
      cout << "\n";
      delete[] sorted;
    } else if (choice == 6) {
      hp.displayArray();
    } else if (choice == 7) {
      hp.displayLevels();
    } else if (choice == 8) {
      cout << "#Nodes=" << hp.n << ", Height(levels)=" << hp.height() << "\n";
    } else if (choice == 9) {
      hp.clear();
      cout << "Cleared.\n";
    } else if (choice == 10) {
      cout << "Bye.\n";
    } else {
      cout << "Invalid option.\n";
    }
  }
  return 0;
}
