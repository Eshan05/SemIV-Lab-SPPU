#include <iostream>
#include <queue>
using namespace std;

const int Tsize = 10;

int hFn(int k) { return k % Tsize; }

void createHT(queue<int> hT[]) {
  for (int i = 0; i < Tsize; i++)
    while (!hT[i].empty()) hT[i].pop();
}

void insert(queue<int> hT[], int k) {
  int idx = hFn(k);
  hT[idx].push(k);
  cout << "Inserted " << k << " at index " << idx << endl;
}

void find(queue<int> hT[], int k) {
  int idx = hFn(k);
  queue<int> t = hT[idx];
  bool found = false;

  while (!t.empty()) {
    if (t.front() == k) {
      found = true;
      break;
    }
    t.pop();
  }

  if (found) cout << "Found " << k << " at index " << idx << endl;
  else cout << "Key not found" << endl;
}

void deleteK(queue<int> hT[], int k) {
  int idx = hFn(k);
  queue<int> t;
  bool deleted = false;

  while (!hT[idx].empty()) {
    if (hT[idx].front() == k && !deleted) {
      hT[idx].pop();
      deleted = true;
    } else {
      t.push(hT[idx].front());
      hT[idx].pop();
    }
  }

  while (!t.empty()) {
    hT[idx].push(t.front());
    t.pop();
  }

  if (deleted) cout << "Deleted " << k << " at index " << idx << endl;
  else cout << "Key not found" << endl;
}

void displayHT(queue<int> hT[]) {
  cout << "Hash Table:" << endl;
  for (int i = 0; i < Tsize; i++) {
    cout << i << ": ";
    if (hT[i].empty()) {
      cout << "Empty" << endl;
    } else {
      queue<int> t = hT[i];
      while (!t.empty()) {
        cout << t.front() << " ";
        t.pop();
      }
      cout << endl;
    }
  }
}

int main() {
  queue<int> hT[Tsize];
  int ch, k;
  createHT(hT);
  do {
    cout << "\n-> HT Modulo Queue <-: " << endl;
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
        insert(hT, k);
        break;

      case 2:
        cout << "Enter key to find: ";
        cin >> k;
        find(hT, k);
        break;

      case 3:
        cout << "Enter key to delete: ";
        cin >> k;
        deleteK(hT, k);
        break;

      case 4:
        displayHT(hT);
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