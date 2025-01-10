// 20. Consider a scenario for Hospital to cater services to different kinds of patients as 
//     1.  Serious (Top priority), 
//     2.  Non-serious (Medium priority), 
//     3.  General Checkup (Least priority).

//     Implement the priority queue to cater services to the patients

import java.io.BufferedReader;
import java.io.InputStreamReader;

public class E20 {
	static class Patient {
		int token;
		String name;
		int age;
		String issue;
 
		Patient(int token, String name, int age, String issue) {
			this.token = token;
			this.name = name;
			this.age = age;
			this.issue = issue;
		}
	}

	static class CircularQueue {
		private Patient[] data;
		private int front;
		private int rear;
		private int size;

		CircularQueue(int capacity) {
			data = new Patient[capacity];
			front = 0;
			rear = 0;
			size = 0;
		}

		boolean isEmpty() {
			return size == 0;
		}

		boolean isFull() {
			return size == data.length;
		}

		int size() {
			return size;
		}

		boolean enqueue(Patient p) {
			if (isFull()) return false;
			data[rear] = p;
			rear = (rear + 1) % data.length;
			size++;
			return true;
		}

		Patient dequeue() {
			if (isEmpty()) return null;
			Patient p = data[front];
			front = (front + 1) % data.length;
			size--;
			return p;
		}

		void display(String title) {
			System.out.println("\n-- " + title + " (waiting: " + size + ") --");
			if (isEmpty()) {
				System.out.println("(empty)");
				return;
			}
			System.out.println("Token\tName\tAge\tIssue");
			for (int i = 0; i < size; i++) {
				int idx = (front + i) % data.length;
				Patient p = data[idx];
				System.out.println(p.token + "\t" + p.name + "\t" + p.age + "\t" + p.issue);
			}
		}
	}

	static BufferedReader br = new BufferedReader(new InputStreamReader(System.in));

	static int readInt(String prompt) throws Exception {
		while (true) {
			System.out.print(prompt);
			String s = br.readLine();
			if (s == null) return 0;
			s = s.trim();
			try {
				return Integer.parseInt(s);
			} catch (Exception e) {
				System.out.println("Invalid number. Try again.");
			}
		}
	}

	static String readLine(String prompt) throws Exception {
		System.out.print(prompt);
		String s = br.readLine();
		return (s == null) ? "" : s;
	}

	public static void main(String[] args) throws Exception {
		final int CAP = 60;
		CircularQueue serious = new CircularQueue(CAP);
		CircularQueue nonSerious = new CircularQueue(CAP);
		CircularQueue general = new CircularQueue(CAP);
		int nextToken = 1;

		while (true) {
			System.out.println("\n===== Group E20 (Hospital Priority Queue) =====");
			System.out.println("1. Add patient");
			System.out.println("2. Serve next patient");
			System.out.println("3. Display waiting list");
			System.out.println("4. Queue status");
			System.out.println("5. Exit");

			int ch = readInt("Enter choice: ");
			if (ch == 1) {
				int token = nextToken++;
				String name = readLine("Name: ").trim();
				int age = readInt("Age: ");
				if (age < 0) age = 0;
				String issue = readLine("Issue/Problem: ").trim();

				System.out.println("Priority Type:");
				System.out.println("1. Serious (Top priority)");
				System.out.println("2. Non-serious (Medium priority)");
				System.out.println("3. General Checkup (Least priority)");
				int pr = readInt("Enter type (1-3): ");

				Patient p = new Patient(token, name, age, issue);
				boolean ok = false;
				if (pr == 1) ok = serious.enqueue(p);
				else if (pr == 2) ok = nonSerious.enqueue(p);
				else if (pr == 3) ok = general.enqueue(p);
				else {
					System.out.println("Invalid type. Patient not added.");
					continue;
				}

				if (ok) System.out.println("Patient added. Token = " + token);
				else System.out.println("Queue is full for this priority. Patient not added.");
			} else if (ch == 2) {
				Patient served = null;
				String from = "";
				served = serious.dequeue();
				if (served != null) from = "Serious";
				else {
					served = nonSerious.dequeue();
					if (served != null) from = "Non-serious";
					else {
						served = general.dequeue();
						if (served != null) from = "General";
					}
				}

				if (served == null) {
					System.out.println("No patients waiting.");
				} else {
					System.out.println("\nServing next patient (" + from + ")");
					System.out.println("Token: " + served.token);
					System.out.println("Name: " + served.name);
					System.out.println("Age: " + served.age);
					System.out.println("Issue: " + served.issue);
				}
			} else if (ch == 3) {
				serious.display("Serious");
				nonSerious.display("Non-serious");
				general.display("General Checkup");
			} else if (ch == 4) {
				System.out.println("\nWaiting counts");
				System.out.println("Serious: " + serious.size());
				System.out.println("Non-serious: " + nonSerious.size());
				System.out.println("General: " + general.size());
				System.out.println("\nEnqueue/Dequeue complexity: O(1)");
			} else if (ch == 5) {
				break;
			} else {
				System.out.println("Invalid choice.");
			}
		}
	}
}
