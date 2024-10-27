# DSAL

## Group A (Only Python)

1. Consider telephone book database of N clients. Make use of a hash table implementation to quickly look up client‘s telephone number. Make use of two collision handling techniques and compare them using number of comparisons required to find a set of telephone numbers
2. Implement all the functions of a dictionary (ADT) using hashing and handle collisions using chaining with / without replacement. Data: Set of (key, value) pairs, Keys are mapped to values, Keys must be comparable, Keys must be unique. Standard Operations: Insert(key, value), Find(key), Delete(key)
3. For given set of elements create skip list. Find the element in the set that is closest to some given value. (Note: Decide the level of element in the list Randomly with some upper limit)
4. To create ADT that implement the "set" concept.
   1. Add (New Element) Place a value into the set
   2. Remove (Element) Remove the value
   3. Contains (Element) Return true if element is in collection
   4. Size () Return number of values in collection
   5. Iterator () Return an iterator used to loop over collection
   6. Intersection of two sets
   7. Union of two sets
   8. Difference between two sets
   9. Subset
  
## Group B

5. A book consists of chapters, chapters consist of sections and sections consist of subsections. Construct a tree and print the nodes. Find the time and space requirements of your method.
6. Beginning with an empty binary search tree, Construct binary search tree by inserting the values in the order given. After constructing a binary tree
   1. Insert new node
   2. Find number of nodes in longest path from root
   3. Minimum data value found in the tree
   4. Change a tree so that the roles of the left and right pointers are swapped at every node
   5. Search a value
7. Construct an expression tree from the given prefix expression [Ex: `+--a*bc/def`] and traverse it using post order traversal (Non-recursive) and then delete the entire tree.
8. Read for the formulas in propositional calculus. Write a function that reads such a formula and creates its binary tree representation. What is the complexity of your function?
9. Convert given binary tree into threaded binary tree. Analyze time and space complexity of the algorithm.
10. Consider threading a binary tree using preorder threads rather than inorder threads. Design an algorithm for traversal without using stack and analyze its complexity
11. A Dictionary stores keywords and its meanings. Provide facility for adding new keywords, deleting keywords, updating values of any entry. Provide facility to display whole data sorted in ascending/ Descending order. Also find how many maximum comparisons may require for finding any keyword. Use Binary Search Tree for implementation.
12. Implement a file compression algorithm that uses binary tree. Your program should allow the user to compress and decompress messages containing alphabets using the standard Huffman algorithm for encoding and decoding.

## Group C

13. Represent a given graph using adjacency matrix/list to perform DFS and using adjacency list to perform BFS. Use the map of the area around the college as the graph. Identify the prominent land marks as nodes and perform DFS and BFS on that.
14. There are flight paths between cities. If there is a flight between city A and city B then there is an edge between the cities. The cost of the edge can be the time that flight take to reach city B from A, or the amount of fuel used for the journey. Represent this as a graph. The node can be represented by airport name or name of the city. Use adjacency list representation of the graph or use adjacency matrix representation of the graph. Check whether the graph is connected or not. Justify the storage representation used.
15. You have a business with several offices; you want to lease phone lines to connect them up with each other; and the phone company charges different amounts of money to connect different pairs of cities. You want a set of lines that connects all your offices with a minimum total cost. Solve the problem by suggesting appropriate data structures. 
16. Tour operator organizes guided bus trips across the Maharashtra. Tourists may have different preferences. Tour operator offers a choice from many different routes. Every day the bus moves from starting city S to another city F as chosen by client. On this way, the tourists can see the sights alongside the route traveled from S to F. Client may have preference to choose route. There is a restriction on the routes that the tourists may choose from, the bus has to take a short route from S to F or a route having one distance unit longer than the minimal distance. Two routes from S to F are considered different if there is at least one road from a city A to a city B which is part of one route, but not of the other route.
17. Consider the scheduling problem. n tasks to be scheduled on single processor. Let $t_1, \dots ,t_n$ be durations required to execute on single processor is known. The tasks can be executed in any order but one task at a time. Design a greedy algorithm for this problem and find a schedule that minimizes the total time spent by all the tasks in the system. (The time spent by one is the sum of the waiting time of task and the time spent on its execution.)

## Group D

18. Given sequence $k = k_1 < k_2 < \dots <k_n$ of $n$ sorted keys, with a search probability $\pi$ for each key $k_i$ . Build the Binary search tree that has the least search cost given the access probability for each key 
19. A Dictionary stores keywords and its meanings. Provide facility for adding new keywords, deleting keywords, updating values of any entry. Provide facility to display whole data sorted in ascending/ Descending order. Also find how many maximum comparisons may require for finding any keyword. Use Height balance tree and find the complexity for finding a keyword

## Group E

20. Consider a scenario for Hospital to cater services to different kinds of patients as 
    1.  Serious (Top priority), 
    2.  Non-serious (Medium priority), 
    3.  General Checkup (Least priority).

    Implement the priority queue to cater services to the patients

21. Implement the Heap/Shell sort algorithm implemented in Java demonstrating heap/shell data structure with modularity of programming language
22. Read the marks obtained by students of second year in an online examination of particular subject. Find out maximum and minimum marks obtained in that subject. Use heap data structure. Analyze the algorithm

## Group F

23. Department maintains a student information. The file contains roll number, name, division and address. Allow user to add, delete information of student. Display information of particular employee. If record of student does not exist an appropriate message is displayed. If it is, then the system displays the student details. Use sequential file to main the data.
24. Company maintains employee information as employee ID, name, designation and salary. Allow user to add, delete information of employee. Display information of particular employee. If employee does not exist an appropriate message is displayed. If it is, then the system displays the employee details. Use index sequential file to maintain the data.
25. Implementation of a direct access file -Insertion and deletion of a record from a direct access file
26. Assume we have two input and two output tapes to perform the sorting. The internal memory can hold and sort m records at a time. Write a program in java for external sorting. Find out time complexity.

## Mini Project / Case Study

27. Design a mini project using JAVA which will use the different data structure with or without Java collection library and show the use of specific data structure on the efficiency (Performance) of the code.
28. Design a mini project to implement Snake and Ladders Game using Python.
29. Design a mini project to implement a Smart text editor.
30. Design a mini project for automated Term work assessment of student based on parameters like daily attendance, Unit Test / Prelim performance, Students achievements if any, Mock Practical.