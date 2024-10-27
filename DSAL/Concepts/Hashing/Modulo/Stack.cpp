#include <iostream>
#include <stack>
using namespace std;

const int Tsize = 10;

int hFn(int k) { return k % Tsize; }

void createHT(stack<int> hT[]) {
  for (int i = 0; i < Tsize; i++)
    while (!hT[i].empty()) hT[i].pop();
}

void insert(stack<int> hT[], int k) {
  int idx = hFn(k);
  hT[idx].push(k);
  cout << "Inserted " << k << " at index " << idx << endl;
}

void find(stack<int> hT[], int k) {
  int idx = hFn(k);
  stack<int> t = hT[idx];
  bool found = false;
  while (!t.empty()) {
    if (t.top() == k) {
      found = true;
      break;
    }
    t.pop();
  }
  if (found) cout << "Found " << k << " at index " << idx << endl;
  else cout << "Key not found" << endl;
}

void deleteK(stack<int> hT[], int k) {
  int idx = hFn(k);
  stack<int> t = hT[idx];
  bool deleted = false;

  while (!hT[idx].empty()) {
    if (hT[idx].top() == k) {
      hT[idx].pop();
      deleted = true;
      break;
    }
    t.push(hT[idx].top());
    hT[idx].pop();
  }

  while (!t.empty()) {
    hT[idx].push(t.top());
    t.pop();
  }

  if (deleted) cout << "Deleted " << k << " at index " << idx << endl;
  else cout << "Key not found" << endl;
}

void displayHT(stack<int> hT[]) {
  cout << "Hash Table: " << endl;
  for (int i = 0; i < Tsize; i++) {
    cout << i << ": ";
    if (hT[i].empty()) cout << "Empty" << endl;
    else {
      stack<int> t = hT[i];
      while (!t.empty()) {
        cout << t.top() << " ";
        t.pop();
      }
      cout << "\n";
    }
  }
}

int main() {
  stack<int> hashT[Tsize];
  int ch, k;
  createHT(hashT);

  do {
    cout << "\n-> HT Modulo Stack <-: " << endl;
    cout << "1. Insert a key" << endl;
    cout << "2. Find a key" << endl;
    cout << "3. Delete a key" << endl;
    cout << "4. Display hash table" << endl;
    cout << "5. Exit" << endl;
    cout << "Enter your choice: ";
    cin >> ch;

    switch (ch) {
      case 1:
        cout << "Enter key to insert: ";
        cin >> k;
        insert(hashT, k);
        break;

      case 2:
        cout << "Enter key to find: ";
        cin >> k;
        find(hashT, k);
        break;

      case 3:
        cout << "Enter key to delete: ";
        cin >> k;
        deleteK(hashT, k);
        break;

      case 4:
        displayHT(hashT);
        break;

      case 5:
        cout << "Exiting..." << endl;
        break;

      default:
        cout << "Invalid choice" << endl;
    }
  } while (ch != 5);
  return 0;
}