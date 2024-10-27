#include <iostream>
#include <map>
using namespace std;

const int Tsize = 10;

int hFn(int k) { return k % Tsize; }

void createHT(map<int, int> hT[]) {
  for (int i = 0; i < Tsize; i++)
    hT[i].clear();
}

void insert(map<int, int> hT[], int k) {
  int idx = hFn(k);
  hT[idx][k] = k;
  cout << "Inserted" << k << " at index " << idx << endl;
}

void find(map<int, int> hT[], int k) {
  int idx = hFn(k);
  auto it = hT[idx].find(k);
  if (it != hT[idx].end())
    cout << "Found " << k << " at index " << idx << endl;
  else
    cout << "Key not found";
}

void deleteK(map<int, int> hT[], int k) {
  int idx = hFn(k);
  auto it = hT[idx].find(k);
  if (it != hT[idx].end())
    cout << "Deleted " << k << " from index " << idx << endl;
  else
    cout << "Key not found";
}

void displayHT(map<int, int> hT[]) {
  cout << "Hash table:\n";
  for (int i = 0; i < Tsize; i++) {
    cout << i << ": ";
    if (hT[i].empty()) cout << "Empty\n";
    else {
      for (auto &pair : hT[i]) cout << pair.first << " ";
      cout << endl;
    }
  }
}

int main() {
  map<int, int> hT[Tsize];
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
      default: cout << "Invalid choice.\n";
    }
  } while (ch != 5);
  return 0;
}
