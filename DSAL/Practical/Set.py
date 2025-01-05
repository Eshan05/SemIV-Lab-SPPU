class Set:
    def __init__(self, capacity):
        self.elements = []
        self.capacity = capacity

    def __str__(self):
        return "{" + ", ".join(map(str, self.elements)) + "}"

    def __len__(self):
        return len(self.elements)

    def __contains__(self, element):
        return element in self.elements

    def add(self, element):
        if len(self.elements) < self.capacity and element not in self.elements:
            self.elements.append(element)
        elif element in self.elements:
            print(f"{element} is already in the set.")
        else:
            print("Set is full. Cannot add more elements.")

    def remove(self, element):
        if element in self.elements:
            self.elements.remove(element)
        else:
            print(f"{element} is not in the set.")

    def intersect(self, other_set):
        intersection_set = Set(self.capacity) # Assuming capacity of first set for simplicity
        for element in self.elements:
            if element in other_set:
                intersection_set.add(element)
        return intersection_set

    def union(self, other_set):
        union_set = Set(self.capacity + other_set.capacity) # Assuming combined capacity for simplicity
        for element in self.elements:
            union_set.add(element)
        for element in other_set.elements:
            union_set.add(element)
        return union_set

    def difference(self, other_set):
        difference_set = Set(self.capacity) # Assuming capacity of first set for simplicity
        for element in self.elements:
            if element not in other_set:
                difference_set.add(element)
        return difference_set

    def isSubsetOf(self, other_set):
        for element in self.elements:
            if element not in other_set:
                return False
        return True

    def isProperSubset(self, other_set):
        if len(self) >= len(other_set):
            return False
        return self.isSubsetOf(other_set)

def createSet():
    n = int(input("Enter Number of Elements in Set "))
    s = Set(n)
    return s

choice = 0
print("Creating Set A")
s1 = createSet()
print(str(s1))

while choice != 10:
    print("\nMenu")
    print("1. Add to Set A")
    print("2. Remove from Set A")
    print("3. Contains in Set A")
    print("4. Size of Set A")
    print("5. Intersection with Set B")
    print("6. Union with Set B")
    print("7. Difference with Set B (A - B)")
    print("8. Is Set A a Subset of Set B?")
    print("9. Is Set A a Proper Subset of Set B?")
    print("10. Exit")

    choice = int(input("Please Enter Your Choice "))

    if choice == 1:
        e = int(input("Enter Number To Add to Set A: "))
        s1.add(e)
        print(str(s1))
    elif choice == 2:
        e = int(input("Enter Number To Remove from Set A: "))
        s1.remove(e)
        print(str(s1))
    elif choice == 3:
        e = int(input("Enter Number to Search in Set A: "))
        if e in s1:
            print("Number is present in Set A")
        else:
            print("Number is not present in Set A")
    elif choice == 4:
        print("Set A contains {} Elements".format(len(s1)))
    elif choice == 5:
        print("Create a Set B for Intersection Operation")
        s2 = createSet()
        s3 = s1.intersect(s2)
        print("Set A =", str(s1))
        print("Set B =", str(s2))
        print("Intersection =", str(s3))
    elif choice == 6:
        print("Create a Set B for Union Operation")
        s2 = createSet()
        s3 = s1.union(s2)
        print("Set A =", str(s1))
        print("Set B =", str(s2))
        print("Union =", str(s3))
    elif choice == 7:
        print("Create a Set B for calculating Set Difference (A - B)")
        s2 = createSet()
        s3 = s1.difference(s2)
        print("Set A =", str(s1))
        print("Set B =", str(s2))
        print("Difference (A - B) =", str(s3))
    elif choice == 8:
        print("Create a Set B for checking if Set A is a Subset of Set B")
        s2 = createSet()
        isSubset = s1.isSubsetOf(s2)
        print("Set A =", str(s1))
        print("Set B =", str(s2))
        if isSubset:
            print("Set A is a Subset of Set B")
        else:
            print("Set A is not a Subset of Set B")
    elif choice == 9:
        print("Create a Set B for checking if Set A is a Proper Subset of Set B")
        s2 = createSet()
        isProperSubset = s1.isProperSubset(s2)
        print("Set A =", str(s1))
        print("Set B =", str(s2))
        if isProperSubset:
            print("Set A is a Proper Subset of Set B")
        else:
            print("Set A is not a Proper Subset of Set B")
    elif choice == 10:
        break
    else:
        print("Please Enter A Valid Choice")