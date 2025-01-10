// 22. Read the marks obtained by students of second year in an online examination of particular subject. Find out maximum and minimum marks obtained in that subject. Use heap data structure. Analyze the algorithm

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

void buildMaxHeap(int a[], int n) {
  for (int i = n / 2 - 1; i >= 0; --i) heapifyMax(a, n, i);
}

void heapifyMin(int a[], int n, int i) {
  int smallest = i;
  int l = 2 * i + 1;
  int r = 2 * i + 2;
  if (l < n && a[l] < a[smallest]) smallest = l;
  if (r < n && a[r] < a[smallest]) smallest = r;
  if (smallest != i) {
    int t = a[i];
    a[i] = a[smallest];
    a[smallest] = t;
    heapifyMin(a, n, smallest);
  }
}

void buildMinHeap(int a[], int n) {
  for (int i = n / 2 - 1; i >= 0; --i) heapifyMin(a, n, i);
}

int main() {
  int marks[MAXN];
  int n = 0;
  bool loaded = false;

  while (true) {
    cout << "\n===== Group E22 (Min/Max Marks using Heap) =====\n";
    cout << "1. Input marks\n";
    cout << "2. Display marks\n";
    cout << "3. Find maximum (using max-heap)\n";
    cout << "4. Find minimum (using min-heap)\n";
    cout << "5. Find both max & min\n";
    cout << "6. Complexity note\n";
    cout << "7. Exit\n";

    int ch = readInt("Enter choice: ");
    if (ch == 1) {
      n = readInt("Enter number of students (1-200): ");
      if (n < 1) {
        cout << "Need at least 1 student.\n";
        n = 0;
        loaded = false;
        continue;
      }
      if (n > MAXN) {
        cout << "Clamping to MAXN=" << MAXN << "\n";
        n = MAXN;
      }
      for (int i = 0; i < n; ++i) {
        int m = readInt("Marks of student " + to_string(i + 1) + ": ");
        marks[i] = m;
      }
      loaded = true;
      cout << "Marks loaded.\n";
    } else if (ch == 2) {
      if (!loaded) cout << "Input marks first.\n";
      else displayArray(marks, n);
    } else if (ch == 3) {
      if (!loaded) {
        cout << "Input marks first.\n";
        continue;
      }
      int heap[MAXN];
      copyArray(marks, heap, n);
      buildMaxHeap(heap, n);
      cout << "Maximum marks = " << heap[0] << "\n";
    } else if (ch == 4) {
      if (!loaded) {
        cout << "Input marks first.\n";
        continue;
      }
      int heap[MAXN];
      copyArray(marks, heap, n);
      buildMinHeap(heap, n);
      cout << "Minimum marks = " << heap[0] << "\n";
    } else if (ch == 5) {
      if (!loaded) {
        cout << "Input marks first.\n";
        continue;
      }
      int heapMax[MAXN];
      int heapMin[MAXN];
      copyArray(marks, heapMax, n);
      copyArray(marks, heapMin, n);
      buildMaxHeap(heapMax, n);
      buildMinHeap(heapMin, n);
      cout << "Maximum marks = " << heapMax[0] << "\n";
      cout << "Minimum marks = " << heapMin[0] << "\n";
    } else if (ch == 6) {
      cout << "\nAnalysis\n";
      cout << "Build heap: O(n)\n";
      cout << "Find max/min from heap root: O(1)\n";
      cout << "Space used by heap array: O(n)\n";
    } else if (ch == 7) {
      break;
    } else {
      cout << "Invalid choice.\n";
    }
  }

  return 0;
}
