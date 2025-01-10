// 17. Consider the scheduling problem. n tasks to be scheduled on single processor. Let $t_1, \dots ,t_n$ be durations required to execute on single processor is known. The tasks can be executed in any order but one task at a time. Design a greedy algorithm for this problem and find a schedule that minimizes the total time spent by all the tasks in the system. (The time spent by one is the sum of the waiting time of task and the time spent on its execution.)

#include <iostream>
#include <string>
using namespace std;

const int MAXT = 200;

struct Task {
	string name;
	int duration;
	int waiting;
	int completion;
	int spent;
};

Task tasks[MAXT];
int n = 0;

void clearLine() {
	char c;
	while (cin.get(c)) if (c == '\n') break;
}

int readInt(const string& prompt) {
	int x;
	while (true) {
		cout << prompt;
		if (cin >> x) return x;
		cout << "Invalid number. Try again.\n";
		cin.clear();
		clearLine();
	}
}

string readLine(const string& prompt) {
	cout << prompt;
	string s;
	getline(cin >> ws, s);
	return s;
}

void inputTasks() {
	n = readInt("Enter number of tasks (max 200): ");
	if (n < 0) n = 0;
	if (n > MAXT) n = MAXT;

	for (int i = 0; i < n; ++i) {
		tasks[i].name = readLine("Task " + to_string(i + 1) + " name: ");
		tasks[i].duration = readInt("Task " + to_string(i + 1) + " duration: ");
		if (tasks[i].duration < 0) tasks[i].duration = 0;
		tasks[i].waiting = 0;
		tasks[i].completion = 0;
		tasks[i].spent = 0;
	}
}

void sortByDuration() {
	for (int i = 0; i < n - 1; ++i) {
		for (int j = 0; j < n - i - 1; ++j) {
			if (tasks[j].duration > tasks[j + 1].duration) {
				Task t = tasks[j];
				tasks[j] = tasks[j + 1];
				tasks[j + 1] = t;
			}
		}
	}
}

void computeMetrics() {
	int time = 0;
	for (int i = 0; i < n; ++i) {
		tasks[i].waiting = time;
		time += tasks[i].duration;
		tasks[i].completion = time;
		tasks[i].spent = tasks[i].waiting + tasks[i].duration;
	}
}

void showSchedule() {
	if (n == 0) {
		cout << "No tasks loaded\n";
		return;
	}

	cout << "\nGreedy order (Shortest Job First):\n";
	for (int i = 0; i < n; ++i) {
		cout << (i + 1) << ". " << tasks[i].name << " (" << tasks[i].duration << ")\n";
	}

	cout << "\nDetailed Table:\n";
	cout << "Task | Duration | Waiting | Completion | TimeSpent(wait+exec)\n";
	for (int i = 0; i < n; ++i) {
		cout << tasks[i].name << " | " << tasks[i].duration << " | "
				 << tasks[i].waiting << " | " << tasks[i].completion << " | "
				 << tasks[i].spent << "\n";
	}

	long long totalWait = 0, totalSpent = 0;
	for (int i = 0; i < n; ++i) {
		totalWait += tasks[i].waiting;
		totalSpent += tasks[i].spent;
	}

	cout << "\nTotal waiting time: " << totalWait << "\n";
	cout << "Total time spent by all tasks: " << totalSpent << "\n";
	if (n > 0) {
		cout << "Average waiting time: " << (double)totalWait / n << "\n";
		cout << "Average time spent per task: " << (double)totalSpent / n << "\n";
	}
}

int main() {
	while (true) {
		cout << "\n===== Scheduling TUI (C17) =====\n";
		cout << "1. Input tasks\n";
		cout << "2. Run greedy schedule (SJF)\n";
		cout << "3. Exit\n";

		int ch = readInt("Enter choice: ");

		if (ch == 1) {
			inputTasks();
		} else if (ch == 2) {
			if (n == 0) cout << "Input tasks first\n";
			else {
				sortByDuration();
				computeMetrics();
				showSchedule();
			}
		} else if (ch == 3) {
			break;
		} else {
			cout << "Invalid choice\n";
		}
	}

	return 0;
}
