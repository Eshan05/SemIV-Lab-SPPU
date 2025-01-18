// 24b. Fenwick Tree (BIT)
// Demo: construction, prefix sum query, range sum query, point update.

#include <iostream>
using namespace std;

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

static long long readLongLong(const string &prompt) {
  long long x;
  while (true) {
    cout << prompt;
    if (cin >> x) return x;
    cout << "Invalid number.\n";
    clearInput();
  }
}

struct BIT {
  int n;
  long long *bit;
  long long *arr;
  BIT() : n(0), bit(NULL), arr(NULL) {}
  ~BIT() {
    delete[] bit;
    delete[] arr;
  }

  void init(int N) {
    delete[] bit;
    delete[] arr;
    n = N;
    if (n < 0) n = 0;
    bit = (n > 0) ? new long long[n + 1] : NULL;
    arr = (n > 0) ? new long long[n + 1] : NULL;
    for (int i = 0; i <= n; i++) {
      if (bit) bit[i] = 0;
      if (arr) arr[i] = 0;
    }
  }

  void add(int i, long long delta) {
    for (; i <= n; i += (i & -i)) {
      cout << "update BIT[" << i << "] += " << delta << "\n";
      bit[i] += delta;
    }
  }

  long long sumPrefix(int i) const {
    int old = i;
    if (i > n) i = n;
    if (i < 0) i = 0;
    if (old != i) cout << "Clamp prefix index " << old << " to " << i << "\n";
    long long s = 0;
    for (; i > 0; i -= (i & -i)) {
      cout << "read BIT[" << i << "] -> " << bit[i] << "\n";
      s += bit[i];
    }
    return s;
  }

  long long sumRange(int l, int r) const {
    if (l > r) {
      cout << "Empty range because l > r.\n";
      return 0;
    }
    int oldL = l;
    int oldR = r;
    if (l < 1) l = 1;
    if (r > n) r = n;
    if (oldL != l || oldR != r) {
      cout << "Clamp range [" << oldL << "," << oldR << "] to ["
           << l << "," << r << "]\n";
    }
    if (l > r) {
      cout << "Empty range after clamping.\n";
      return 0;
    }
    return sumPrefix(r) - sumPrefix(l - 1);
  }

  void buildFromInput() {
    for (int i = 1; i <= n; i++) {
      long long v = readLongLong(string("a[") + to_string(i) + "]: ");
      arr[i] = v;
      add(i, v);
    }
  }

  bool setValue(int i, long long newVal) {
    if (i < 1 || i > n) {
      cout << "Index out of range.\n";
      return false;
    }
    long long delta = newVal - arr[i];
    cout << "set a[" << i << "] from " << arr[i] << " to " << newVal
         << " (delta " << delta << ")\n";
    arr[i] = newVal;
    add(i, delta);
    return true;
  }

  void display() const {
    cout << "Array(1..n): ";
    for (int i = 1; i <= n; i++) cout << arr[i] << " ";
    cout << "\n";
    cout << "BIT: ";
    for (int i = 1; i <= n; i++) cout << bit[i] << " ";
    cout << "\n";
  }
};

int main() {
  BIT ft;
  bool created = false;
  int choice = 0;
  while (choice != 7) {
    cout << "\n=== T28: Fenwick Tree (BIT) ===\n";
    cout << "1. Create / Reset with n and values\n";
    cout << "2. Prefix sum query (1..i)\n";
    cout << "3. Range sum query (l..r)\n";
    cout << "4. Point update: add delta\n";
    cout << "5. Point update: set value\n";
    cout << "6. Display\n";
    cout << "7. Exit\n";
    cout << "Select: ";
    cin >> choice;
    if (!cin) {
      clearInput();
      choice = 0;
    }

    if (choice == 1) {
      int n = readInt("n: ");
      if (n < 0) {
        cout << "n cannot be negative; using 0.\n";
        n = 0;
      }
      ft.init(n);
      created = true;
      ft.buildFromInput();
      cout << "Built.\n";
    } else if (choice == 2) {
      if (!created) {
        cout << "Create/reset first (option 1).\n";
        continue;
      }
      int i = readInt("i: ");
      long long ans = ft.sumPrefix(i);
      cout << "Sum(1.." << i << ")=" << ans << "\n";
    } else if (choice == 3) {
      if (!created) {
        cout << "Create/reset first (option 1).\n";
        continue;
      }
      int l = readInt("l: ");
      int r = readInt("r: ");
      long long ans = ft.sumRange(l, r);
      cout << "Sum(" << l << ".." << r << ")=" << ans << "\n";
    } else if (choice == 4) {
      if (!created) {
        cout << "Create/reset first (option 1).\n";
        continue;
      }
      int i = readInt("index i (1..n): ");
      long long d = readLongLong("delta: ");
      if (i >= 1 && i <= ft.n) {
        cout << "add delta " << d << " to a[" << i << "]\n";
        ft.arr[i] += d;
        ft.add(i, d);
        cout << "Updated.\n";
      } else {
        cout << "Out of range.\n";
      }
    } else if (choice == 5) {
      if (!created) {
        cout << "Create/reset first (option 1).\n";
        continue;
      }
      int i = readInt("index i (1..n): ");
      long long v = readLongLong("new value: ");
      if (ft.setValue(i, v)) cout << "Set done.\n";
    } else if (choice == 6) {
      if (!created) {
        cout << "Create/reset first (option 1).\n";
        continue;
      }
      ft.display();
    } else if (choice == 7) {
      cout << "Bye.\n";
    } else {
      cout << "Invalid option.\n";
    }
  }
  return 0;
}
