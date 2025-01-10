// 30. Design a mini project for automated Term work assessment of student based on parameters like daily attendance, Unit Test / Prelim performance, Students achievements if any, Mock Practical.

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.InputStreamReader;

public class MiniProject_30 {
	static final String DATA_FILE = "MP30_StudentsL.txt";
	static final String REPORT_FILE = "MP30_Report.txt";
	static final int MAXR = 1000;

	// Scoring formula (max 100):
	// Attendance (0-100) * 0.20 -> 0..20
	// UT/Prelim (0-100) * 0.40 -> 0..40
	// Mock Practical (0-100) * 0.30 -> 0..30
	// Achievements points (0-10) -> 0..10
	static final double W_ATT = 0.20;
	static final double W_UT = 0.40;
	static final double W_MOCK = 0.30;
	static final int ACH_MAX = 10;

	static BufferedReader br = new BufferedReader(new InputStreamReader(System.in));

	static class Student {
		int roll;
		String name;
		String division;
		double attendance;
		double ut;
		double mock;
		int achievements;
		double totalScore;
		int scorePoints; // totalScore * 100
	}

	static class IntList {
		int[] a;
		int n;

		IntList(int cap) {
			if (cap < 1) cap = 1;
			a = new int[cap];
			n = 0;
		}

		void add(int x) {
			if (n >= a.length) {
				int[] b = new int[a.length * 2];
				for (int i = 0; i < a.length; i++) b[i] = a[i];
				a = b;
			}
			a[n++] = x;
		}

		int size() {
			return n;
		}
	}

	static class RollIndex {
		int[] keys;
		int[] values;
		byte[] states; // 0 empty, 1 used, 2 deleted
		int size;
		int threshold;

		RollIndex(int expectedSize) {
			if (expectedSize < 4) expectedSize = 4;
			int tableSize = 1;
			while (tableSize < expectedSize * 2) tableSize <<= 1;
			keys = new int[tableSize];
			values = new int[tableSize];
			states = new byte[tableSize];
			size = 0;
			threshold = (int) (tableSize * 7L / 10L);
		}

		int hash(int k) {
			int h = k ^ (k >>> 16);
			h *= 0x7feb352d;
			h ^= (h >>> 15);
			h *= 0x846ca68b;
			h ^= (h >>> 16);
			return h;
		}

		void rehash(int newSize) {
			int[] oldK = keys;
			int[] oldV = values;
			byte[] oldS = states;
			keys = new int[newSize];
			values = new int[newSize];
			states = new byte[newSize];
			size = 0;
			threshold = (int) (newSize * 7L / 10L);
			for (int i = 0; i < oldK.length; i++) {
				if (oldS[i] == 1) put(oldK[i], oldV[i]);
			}
		}

		int get(int key) {
			int mask = keys.length - 1;
			int idx = hash(key) & mask;
			while (true) {
				byte st = states[idx];
				if (st == 0) return -1;
				if (st == 1 && keys[idx] == key) return values[idx];
				idx = (idx + 1) & mask;
			}
		}

		void put(int key, int value) {
			if (size + 1 > threshold) rehash(keys.length * 2);
			int mask = keys.length - 1;
			int idx = hash(key) & mask;
			int firstDel = -1;
			while (true) {
				byte st = states[idx];
				if (st == 0) {
					int ins = (firstDel >= 0) ? firstDel : idx;
					states[ins] = 1;
					keys[ins] = key;
					values[ins] = value;
					size++;
					return;
				}
				if (st == 1 && keys[idx] == key) {
					values[idx] = value;
					return;
				}
				if (st == 2 && firstDel < 0) firstDel = idx;
				idx = (idx + 1) & mask;
			}
		}
	}

	static class DivisionIndex {
		String[] keys;
		IntList[] values;
		byte[] states;
		int size;
		int threshold;

		DivisionIndex(int expectedSize) {
			if (expectedSize < 4) expectedSize = 4;
			int tableSize = 1;
			while (tableSize < expectedSize * 2) tableSize <<= 1;
			keys = new String[tableSize];
			values = new IntList[tableSize];
			states = new byte[tableSize];
			size = 0;
			threshold = (int) (tableSize * 7L / 10L);
		}

		int hashString(String s) {
			int h = 0;
			for (int i = 0; i < s.length(); i++) h = 31 * h + s.charAt(i);
			h ^= (h >>> 16);
			return h;
		}

		void rehash(int newSize) {
			String[] oldK = keys;
			IntList[] oldV = values;
			byte[] oldS = states;
			keys = new String[newSize];
			values = new IntList[newSize];
			states = new byte[newSize];
			size = 0;
			threshold = (int) (newSize * 7L / 10L);
			for (int i = 0; i < oldK.length; i++) {
				if (oldS[i] == 1) {
					IntList list = getOrCreate(oldK[i]);
					IntList src = oldV[i];
					for (int j = 0; j < src.n; j++) list.add(src.a[j]);
				}
			}
		}

		IntList get(String key) {
			int mask = keys.length - 1;
			int idx = hashString(key) & mask;
			while (true) {
				byte st = states[idx];
				if (st == 0) return null;
				if (st == 1 && keys[idx].equals(key)) return values[idx];
				idx = (idx + 1) & mask;
			}
		}

		IntList getOrCreate(String key) {
			if (size + 1 > threshold) rehash(keys.length * 2);
			int mask = keys.length - 1;
			int idx = hashString(key) & mask;
			int firstDel = -1;
			while (true) {
				byte st = states[idx];
				if (st == 0) {
					int ins = (firstDel >= 0) ? firstDel : idx;
					states[ins] = 1;
					keys[ins] = key;
					values[ins] = new IntList(8);
					size++;
					return values[ins];
				}
				if (st == 1 && keys[idx].equals(key)) return values[idx];
				if (st == 2 && firstDel < 0) firstDel = idx;
				idx = (idx + 1) & mask;
			}
		}
	}

	static class ScoreNode {
		int scorePoints;
		int roll;
		int studentIndex;
		int height;
		ScoreNode left;
		ScoreNode right;
	}

	static int height(ScoreNode n) {
		return (n == null) ? 0 : n.height;
	}

	static int max(int a, int b) {
		return (a > b) ? a : b;
	}

	static int cmpKey(int s1, int r1, int s2, int r2) {
		if (s1 != s2) return (s1 < s2) ? -1 : 1;
		if (r1 != r2) return (r1 < r2) ? -1 : 1;
		return 0;
	}

	static ScoreNode rotateRight(ScoreNode y) {
		ScoreNode x = y.left;
		ScoreNode t2 = x.right;
		x.right = y;
		y.left = t2;
		y.height = max(height(y.left), height(y.right)) + 1;
		x.height = max(height(x.left), height(x.right)) + 1;
		return x;
	}

	static ScoreNode rotateLeft(ScoreNode x) {
		ScoreNode y = x.right;
		ScoreNode t2 = y.left;
		y.left = x;
		x.right = t2;
		x.height = max(height(x.left), height(x.right)) + 1;
		y.height = max(height(y.left), height(y.right)) + 1;
		return y;
	}

	static int balance(ScoreNode n) {
		return (n == null) ? 0 : (height(n.left) - height(n.right));
	}

	static ScoreNode insertScore(ScoreNode node, int scorePoints, int roll, int studentIndex) {
		if (node == null) {
			ScoreNode nn = new ScoreNode();
			nn.scorePoints = scorePoints;
			nn.roll = roll;
			nn.studentIndex = studentIndex;
			nn.height = 1;
			return nn;
		}
		int c = cmpKey(scorePoints, roll, node.scorePoints, node.roll);
		if (c < 0) node.left = insertScore(node.left, scorePoints, roll, studentIndex);
		else if (c > 0) node.right = insertScore(node.right, scorePoints, roll, studentIndex);
		else {
			node.studentIndex = studentIndex;
			return node;
		}

		node.height = 1 + max(height(node.left), height(node.right));
		int bf = balance(node);

		if (bf > 1 && cmpKey(scorePoints, roll, node.left.scorePoints, node.left.roll) < 0) return rotateRight(node);
		if (bf < -1 && cmpKey(scorePoints, roll, node.right.scorePoints, node.right.roll) > 0) return rotateLeft(node);
		if (bf > 1 && cmpKey(scorePoints, roll, node.left.scorePoints, node.left.roll) > 0) {
			node.left = rotateLeft(node.left);
			return rotateRight(node);
		}
		if (bf < -1 && cmpKey(scorePoints, roll, node.right.scorePoints, node.right.roll) < 0) {
			node.right = rotateRight(node.right);
			return rotateLeft(node);
		}
		return node;
	}

	static ScoreNode minNode(ScoreNode n) {
		ScoreNode cur = n;
		while (cur != null && cur.left != null) cur = cur.left;
		return cur;
	}

	static ScoreNode deleteScore(ScoreNode root, int scorePoints, int roll) {
		if (root == null) return null;
		int c = cmpKey(scorePoints, roll, root.scorePoints, root.roll);
		if (c < 0) root.left = deleteScore(root.left, scorePoints, roll);
		else if (c > 0) root.right = deleteScore(root.right, scorePoints, roll);
		else {
			if (root.left == null || root.right == null) {
				ScoreNode temp = (root.left != null) ? root.left : root.right;
				if (temp == null) {
					root = null;
				} else {
					root = temp;
				}
			} else {
				ScoreNode temp = minNode(root.right);
				root.scorePoints = temp.scorePoints;
				root.roll = temp.roll;
				root.studentIndex = temp.studentIndex;
				root.right = deleteScore(root.right, temp.scorePoints, temp.roll);
			}
		}
		if (root == null) return null;

		root.height = 1 + max(height(root.left), height(root.right));
		int bf = balance(root);

		if (bf > 1 && balance(root.left) >= 0) return rotateRight(root);
		if (bf > 1 && balance(root.left) < 0) {
			root.left = rotateLeft(root.left);
			return rotateRight(root);
		}
		if (bf < -1 && balance(root.right) <= 0) return rotateLeft(root);
		if (bf < -1 && balance(root.right) > 0) {
			root.right = rotateRight(root.right);
			return rotateLeft(root);
		}
		return root;
	}

	static Student[] students = new Student[MAXR];
	static int nStudents = 0;

	static RollIndex rollIndex = new RollIndex(16);
	static DivisionIndex divisionIndex = new DivisionIndex(16);
	static ScoreNode scoreRoot = null;

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

	static double readDouble(String prompt) throws Exception {
		while (true) {
			System.out.print(prompt);
			String s = br.readLine();
			if (s == null) return 0.0;
			s = s.trim();
			try {
				return Double.parseDouble(s);
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

	static double clamp(double x, double lo, double hi) {
		if (x < lo) return lo;
		if (x > hi) return hi;
		return x;
	}

	static int clampInt(int x, int lo, int hi) {
		if (x < lo) return lo;
		if (x > hi) return hi;
		return x;
	}

	static void computeScore(Student s) {
		s.attendance = clamp(s.attendance, 0, 100);
		s.ut = clamp(s.ut, 0, 100);
		s.mock = clamp(s.mock, 0, 100);
		s.achievements = clampInt(s.achievements, 0, ACH_MAX);
		s.totalScore = (s.attendance * W_ATT) + (s.ut * W_UT) + (s.mock * W_MOCK) + s.achievements;
		if (s.totalScore < 0) s.totalScore = 0;
		if (s.totalScore > 100) s.totalScore = 100;
		s.scorePoints = (int) (s.totalScore * 100.0 + 0.5);
	}

	static String sanitizeField(String s) {
		if (s == null) return "";
		StringBuilder out = new StringBuilder();
		for (int i = 0; i < s.length(); i++) {
			char c = s.charAt(i);
			if (c == '|') c = '/';
			if (c == '\r' || c == '\n') c = ' ';
			out.append(c);
		}
		return out.toString();
	}

	static String toLine(Student s) {
		return s.roll + "|" + sanitizeField(s.name) + "|" + sanitizeField(s.division) + "|" + s.attendance + "|" + s.ut + "|" + s.mock
				+ "|" + s.achievements;
	}

	static boolean parseLine(String line, Student s) {
		// roll|name|division|attendance|ut|mock|achievements
		if (line == null) return false;
		int p1 = line.indexOf('|');
		if (p1 < 0) return false;
		int p2 = line.indexOf('|', p1 + 1);
		if (p2 < 0) return false;
		int p3 = line.indexOf('|', p2 + 1);
		if (p3 < 0) return false;
		int p4 = line.indexOf('|', p3 + 1);
		if (p4 < 0) return false;
		int p5 = line.indexOf('|', p4 + 1);
		if (p5 < 0) return false;
		int p6 = line.indexOf('|', p5 + 1);
		if (p6 < 0) return false;

		try {
			s.roll = Integer.parseInt(line.substring(0, p1).trim());
			s.name = line.substring(p1 + 1, p2).trim();
			s.division = line.substring(p2 + 1, p3).trim();
			s.attendance = Double.parseDouble(line.substring(p3 + 1, p4).trim());
			s.ut = Double.parseDouble(line.substring(p4 + 1, p5).trim());
			s.mock = Double.parseDouble(line.substring(p5 + 1, p6).trim());
			s.achievements = Integer.parseInt(line.substring(p6 + 1).trim());
		} catch (Exception e) {
			return false;
		}
		computeScore(s);
		return true;
	}

	static void loadData() throws Exception {
		File f = new File(DATA_FILE);
		if (!f.exists()) {
			nStudents = 0;
			rebuildIndexes();
			return;
		}
		BufferedReader r = new BufferedReader(new FileReader(f));
		String line;
		nStudents = 0;
		while ((line = r.readLine()) != null && nStudents < MAXR) {
			Student s = new Student();
			if (!parseLine(line, s)) continue;
			students[nStudents++] = s;
		}
		r.close();
		rebuildIndexes();
	}

	static void saveData() throws Exception {
		BufferedWriter w = new BufferedWriter(new FileWriter(DATA_FILE, false));
		for (int i = 0; i < nStudents; i++) {
			w.write(toLine(students[i]));
			w.newLine();
		}
		w.close();
	}

	static void rebuildIndexes() {
		rollIndex = new RollIndex(nStudents * 2 + 8);
		divisionIndex = new DivisionIndex(nStudents * 2 + 8);
		scoreRoot = null;
		for (int i = 0; i < nStudents; i++) {
			Student s = students[i];
			rollIndex.put(s.roll, i);
			divisionIndex.getOrCreate(s.division).add(i);
			scoreRoot = insertScore(scoreRoot, s.scorePoints, s.roll, i);
		}
	}

	static void showStudent(Student s) {
		System.out.println("\nRoll: " + s.roll);
		System.out.println("Name: " + s.name);
		System.out.println("Division: " + s.division);
		System.out.println("Attendance(%): " + s.attendance);
		System.out.println("UT/Prelim(%): " + s.ut);
		System.out.println("Mock Practical(%): " + s.mock);
		System.out.println("Achievements(0-10): " + s.achievements);
		System.out.println("Total Score(/100): " + String.format("%.2f", s.totalScore));
	}

	static void addStudent() throws Exception {
		if (nStudents >= MAXR) {
			System.out.println("Maximum records reached.");
			return;
		}
		System.out.println("\n--- Add Student ---");
		int roll = readInt("Roll number: ");
		if (roll <= 0) {
			System.out.println("Roll should be positive.");
			return;
		}
		if (rollIndex.get(roll) != -1) {
			System.out.println("Roll already exists.");
			return;
		}
		Student s = new Student();
		s.roll = roll;
		s.name = readLine("Name: ").trim();
		s.division = readLine("Division: ").trim();
		s.attendance = readDouble("Attendance (%): ");
		s.ut = readDouble("UT/Prelim (%): ");
		s.mock = readDouble("Mock Practical (%): ");
		s.achievements = readInt("Achievements points (0-10): ");
		computeScore(s);
		students[nStudents++] = s;
		rebuildIndexes();
		saveData();
		System.out.println("Student added.");
	}

	static void updateStudent() throws Exception {
		if (nStudents == 0) {
			System.out.println("No records.");
			return;
		}
		System.out.println("\n--- Update Student ---");
		int roll = readInt("Roll number to update: ");
		int idx = rollIndex.get(roll);
		if (idx < 0) {
			System.out.println("Student not found.");
			return;
		}
		Student s = students[idx];
		showStudent(s);
		System.out.println("\nEnter new values:");
		s.name = readLine("Name: ").trim();
		s.division = readLine("Division: ").trim();
		s.attendance = readDouble("Attendance (%): ");
		s.ut = readDouble("UT/Prelim (%): ");
		s.mock = readDouble("Mock Practical (%): ");
		s.achievements = readInt("Achievements points (0-10): ");
		computeScore(s);
		rebuildIndexes();
		saveData();
		System.out.println("Student updated.");
	}

	static void deleteStudent() throws Exception {
		if (nStudents == 0) {
			System.out.println("No records.");
			return;
		}
		System.out.println("\n--- Delete Student ---");
		int roll = readInt("Roll number to delete: ");
		int idx = rollIndex.get(roll);
		if (idx < 0) {
			System.out.println("Student not found.");
			return;
		}
		for (int i = idx; i < nStudents - 1; i++) students[i] = students[i + 1];
		nStudents--;
		rebuildIndexes();
		saveData();
		System.out.println("Student deleted.");
	}

	static void displayStudent() throws Exception {
		System.out.println("\n--- Display Student ---");
		int roll = readInt("Roll number: ");
		int idx = rollIndex.get(roll);
		if (idx < 0) {
			System.out.println("Student not found.");
			return;
		}
		showStudent(students[idx]);
	}

	static void displayAll() {
		System.out.println("\n--- All Students ---");
		if (nStudents == 0) {
			System.out.println("(no records)");
			return;
		}
		System.out.println("Roll\tName\tDiv\tAtt\tUT\tMock\tAch\tTotal");
		for (int i = 0; i < nStudents; i++) {
			Student s = students[i];
			System.out.println(s.roll + "\t" + s.name + "\t" + s.division + "\t" + String.format("%.1f", s.attendance) + "\t"
					+ String.format("%.1f", s.ut) + "\t" + String.format("%.1f", s.mock) + "\t" + s.achievements + "\t"
					+ String.format("%.2f", s.totalScore));
		}
	}

	static class RankCounter {
		int remaining;
		int rank;
		StringBuilder sb;
	}

	static void traverseDesc(ScoreNode node, RankCounter c) {
		if (node == null || c.remaining <= 0) return;
		traverseDesc(node.right, c);
		if (node != null && c.remaining > 0) {
			Student s = students[node.studentIndex];
			c.rank++;
			c.sb.append(c.rank).append(". ")
					.append(s.roll).append("\t")
					.append(s.name).append("\t")
					.append(s.division).append("\t")
					.append(String.format("%.2f", s.totalScore)).append("\n");
			c.remaining--;
		}
		traverseDesc(node.left, c);
	}

	static void showRankList() throws Exception {
		if (nStudents == 0) {
			System.out.println("No records.");
			return;
		}
		System.out.println("\n--- Ranklist (Top N) ---");
		int top = readInt("Enter N: ");
		if (top <= 0) return;
		if (top > nStudents) top = nStudents;
		RankCounter c = new RankCounter();
		c.remaining = top;
		c.rank = 0;
		c.sb = new StringBuilder();
		c.sb.append("Rank\tRoll\tName\tDiv\tTotal\n");
		traverseDesc(scoreRoot, c);
		System.out.print(c.sb.toString());
	}

	static void divisionReport() throws Exception {
		if (nStudents == 0) {
			System.out.println("No records.");
			return;
		}
		System.out.println("\n--- Division Report ---");
		String div = readLine("Enter division: ").trim();
		IntList list = divisionIndex.get(div);
		if (list == null || list.size() == 0) {
			System.out.println("No students found in this division.");
			return;
		}
		double sumAtt = 0, sumUt = 0, sumMock = 0, sumTotal = 0;
		System.out.println("Roll\tName\tAtt\tUT\tMock\tAch\tTotal");
		for (int i = 0; i < list.n; i++) {
			Student s = students[list.a[i]];
			sumAtt += s.attendance;
			sumUt += s.ut;
			sumMock += s.mock;
			sumTotal += s.totalScore;
			System.out.println(s.roll + "\t" + s.name + "\t" + String.format("%.1f", s.attendance) + "\t" + String.format("%.1f", s.ut)
					+ "\t" + String.format("%.1f", s.mock) + "\t" + s.achievements + "\t" + String.format("%.2f", s.totalScore));
		}
		int cnt = list.size();
		System.out.println("\nDivision: " + div);
		System.out.println("Count: " + cnt);
		System.out.println("Avg Attendance: " + String.format("%.2f", sumAtt / cnt));
		System.out.println("Avg UT: " + String.format("%.2f", sumUt / cnt));
		System.out.println("Avg Mock: " + String.format("%.2f", sumMock / cnt));
		System.out.println("Avg Total: " + String.format("%.2f", sumTotal / cnt));
	}

	static String buildAnalyticsReport() {
		StringBuilder sb = new StringBuilder();
		sb.append("===== Term Work Analytics Report =====\n");
		sb.append("Scoring: total = attendance*0.20 + UT*0.40 + mock*0.30 + achievements(0..10)\n\n");
		sb.append("Total students: ").append(nStudents).append("\n");
		if (nStudents == 0) return sb.toString();

		double sumAtt = 0, sumUt = 0, sumMock = 0, sumTotal = 0;
		int atRisk = 0;
		for (int i = 0; i < nStudents; i++) {
			Student s = students[i];
			sumAtt += s.attendance;
			sumUt += s.ut;
			sumMock += s.mock;
			sumTotal += s.totalScore;
			if (s.attendance < 75.0 || s.totalScore < 50.0) atRisk++;
		}
		sb.append("Average Attendance: ").append(String.format("%.2f", sumAtt / nStudents)).append("\n");
		sb.append("Average UT: ").append(String.format("%.2f", sumUt / nStudents)).append("\n");
		sb.append("Average Mock: ").append(String.format("%.2f", sumMock / nStudents)).append("\n");
		sb.append("Average Total Score: ").append(String.format("%.2f", sumTotal / nStudents)).append("\n");
		sb.append("At-risk students (attendance<75 OR total<50): ").append(atRisk).append("\n\n");

		sb.append("--- Top 5 Students ---\n");
		RankCounter c = new RankCounter();
		c.remaining = (nStudents < 5) ? nStudents : 5;
		c.rank = 0;
		c.sb = sb;
		sb.append("Rank\tRoll\tName\tDiv\tTotal\n");
		traverseDesc(scoreRoot, c);
		sb.append("\n");

		sb.append("--- Division Summary ---\n");
		sb.append("Div\tCount\tAvgTotal\n");
		for (int i = 0; i < divisionIndex.keys.length; i++) {
			if (divisionIndex.states[i] == 1) {
				String div = divisionIndex.keys[i];
				IntList list = divisionIndex.values[i];
				if (list == null || list.size() == 0) continue;
				double st = 0;
				for (int j = 0; j < list.n; j++) st += students[list.a[j]].totalScore;
				sb.append(div).append("\t").append(list.size()).append("\t").append(String.format("%.2f", st / list.size())).append("\n");
			}
		}
		sb.append("\n--- At-Risk List ---\n");
		sb.append("Roll\tName\tDiv\tAtt\tTotal\n");
		int shown = 0;
		for (int i = 0; i < nStudents; i++) {
			Student s = students[i];
			if (s.attendance < 75.0 || s.totalScore < 50.0) {
				sb.append(s.roll).append("\t").append(s.name).append("\t").append(s.division).append("\t")
					.append(String.format("%.1f", s.attendance)).append("\t").append(String.format("%.2f", s.totalScore)).append("\n");
				shown++;
			}
		}
		if (shown == 0) sb.append("(none)\n");
		return sb.toString();
	}

	static void analyticsDashboard() throws Exception {
		String report = buildAnalyticsReport();
		System.out.println("\n" + report);
		String ans = readLine("Save report to file (" + REPORT_FILE + ")? (y/n): ").trim().toLowerCase();
		if (ans.equals("y") || ans.equals("yes")) {
			BufferedWriter w = new BufferedWriter(new FileWriter(REPORT_FILE, false));
			w.write(report);
			w.close();
			System.out.println("Report saved.");
		}
	}

	public static void main(String[] args) throws Exception {
		loadData();
		while (true) {
			System.out.println("\n===== MiniProject 30: Term Work Assessment =====");
			System.out.println("1. Add student");
			System.out.println("2. Update student");
			System.out.println("3. Delete student");
			System.out.println("4. Display student (by roll)");
			System.out.println("5. Display all students");
			System.out.println("6. Ranklist (Top N)");
			System.out.println("7. Division report");
			System.out.println("8. Analytics dashboard / export report");
			System.out.println("9. Exit");
			int ch = readInt("Enter choice: ");
			if (ch == 1) addStudent();
			else if (ch == 2) updateStudent();
			else if (ch == 3) deleteStudent();
			else if (ch == 4) displayStudent();
			else if (ch == 5) displayAll();
			else if (ch == 6) showRankList();
			else if (ch == 7) divisionReport();
			else if (ch == 8) analyticsDashboard();
			else if (ch == 9) break;
			else System.out.println("Invalid choice.");
		}
	}
}
