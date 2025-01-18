// 24a. Segment Tree (range sum)
// Demo: construction, sum query, update query, display (basic).

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

struct SegTree {
  int n;
  long long *seg;
  int size;
  int *arr;

  SegTree() : n(0), seg(NULL), size(0), arr(NULL) {}
  ~SegTree() {
    delete[] seg;
    delete[] arr;
  }

  void init(int N) {
    delete[] seg;
    delete[] arr;
    n = N;
    if (n < 0) n = 0;
    arr = (n > 0) ? new int[n] : NULL;
    size = 1;
    while (size < n) size *= 2;
    seg = (n > 0) ? new long long[2 * size] : NULL;
    if (seg) {
      for (int i = 0; i < 2 * size; i++) seg[i] = 0;
    }
  }

  void build() {
    if (n <= 0) return;
    for (int i = 0; i < n; i++) {
      seg[size + i] = arr[i];
      cout << "leaf node " << size + i << " = a[" << i << "]\n";
    }
    for (int i = size - 1; i >= 1; i--) {
      seg[i] = seg[2 * i] + seg[2 * i + 1];
      cout << "internal node " << i << " = left+right = " << seg[i] << "\n";
    }
  }

  bool updateSet(int idx, int val) {
    if (idx < 0 || idx >= n) {
      cout << "Index out of range.\n";
      return false;
    }
    arr[idx] = val;
    int p = size + idx;
    seg[p] = val;
    cout << "update leaf node " << p << "\n";
    p /= 2;
    while (p >= 1) {
      seg[p] = seg[2 * p] + seg[2 * p + 1];
      cout << "recompute node " << p << " -> " << seg[p] << "\n";
      p /= 2;
    }
    return true;
  }

  long long querySum(int l, int r) const {
    if (n <= 0) return 0;
    int oldL = l;
    int oldR = r;
    if (l < 0) l = 0;
    if (r >= n) r = n - 1;
    if (oldL != l || oldR != r) {
      cout << "Clamp query [" << oldL << "," << oldR << "] to ["
           << l << "," << r << "]\n";
    }
    if (l > r) {
      cout << "Empty query range after clamping.\n";
      return 0;
    }
    long long res = 0;
    int L = size + l;
    int R = size + r;
    while (L <= R) {
      if ((L % 2) == 1) {
        cout << "take node " << L << " sum=" << seg[L] << "\n";
        res += seg[L++];
      }
      if ((R % 2) == 0) {
        cout << "take node " << R << " sum=" << seg[R] << "\n";
        res += seg[R--];
      }
      L /= 2;
      R /= 2;
    }
    return res;
  }

  void display() const {
    cout << "Array: ";
    for (int i = 0; i < n; i++) cout << arr[i] << " ";
    cout << "\n";
    cout << "SegTree(root sum)=" << (n > 0 ? seg[1] : 0) << "\n";
  }
};

int main() {
  SegTree st;
  bool created = false;
  int choice = 0;
  while (choice != 7) {
    cout << "\n=== T27: Segment Tree (Range Sum) ===\n";
    cout << "1. Create / Reset with n and values\n";
    cout << "2. Range sum query [l..r]\n";
    cout << "3. Point update: set a[i] = value\n";
    cout << "4. Display\n";
    cout << "5. Show internal size\n";
    cout << "6. Clear\n";
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
      st.init(n);
      created = true;
      for (int i = 0; i < n; i++) st.arr[i] = readInt(string("a[") + to_string(i) + "]: ");
      st.build();
      cout << "Built.\n";
    } else if (choice == 2) {
      if (!created) {
        cout << "Create/reset first (option 1).\n";
        continue;
      }
      int l = readInt("l: ");
      int r = readInt("r: ");
      long long ans = st.querySum(l, r);
      cout << "Sum=" << ans << "\n";
    } else if (choice == 3) {
      if (!created) {
        cout << "Create/reset first (option 1).\n";
        continue;
      }
      int i = readInt("index i: ");
      int v = readInt("value: ");
      if (st.updateSet(i, v)) cout << "Updated.\n";
    } else if (choice == 4) {
      if (!created) {
        cout << "Create/reset first (option 1).\n";
        continue;
      }
      st.display();
    } else if (choice == 5) {
      if (!created) {
        cout << "Create/reset first (option 1).\n";
        continue;
      }
      cout << "n=" << st.n << ", base size(power2)=" << st.size << ", seg array size=" << (st.n > 0 ? 2 * st.size : 0) << "\n";
    } else if (choice == 6) {
      st.init(0);
      created = false;
      cout << "Cleared.\n";
    } else if (choice == 7) {
      cout << "Bye.\n";
    } else {
      cout << "Invalid option.\n";
    }
  }
  return 0;
}
