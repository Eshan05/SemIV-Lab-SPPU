// 29. Design a mini project to implement a Smart text editor.

#include <fstream>
#include <iostream>
#include <string>
using namespace std;

enum ActionType {
	ACT_INSERT = 1,
	ACT_DELETE = 2,
	ACT_REPLACE = 3
};

struct Action {
	int type;
	int pos;        // 1-based position
	string oldText; // for delete/replace
	string newText; // for insert/replace
};

struct ActionStack {
	Action *a;
	int n;
	int cap;

	ActionStack() {
		a = NULL;
		n = 0;
		cap = 0;
	}

	~ActionStack() {
		delete[] a;
	}

	void clear() {
		n = 0;
	}

	bool empty() const {
		return n == 0;
	}

	void ensureCap(int need) {
		if (need <= cap) return;
		int newCap = (cap == 0) ? 8 : cap;
		while (newCap < need) newCap *= 2;
		Action *b = new Action[newCap];
		for (int i = 0; i < n; i++) b[i] = a[i];
		delete[] a;
		a = b;
		cap = newCap;
	}

	void push(const Action &x) {
		ensureCap(n + 1);
		a[n++] = x;
	}

	bool pop(Action &out) {
		if (n == 0) return false;
		out = a[n - 1];
		n--;
		return true;
	}
};

struct Document {
	string *lines;
	int n;
	int cap;

	Document() {
		lines = NULL;
		n = 0;
		cap = 0;
	}

	~Document() {
		delete[] lines;
	}

	void clear() {
		n = 0;
	}

	void ensureCap(int need) {
		if (need <= cap) return;
		int newCap = (cap == 0) ? 8 : cap;
		while (newCap < need) newCap *= 2;
		string *b = new string[newCap];
		for (int i = 0; i < n; i++) b[i] = lines[i];
		delete[] lines;
		lines = b;
		cap = newCap;
	}

	bool validPos(int pos) const {
		return pos >= 1 && pos <= n;
	}

	void insertLine(int pos, const string &text) {
		if (pos < 1) pos = 1;
		if (pos > n + 1) pos = n + 1;
		ensureCap(n + 1);
		for (int i = n; i >= pos; i--) lines[i] = lines[i - 1];
		lines[pos - 1] = text;
		n++;
	}

	string deleteLine(int pos) {
		if (!validPos(pos)) return "";
		string old = lines[pos - 1];
		for (int i = pos; i < n; i++) lines[i - 1] = lines[i];
		n--;
		return old;
	}

	string replaceLine(int pos, const string &text) {
		if (!validPos(pos)) return "";
		string old = lines[pos - 1];
		lines[pos - 1] = text;
		return old;
	}
};

void clearLineInput() {
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
		clearLineInput();
	}
}

string readLineStr(const string &prompt) {
	cout << prompt;
	string s;
	getline(cin >> ws, s);
	return s;
}

void showDocument(const Document &doc) {
	cout << "\n--- Document (" << doc.n << " lines) ---\n";
	if (doc.n == 0) {
		cout << "(empty)\n";
		return;
	}
	for (int i = 0; i < doc.n; i++) {
		cout << (i + 1) << ": " << doc.lines[i] << "\n";
	}
}

bool loadFromFile(Document &doc, const string &path) {
	ifstream in(path.c_str());
	if (!in) return false;
	doc.clear();
	string line;
	while (getline(in, line)) {
		doc.insertLine(doc.n + 1, line);
	}
	return true;
}

bool saveToFile(const Document &doc, const string &path) {
	ofstream out(path.c_str(), ios::trunc);
	if (!out) return false;
	for (int i = 0; i < doc.n; i++) out << doc.lines[i] << "\n";
	return true;
}

void doInsert(Document &doc, ActionStack &undo, ActionStack &redo) {
	int pos = readInt("Insert at line number (1..n+1): ");
	string text = readLineStr("Enter line text: ");
	doc.insertLine(pos, text);
	Action a;
	a.type = ACT_INSERT;
	a.pos = pos;
	a.oldText = "";
	a.newText = text;
	undo.push(a);
	redo.clear();
	cout << "Line inserted.\n";
}

void doAppend(Document &doc, ActionStack &undo, ActionStack &redo) {
	int pos = doc.n + 1;
	string text = readLineStr("Enter line text: ");
	doc.insertLine(pos, text);
	Action a;
	a.type = ACT_INSERT;
	a.pos = pos;
	a.oldText = "";
	a.newText = text;
	undo.push(a);
	redo.clear();
	cout << "Line appended.\n";
}

void doDelete(Document &doc, ActionStack &undo, ActionStack &redo) {
	if (doc.n == 0) {
		cout << "Document is empty.\n";
		return;
	}
	int pos = readInt("Delete which line (1..n): ");
	if (!doc.validPos(pos)) {
		cout << "Invalid line number.\n";
		return;
	}
	string old = doc.deleteLine(pos);
	Action a;
	a.type = ACT_DELETE;
	a.pos = pos;
	a.oldText = old;
	a.newText = "";
	undo.push(a);
	redo.clear();
	cout << "Line deleted.\n";
}

void doReplace(Document &doc, ActionStack &undo, ActionStack &redo) {
	if (doc.n == 0) {
		cout << "Document is empty.\n";
		return;
	}
	int pos = readInt("Replace which line (1..n): ");
	if (!doc.validPos(pos)) {
		cout << "Invalid line number.\n";
		return;
	}
	string text = readLineStr("Enter new text: ");
	string old = doc.replaceLine(pos, text);
	Action a;
	a.type = ACT_REPLACE;
	a.pos = pos;
	a.oldText = old;
	a.newText = text;
	undo.push(a);
	redo.clear();
	cout << "Line replaced.\n";
}

void doSearch(const Document &doc) {
	if (doc.n == 0) {
		cout << "Document is empty.\n";
		return;
	}
	string q = readLineStr("Search text: ");
	if (q.size() == 0) {
		cout << "Empty query.\n";
		return;
	}
	int found = 0;
	cout << "\nMatches:\n";
	for (int i = 0; i < doc.n; i++) {
		if (doc.lines[i].find(q) != string::npos) {
			cout << (i + 1) << ": " << doc.lines[i] << "\n";
			found++;
		}
	}
	if (found == 0) cout << "(no matches)\n";
	else cout << "Total matches: " << found << "\n";
}

void doUndo(Document &doc, ActionStack &undo, ActionStack &redo) {
	Action a;
	if (!undo.pop(a)) {
		cout << "Nothing to undo.\n";
		return;
	}
	if (a.type == ACT_INSERT) {
		doc.deleteLine(a.pos);
	} else if (a.type == ACT_DELETE) {
		doc.insertLine(a.pos, a.oldText);
	} else if (a.type == ACT_REPLACE) {
		doc.replaceLine(a.pos, a.oldText);
	}
	redo.push(a);
	cout << "Undo done.\n";
}

void doRedo(Document &doc, ActionStack &undo, ActionStack &redo) {
	Action a;
	if (!redo.pop(a)) {
		cout << "Nothing to redo.\n";
		return;
	}
	if (a.type == ACT_INSERT) {
		doc.insertLine(a.pos, a.newText);
	} else if (a.type == ACT_DELETE) {
		doc.deleteLine(a.pos);
	} else if (a.type == ACT_REPLACE) {
		doc.replaceLine(a.pos, a.newText);
	}
	undo.push(a);
	cout << "Redo done.\n";
}

int main() {
	Document doc;
	ActionStack undo;
	ActionStack redo;

	while (true) {
		cout << "\n===== MiniProject 29: Smart Text Editor (Undo/Redo) =====\n";
		cout << "1. New/Clear document\n";
		cout << "2. Load from file\n";
		cout << "3. Save to file\n";
		cout << "4. View document\n";
		cout << "5. Insert line at position\n";
		cout << "6. Append line\n";
		cout << "7. Delete line\n";
		cout << "8. Replace line\n";
		cout << "9. Search\n";
		cout << "10. Undo\n";
		cout << "11. Redo\n";
		cout << "12. Exit\n";
		int ch = readInt("Enter choice: ");

		if (ch == 1) {
			doc.clear();
			undo.clear();
			redo.clear();
			cout << "Document cleared.\n";
		} else if (ch == 2) {
			string path = readLineStr("Enter file path to load: ");
			if (loadFromFile(doc, path)) {
				undo.clear();
				redo.clear();
				cout << "File loaded.\n";
			} else {
				cout << "Unable to open file.\n";
			}
		} else if (ch == 3) {
			string path = readLineStr("Enter file path to save: ");
			if (saveToFile(doc, path)) cout << "File saved.\n";
			else cout << "Unable to write file.\n";
		} else if (ch == 4) {
			showDocument(doc);
		} else if (ch == 5) {
			doInsert(doc, undo, redo);
		} else if (ch == 6) {
			doAppend(doc, undo, redo);
		} else if (ch == 7) {
			doDelete(doc, undo, redo);
		} else if (ch == 8) {
			doReplace(doc, undo, redo);
		} else if (ch == 9) {
			doSearch(doc);
		} else if (ch == 10) {
			doUndo(doc, undo, redo);
		} else if (ch == 11) {
			doRedo(doc, undo, redo);
		} else if (ch == 12) {
			break;
		} else {
			cout << "Invalid choice.\n";
		}
	}
	return 0;
}
