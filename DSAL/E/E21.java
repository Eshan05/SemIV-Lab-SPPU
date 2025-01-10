// 21. Implement the Heap/Shell sort algorithm implemented in Java demonstrating heap/shell data structure with modularity of programming language

import java.io.BufferedReader;
import java.io.InputStreamReader;

public class E21 {
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

	static void heapSortAsc(int[] a, int n) {
		for (int i = n / 2 - 1; i >= 0; i--) heapifyMax(a, n, i);
		for (int end = n - 1; end > 0; end--) {
			int t = a[0];
			a[0] = a[end];
			a[end] = t;
			heapifyMax(a, end, 0);
		}
	}

	static void shellSortAsc(int[] a, int n) {
		for (int gap = n / 2; gap > 0; gap /= 2) {
			for (int i = gap; i < n; i++) {
				int temp = a[i];
				int j = i;
				while (j >= gap && a[j - gap] > temp) {
					a[j] = a[j - gap];
					j -= gap;
				}
				a[j] = temp;
			}
		}
	}

	public static void main(String[] args) throws Exception {
		int[] data = new int[MAXN];
		int n = 0;
		boolean loaded = false;

		while (true) {
			System.out.println("\n===== Group E21 (Heap Sort / Shell Sort) =====");
			System.out.println("1. Input array");
			System.out.println("2. Display array");
			System.out.println("3. Heap sort (ascending)");
			System.out.println("4. Shell sort (ascending)");
			System.out.println("5. Exit");

			int ch = readInt("Enter choice: ");
			if (ch == 1) {
				n = readInt("Enter number of elements (1-200): ");
				if (n < 1) {
					System.out.println("Need at least 1 element.");
					n = 0;
					loaded = false;
					continue;
				}
				if (n > MAXN) {
					System.out.println("Clamping to MAXN=" + MAXN);
					n = MAXN;
				}
				for (int i = 0; i < n; i++) {
					data[i] = readInt("a[" + i + "] = ");
				}
				loaded = true;
				System.out.println("Array loaded.");
			} else if (ch == 2) {
				if (!loaded) System.out.println("Input array first.");
				else displayArray(data, n);
			} else if (ch == 3) {
				if (!loaded) {
					System.out.println("Input array first.");
					continue;
				}
				int[] work = new int[n];
				copyArray(data, work, n);
				heapSortAsc(work, n);
				System.out.print("Sorted (Heap sort): ");
				displayArray(work, n);
			} else if (ch == 4) {
				if (!loaded) {
					System.out.println("Input array first.");
					continue;
				}
				int[] work = new int[n];
				copyArray(data, work, n);
				shellSortAsc(work, n);
				System.out.print("Sorted (Shell sort): ");
				displayArray(work, n);
			} else if (ch == 5) {
				break;
			} else {
				System.out.println("Invalid choice.");
			}
		}
	}
}
