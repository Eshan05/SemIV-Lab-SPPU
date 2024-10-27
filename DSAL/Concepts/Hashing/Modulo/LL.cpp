#include <iostream>
using namespace std;

const int Tsize = 10;

struct Node {
  int data;
  Node *next;
  Node(int v) : data(v), next(nullptr) {}
};

class LL {
 public:
  Node *head;
  LL() : head(nullptr) {}

  void insert(int v) {
    Node *nn = new Node(v);
    if (!head) head = nn;
    else {
      nn->next = head;
      head = nn;
    }
  }

  bool find(int v) {
    Node *c = head;
    while (c) {
      if (c->data == v) return true;
      c = c->next;
    }
    return false;
  }

  void deleteK(int v) {
    Node *c = head, *prev = nullptr;
    if (c && c->data == v) {
      head = c->next;
      delete c;
      return;
    }

    while (c && c->data != v) {
      prev = c;
      c = c->next;
    }
    if (!c) {
      cout << "The key (" << v << ") is not found" << endl;
      return;
    }

    prev->next = c->next;
    delete c;
  }

  void display() {
    Node *c = head;
    if (!c) {
      cout << "Empty" << endl;
      return;
    }

    while (c) {
      cout << c->data << " -> ";
      c = c->next;
    }
    cout << "\n";
  }
};

int hFn(int k) { return k % Tsize; }

void createHT(LL hT[]) {
  for (int i = 0; i < Tsize; i++)
    hT[i].head = nullptr;
}

void insert(LL hT[], int k) {
  int idx = hFn(k);
  hT[idx].insert(k);
  cout << "Inserted " << k << " at index " << idx << endl;
}

void find(LL hT[], int k) {
  int idx = hFn(k);
  if (hT[idx].find(k)) cout << "Found " << k << " at index " << idx << endl;
  else cout << "Key not found" << endl;
}

void deleteK(LL hT[], int k) {
  int idx = hFn(k);
  hT[idx].deleteK(k);
  cout << "Deleted " << k << " at index " << idx << endl;
}

void displayHT(LL hT[]) {
  cout << "Hash Table:" << endl;
  for (int i = 0; i < Tsize; i++) {
    cout << i << ": ";
    hT[i].display();
  }
}

int main() {
  LL hT[Tsize];
  int ch, key;
  createHT(hT);

  do {
    cout << " -> HT Modulo Linked List <-: " << endl;
    cout << "1. Insert a key" << endl;
    cout << "2. Find a key" << endl;
    cout << "3. Delete a key" << endl;
    cout << "4. Display the Hash Table" << endl;
    cout << "5. Exit" << endl;
    cout << "Enter your choice: ";
    cin >> ch;

    switch (ch) {
      case 1:
        cout << "Enter key to insert: ";
        cin >> key;
        insert(hT, key);
        break;

      case 2:
        cout << "Enter key to find: ";
        cin >> key;
        find(hT, key);
        break;

      case 3:
        cout << "Enter key to delete: ";
        cin >> key;
        deleteK(hT, key);
        break;

      case 4:
        displayHT(hT);
        break;

      case 5:
        cout << "Exiting..." << endl;
        break;
      default: cout << "Invalid choice" << endl;
    }
  } while (ch != 5);
  return 0;
}