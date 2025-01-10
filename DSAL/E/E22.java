// 22. Read the marks obtained by students of second year in an online examination of particular subject. Find out maximum and minimum marks obtained in that subject. Use heap data structure. Analyze the algorithm

import java.io.BufferedReader;
import java.io.InputStreamReader;

public class E22 {
	static final int MAXN = 200;
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

	static void copyArray(int[] src, int[] dst, int n) {
		for (int i = 0; i < n; i++) dst[i] = src[i];
	}

	static void displayArray(int[] a, int n) {
		if (n <= 0) {
			System.out.println("(empty)");
			return;
		}
		for (int i = 0; i < n; i++) {
			System.out.print(a[i]);
			if (i + 1 < n) System.out.print(" ");
		}
		System.out.println();
	}

	static void heapifyMax(int[] a, int n, int i) {
		int largest = i;
		int l = 2 * i + 1;
		int r = 2 * i + 2;
		if (l < n && a[l] > a[largest]) largest = l;
		if (r < n && a[r] > a[largest]) largest = r;
		if (largest != i) {
			int t = a[i];
			a[i] = a[largest];
			a[largest] = t;
			heapifyMax(a, n, largest);
		}
	}

	static void buildMaxHeap(int[] a, int n) {
		for (int i = n / 2 - 1; i >= 0; i--) heapifyMax(a, n, i);
	}

	static void heapifyMin(int[] a, int n, int i) {
		int smallest = i;
		int l = 2 * i + 1;
		int r = 2 * i + 2;
		if (l < n && a[l] < a[smallest]) smallest = l;
		if (r < n && a[r] < a[smallest]) smallest = r;
		if (smallest != i) {
			int t = a[i];
			a[i] = a[smallest];
			a[smallest] = t;
			heapifyMin(a, n, smallest);
		}
	}

	static void buildMinHeap(int[] a, int n) {
		for (int i = n / 2 - 1; i >= 0; i--) heapifyMin(a, n, i);
	}

	public static void main(String[] args) throws Exception {
		int[] marks = new int[MAXN];
		int n = 0;
		boolean loaded = false;

		while (true) {
			System.out.println("\n===== Group E22 (Min/Max Marks using Heap) =====");
			System.out.println("1. Input marks");
			System.out.println("2. Display marks");
			System.out.println("3. Find maximum (using max-heap)");
			System.out.println("4. Find minimum (using min-heap)");
			System.out.println("5. Find both max & min");
			System.out.println("6. Complexity note");
			System.out.println("7. Exit");

			int ch = readInt("Enter choice: ");
			if (ch == 1) {
				n = readInt("Enter number of students (1-200): ");
				if (n < 1) {
					System.out.println("Need at least 1 student.");
					n = 0;
					loaded = false;
					continue;
				}
				if (n > MAXN) {
					System.out.println("Clamping to MAXN=" + MAXN);
					n = MAXN;
				}
				for (int i = 0; i < n; i++) {
					marks[i] = readInt("Marks of student " + (i + 1) + ": ");
				}
				loaded = true;
				System.out.println("Marks loaded.");
			} else if (ch == 2) {
				if (!loaded) System.out.println("Input marks first.");
				else displayArray(marks, n);
			} else if (ch == 3) {
				if (!loaded) {
					System.out.println("Input marks first.");
					continue;
				}
				int[] heap = new int[n];
				copyArray(marks, heap, n);
				buildMaxHeap(heap, n);
				System.out.println("Maximum marks = " + heap[0]);
			} else if (ch == 4) {
				if (!loaded) {
					System.out.println("Input marks first.");
					continue;
				}
				int[] heap = new int[n];
				copyArray(marks, heap, n);
				buildMinHeap(heap, n);
				System.out.println("Minimum marks = " + heap[0]);
			} else if (ch == 5) {
				if (!loaded) {
					System.out.println("Input marks first.");
					continue;
				}
				int[] heapMax = new int[n];
				int[] heapMin = new int[n];
				copyArray(marks, heapMax, n);
				copyArray(marks, heapMin, n);
				buildMaxHeap(heapMax, n);
				buildMinHeap(heapMin, n);
				System.out.println("Maximum marks = " + heapMax[0]);
				System.out.println("Minimum marks = " + heapMin[0]);
			} else if (ch == 6) {
				System.out.println("\nAnalysis");
				System.out.println("Build heap: O(n)");
				System.out.println("Find max/min from heap root: O(1)");
				System.out.println("Space used by heap array: O(n)");
			} else if (ch == 7) {
				break;
			} else {
				System.out.println("Invalid choice.");
			}
		}
	}
}
