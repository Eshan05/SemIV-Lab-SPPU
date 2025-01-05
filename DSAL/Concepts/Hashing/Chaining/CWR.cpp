#include <iostream>
#include <list>
using namespace std;

const int sizeT = 10;
int hFn(int k) { return k % sizeT; }

void createHT(list<int> hT[]) {
  for (int i = 0; i < sizeT; i++)
    hT[i].clear();
}

void insert(list<int> hT[], int k) {
  int idx = hFn(k);
  if (hT[idx].empty()) {
    hT[idx].push_back(k);
    cout << "Inserted key " << k << " at index " << idx << endl;
  } else {
    int replacedKey = hT[idx].front();
    hT[idx].pop_front();
    hT[idx].push_back(k);
    cout << "Inserted key " << k << " at index " << idx << " (Replaced  " << replacedKey << ")" << endl;
    insert(hT, replacedKey);
  }
}

void find(list<int> hT[], int k) {
  int idx = hFn(k);
  auto &chain = hT[idx];
  for (auto it = chain.begin(); it != chain.end(); it++) {
    if (*it == k) {
      cout << "Found key " << k << " at index " << idx << endl;
      return;
    }
    cout << "Key not found";
  }
}

void deleteK(list<int> hT[], int k) {
  int idx = hFn(k);
  auto &chain = hT[idx];
  for (auto it = chain.begin(); it != chain.end(); it++) {
    if (*it == k) {
      chain.erase(it);
      cout << "Deleted key " << k << " from index " << idx << endl;
      return;
    }
  }
  cout << "Key not found";
}

void displayHT(list<int> hT[]) {
  cout << "Hash Table:" << endl;
  for (int i = 0; i < sizeT; i++) {
    cout << i << ": ";
    if (hT[i].empty()) {
      cout << "Empty" << endl;
    } else {
      for (int value : hT[i]) cout << value << " ";
      cout << endl;
    }
  }
}

int main() {
  list<int> hT[sizeT];
  int ch, k;
  createHT(hT);
  do {
    cout << "\nMenu: " << endl;
    cout << "1. Insert a key\n2. Find a key\n3. Delete a key\n4. Display hash table\n5. Exit\nEnter your choice: ";
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
      case 4: displayHT(hT); break;
      case 5: cout << "Exiting...\n"; break;
      default: cout << "Invalid choice\n";
    }
  } while (ch != 5);
  return 0;
}