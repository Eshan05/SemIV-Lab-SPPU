# 4. To create ADT that implement the "set" concept.
#   1. Add (New Element) Place a value into the set
#   2. Remove (Element) Remove the value
#   3. Contains (Element) Return true if element is in collection
#   4. Size () Return number of values in collection
#   5. Iterator () Return an iterator used to loop over collection
#   6. Intersection of two sets
#   7. Union of two sets
#   8. Difference between two sets
#   9. Subset

class SetIterator:
    def __init__(self, data, count):
        self.data = data
        self.count = count
        self.current = 0

    def has_next(self):
        return self.current < self.count

    def next(self):
        if self.current < self.count:
            val = self.data[self.current]
            self.current += 1
            return val
        return None

class SetADT:
    def __init__(self, capacity=100):
        self.capacity = capacity
        self.data = [None] * capacity
        self.count = 0

    def _binary_search(self, key):
        low = 0
        high = self.count - 1
        while low <= high:
            mid = (low + high) // 2
            if self.data[mid] == key:
                return mid
            elif self.data[mid] < key:
                low = mid + 1
            else:
                high = mid - 1
        return -1

    def _find_insert_position(self, key):
        low = 0
        high = self.count - 1
        while low <= high:
            mid = (low + high) // 2
            if self.data[mid] < key:
                low = mid + 1
            else:
                high = mid - 1
        return low

    def add(self, element):
        if self.count >= self.capacity:
            print("Set is full")
            return False

        if self.contains(element):
            return False

        pos = self._find_insert_position(element)

        i = self.count
        while i > pos:
            self.data[i] = self.data[i - 1]
            i -= 1

        self.data[pos] = element
        self.count += 1
        return True

    def remove(self, element):
        index = self._binary_search(element)
        if index == -1:
            return False

        i = index
        while i < self.count - 1:
            self.data[i] = self.data[i + 1]
            i += 1

        self.data[self.count - 1] = None
        self.count -= 1
        return True

    def contains(self, element):
        return self._binary_search(element) != -1

    def size(self):
        return self.count

    def iterator(self):
        return SetIterator(self.data, self.count)

    def display(self):
        print("{", end=" ")
        for i in range(self.count):
            print(self.data[i], end=" ")
        print("}")

    def union(self, other_set):
        result = SetADT(self.capacity + other_set.capacity)
        i = 0
        j = 0

        while i < self.count and j < other_set.count:
            if self.data[i] < other_set.data[j]:
                result.add(self.data[i])
                i += 1
            elif self.data[i] > other_set.data[j]:
                result.add(other_set.data[j])
                j += 1
            else:
                result.add(self.data[i])
                i += 1
                j += 1

        while i < self.count:
            result.add(self.data[i])
            i += 1

        while j < other_set.count:
            result.add(other_set.data[j])
            j += 1

        return result

    def intersection(self, other_set):
        result = SetADT(min(self.capacity, other_set.capacity))
        i = 0
        j = 0

        while i < self.count and j < other_set.count:
            if self.data[i] < other_set.data[j]:
                i += 1
            elif self.data[i] > other_set.data[j]:
                j += 1
            else:
                result.add(self.data[i])
                i += 1
                j += 1

        return result

    def difference(self, other_set):
        result = SetADT(self.capacity)
        i = 0
        j = 0

        while i < self.count and j < other_set.count:
            if self.data[i] < other_set.data[j]:
                result.add(self.data[i])
                i += 1
            elif self.data[i] > other_set.data[j]:
                j += 1
            else:
                i += 1
                j += 1

        while i < self.count:
            result.add(self.data[i])
            i += 1

        return result

    def subset(self, other_set):
        i = 0
        j = 0

        while i < self.count and j < other_set.count:
            if self.data[i] < other_set.data[j]:
                return False
            elif self.data[i] > other_set.data[j]:
                j += 1
            else:
                i += 1
                j += 1

        return i == self.count

def main():
    setA = SetADT()
    setB = SetADT()

    while True:
        print("\n1. Add to Set A")
        print("2. Add to Set B")
        print("3. Remove from Set A")
        print("4. Remove from Set B")
        print("5. Contains in Set A")
        print("6. Size of Sets")
        print("7. Iterator (Display Set A)")
        print("8. Union (A U B)")
        print("9. Intersection (A n B)")
        print("10. Difference (A - B)")
        print("11. Subset (A subset of B?)")
        print("12. Display Both Sets")
        print("13. Exit")

        choice = input("Enter choice: ")

        if choice == '1':
            val = int(input("Enter element: "))
            if setA.add(val):
                print("Added")
            else:
                print("Duplicate or Full")
        elif choice == '2':
            val = int(input("Enter element: "))
            if setB.add(val):
                print("Added")
            else:
                print("Duplicate or Full")
        elif choice == '3':
            val = int(input("Enter element: "))
            if setA.remove(val):
                print("Removed")
            else:
                print("Not Found")
        elif choice == '4':
            val = int(input("Enter element: "))
            if setB.remove(val):
                print("Removed")
            else:
                print("Not Found")
        elif choice == '5':
            val = int(input("Enter element: "))
            if setA.contains(val):
                print("Found")
            else:
                print("Not Found")
        elif choice == '6':
            print("Size A:", setA.size())
            print("Size B:", setB.size())
        elif choice == '7':
            it = setA.iterator()
            print("Iterating Set A:")
            while it.has_next():
                print(it.next(), end=" ")
            print()
        elif choice == '8':
            res = setA.union(setB)
            print("Union:", end=" ")
            res.display()
        elif choice == '9':
            res = setA.intersection(setB)
            print("Intersection:", end=" ")
            res.display()
        elif choice == '10':
            res = setA.difference(setB)
            print("Difference (A-B):", end=" ")
            res.display()
        elif choice == '11':
            if setA.subset(setB):
                print("Set A is a subset of Set B")
            else:
                print("Set A is NOT a subset of Set B")
        elif choice == '12':
            print("Set A:", end=" ")
            setA.display()
            print("Set B:", end=" ")
            setB.display()
        elif choice == '13':
            break
        else:
            print("Invalid choice")

main()