// 26. Assume we have two input and two output tapes to perform the sorting. The internal memory can hold and sort m records at a time. Write a program in java for external sorting. Find out time complexity.

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.InputStreamReader;

public class F26 {
	static final String INPUT_FILE = "F26_Input.txt";
	static final String T1 = "F26_T1.txt";
	static final String T2 = "F26_T2.txt";
	static final String T3 = "F26_T3.txt";
	static final String T4 = "F26_T4.txt";
	static final String OUTPUT_FILE = "F26_Sorted.txt";

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

	static Integer readNextInt(BufferedReader r) throws Exception {
		while (true) {
			String line = r.readLine();
			if (line == null) return null;
			line = line.trim();
			if (line.length() == 0) continue;
			try {
				return Integer.valueOf(Integer.parseInt(line));
			} catch (Exception e) {
				// skip malformed line
			}
		}
	}

	static int countRecords(String path) throws Exception {
		File f = new File(path);
		if (!f.exists()) return 0;
		BufferedReader r = new BufferedReader(new FileReader(f));
		int n = 0;
		while (readNextInt(r) != null) n++;
		r.close();
		return n;
	}

	static boolean fileHasAnyInt(String path) throws Exception {
		File f = new File(path);
		if (!f.exists()) return false;
		BufferedReader r = new BufferedReader(new FileReader(f));
		boolean ok = (readNextInt(r) != null);
		r.close();
		return ok;
	}

	static void displayFile(String path, String title) throws Exception {
		System.out.println("\n--- " + title + " ---");
		File f = new File(path);
		if (!f.exists()) {
			System.out.println("(file not found)");
			return;
		}
		BufferedReader r = new BufferedReader(new FileReader(f));
		int shown = 0;
		Integer x;
		while ((x = readNextInt(r)) != null) {
			System.out.print(x + " ");
			shown++;
			if (shown % 20 == 0) System.out.println();
		}
		r.close();
		if (shown == 0) System.out.print("(empty)");
		System.out.println("\nTotal records: " + shown);
	}

	static void selectionSort(int[] arr, int n) {
		for (int i = 0; i < n - 1; i++) {
			int minIdx = i;
			for (int j = i + 1; j < n; j++) {
				if (arr[j] < arr[minIdx]) minIdx = j;
			}
			int t = arr[i];
			arr[i] = arr[minIdx];
			arr[minIdx] = t;
		}
	}

	static void createInputFile() throws Exception {
		int n = readInt("Enter number of records (integers): ");
		if (n < 0) n = 0;
		BufferedWriter out = new BufferedWriter(new FileWriter(INPUT_FILE, false));
		System.out.println("Enter " + n + " integers:");
		for (int i = 0; i < n; i++) {
			int x = readInt("  value: ");
			out.write(Integer.toString(x));
			out.newLine();
		}
		out.close();
		System.out.println("Input file created: " + INPUT_FILE);
	}

	static void truncateFile(String path) throws Exception {
		new BufferedWriter(new FileWriter(path, false)).close();
	}

	static void distributeRuns(int m) throws Exception {
		File f = new File(INPUT_FILE);
		if (!f.exists()) {
			System.out.println("Input file not found. Create it first.");
			return;
		}
		BufferedReader in = new BufferedReader(new FileReader(f));
		BufferedWriter out1 = new BufferedWriter(new FileWriter(T1, false));
		BufferedWriter out2 = new BufferedWriter(new FileWriter(T2, false));
		int[] buf = new int[m];
		boolean toggle = true;

		while (true) {
			int k = 0;
			while (k < m) {
				Integer x = readNextInt(in);
				if (x == null) break;
				buf[k++] = x.intValue();
			}
			if (k == 0) break;
			selectionSort(buf, k);
			BufferedWriter out = toggle ? out1 : out2;
			for (int i = 0; i < k; i++) {
				out.write(Integer.toString(buf[i]));
				out.newLine();
			}
			toggle = !toggle;
		}
		in.close();
		out1.close();
		out2.close();
	}

	static boolean readNextInRun(BufferedReader in, int runSize, int[] countRead, int[] valueOut) throws Exception {
		if (countRead[0] >= runSize) return false;
		Integer x = readNextInt(in);
		if (x == null) return false;
		countRead[0]++;
		valueOut[0] = x.intValue();
		return true;
	}

	static void mergePass(String inAPath, String inBPath, String outAPath, String outBPath, int runSize) throws Exception {
		BufferedReader inA = new BufferedReader(new FileReader(inAPath));
		BufferedReader inB = new BufferedReader(new FileReader(inBPath));
		BufferedWriter outA = new BufferedWriter(new FileWriter(outAPath, false));
		BufferedWriter outB = new BufferedWriter(new FileWriter(outBPath, false));
		boolean toggle = true;

		while (true) {
			int[] countA = new int[] { 0 };
			int[] countB = new int[] { 0 };
			int[] aVal = new int[1];
			int[] bVal = new int[1];
			boolean hasA = readNextInRun(inA, runSize, countA, aVal);
			boolean hasB = readNextInRun(inB, runSize, countB, bVal);
			if (!hasA && !hasB) break;

			BufferedWriter out = toggle ? outA : outB;
			while (hasA || hasB) {
				if (!hasB || (hasA && aVal[0] <= bVal[0])) {
					out.write(Integer.toString(aVal[0]));
					out.newLine();
					hasA = readNextInRun(inA, runSize, countA, aVal);
				} else {
					out.write(Integer.toString(bVal[0]));
					out.newLine();
					hasB = readNextInRun(inB, runSize, countB, bVal);
				}
			}
			toggle = !toggle;
		}

		inA.close();
		inB.close();
		outA.close();
		outB.close();
	}

	static void copyFile(String src, String dst) throws Exception {
		BufferedReader in = new BufferedReader(new FileReader(src));
		BufferedWriter out = new BufferedWriter(new FileWriter(dst, false));
		Integer x;
		while ((x = readNextInt(in)) != null) {
			out.write(x.toString());
			out.newLine();
		}
		in.close();
		out.close();
	}

	static void externalSortMenu() throws Exception {
		int n = countRecords(INPUT_FILE);
		if (n <= 0) {
			System.out.println("Input file is empty or missing.");
			return;
		}
		int m = readInt("Enter m (records that fit in memory): ");
		if (m <= 0) {
			System.out.println("m must be positive.");
			return;
		}

		System.out.println("\nExternal Sort (2 input + 2 output tapes)");
		System.out.println("Records n = " + n + ", Memory m = " + m);

		distributeRuns(m);

		int runSize = m;
		int mergePasses = 0;
		String inA = T1, inB = T2, outA = T3, outB = T4;

		while (runSize < n) {
			mergePass(inA, inB, outA, outB, runSize);
			// swap tapes
			String tmpA = inA;
			inA = outA;
			outA = tmpA;
			String tmpB = inB;
			inB = outB;
			outB = tmpB;
			truncateFile(outA);
			truncateFile(outB);
			runSize *= 2;
			mergePasses++;
		}

		String finalTape = fileHasAnyInt(inA) ? inA : inB;
		copyFile(finalTape, OUTPUT_FILE);
		System.out.println("\nSorted output written to: " + OUTPUT_FILE);

		int totalPasses = 1 + mergePasses;
		System.out.println("Merge passes: " + mergePasses + ", Total passes (including initial): " + totalPasses);
		System.out.println("Time complexity (approx): O(n * log2(ceil(n/m))) merges, and O(n * passes) I/O.");
	}

	public static void main(String[] args) throws Exception {
		while (true) {
			System.out.println("\n===== Group F26 (External Sorting) =====");
			System.out.println("1. Create/overwrite input file");
			System.out.println("2. Display input file");
			System.out.println("3. Perform external sort");
			System.out.println("4. Display sorted output file");
			System.out.println("5. Exit");
			int ch = readInt("Enter choice: ");
			if (ch == 1) createInputFile();
			else if (ch == 2) displayFile(INPUT_FILE, "Input File");
			else if (ch == 3) externalSortMenu();
			else if (ch == 4) displayFile(OUTPUT_FILE, "Sorted Output");
			else if (ch == 5) break;
			else System.out.println("Invalid choice.");
		}
	}
}
