// 20. Consider a scenario for Hospital to cater services to different kinds of patients as
//     1.  Serious (Top priority),
//     2.  Non-serious (Medium priority),
//     3.  General Checkup (Least priority).

//     Implement the priority queue to cater services to the patients

#include <iostream>
#include <string>
using namespace std;

struct Patient {
  int token;
  string name;
  int age;
  string issue;
};

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

string readLine(const string &prompt) {
  cout << prompt;
  string s;
  getline(cin >> ws, s);
  return s;
}

class CircularQueue {
  static const int CAP = 60;
  Patient data[CAP];
  int front;
  int rear;
  int sz;

 public:
  CircularQueue() : front(0), rear(0), sz(0) {}

  bool isEmpty() const {
    return sz == 0;
  }

  bool isFull() const {
    return sz == CAP;
  }

  int size() const {
    return sz;
  }

  bool enqueue(const Patient &p) {
    if (isFull()) return false;
    data[rear] = p;
    rear = (rear + 1) % CAP;
    sz++;
    return true;
  }

  bool dequeue(Patient &out) {
    if (isEmpty()) return false;
    out = data[front];
    front = (front + 1) % CAP;
    sz--;
    return true;
  }

  void display(const string &title) const {
    cout << "\n-- " << title << " (waiting: " << sz << ") --\n";
    if (isEmpty()) {
      cout << "(empty)\n";
      return;
    }
    cout << "Token\tName\tAge\tIssue\n";
    for (int i = 0; i < sz; ++i) {
      int idx = (front + i) % CAP;
      cout << data[idx].token << "\t" << data[idx].name << "\t" << data[idx].age
           << "\t" << data[idx].issue << "\n";
    }
  }
};

int main() {
  CircularQueue serious;
  CircularQueue nonSerious;
  CircularQueue general;
  int nextToken = 1;

  while (true) {
    cout << "\n===== Group E20 (Hospital Priority Queue) =====\n";
    cout << "1. Add patient\n";
    cout << "2. Serve next patient\n";
    cout << "3. Display waiting list\n";
    cout << "4. Queue status\n";
    cout << "5. Exit\n";

    int ch = readInt("Enter choice: ");
    if (ch == 1) {
      Patient p;
      p.token = nextToken++;
      p.name = readLine("Name: ");
      p.age = readInt("Age: ");
      if (p.age < 0) p.age = 0;
      p.issue = readLine("Issue/Problem: ");

      cout << "Priority Type:\n";
      cout << "1. Serious (Top priority)\n";
      cout << "2. Non-serious (Medium priority)\n";
      cout << "3. General Checkup (Least priority)\n";
      int pr = readInt("Enter type (1-3): ");

      bool ok = false;
      if (pr == 1) ok = serious.enqueue(p);
      else if (pr == 2) ok = nonSerious.enqueue(p);
      else if (pr == 3) ok = general.enqueue(p);
      else {
        cout << "Invalid type. Patient not added.\n";
        continue;
      }

      if (ok) {
        cout << "Patient added. Token = " << p.token << "\n";
      } else {
        cout << "Queue is full for this priority. Patient not added.\n";
      }
    } else if (ch == 2) {
      Patient served;
      bool ok = false;
      string from;
      if (serious.dequeue(served)) {
        ok = true;
        from = "Serious";
      } else if (nonSerious.dequeue(served)) {
        ok = true;
        from = "Non-serious";
      } else if (general.dequeue(served)) {
        ok = true;
        from = "General";
      }

      if (!ok) {
        cout << "No patients waiting.\n";
      } else {
        cout << "\nServing next patient (" << from << ")\n";
        cout << "Token: " << served.token << "\n";
        cout << "Name: " << served.name << "\n";
        cout << "Age: " << served.age << "\n";
        cout << "Issue: " << served.issue << "\n";
      }
    } else if (ch == 3) {
      serious.display("Serious");
      nonSerious.display("Non-serious");
      general.display("General Checkup");
    } else if (ch == 4) {
      cout << "\nWaiting counts\n";
      cout << "Serious: " << serious.size() << "\n";
      cout << "Non-serious: " << nonSerious.size() << "\n";
      cout << "General: " << general.size() << "\n";
      cout << "\nEnqueue/Dequeue complexity: O(1)\n";
    } else if (ch == 5) {
      break;
    } else {
      cout << "Invalid choice.\n";
    }
  }

  return 0;
}
