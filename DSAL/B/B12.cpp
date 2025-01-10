// 12. Implement a file compression algorithm that uses binary tree. Your program should allow the user to compress and decompress messages containing alphabets using the standard Huffman algorithm for encoding and decoding.

#include <iostream>
#include <string>
using namespace std;

struct Node {
  char ch;
  int freq;
  Node *left;
  Node *right;
};

Node *createNode(char c, int f) {
  Node *n = new Node;
  n->ch = c;
  n->freq = f;
  n->left = 0;
  n->right = 0;
  return n;
}

int isLeaf(Node *n) {
  return n != 0 && n->left == 0 && n->right == 0;
}

void heapSwap(Node *&a, Node *&b) {
  Node *t = a;
  a = b;
  b = t;
}

void heapifyUp(Node **heap, int idx) {
  while (idx > 1) {
    int p = idx / 2;
    if (heap[p]->freq <= heap[idx]->freq) break;
    heapSwap(heap[p], heap[idx]);
    idx = p;
  }
}

void heapifyDown(Node **heap, int size, int idx) {
  while (true) {
    int l = idx * 2;
    int r = l + 1;
    int smallest = idx;
    if (l <= size && heap[l]->freq < heap[smallest]->freq) smallest = l;
    if (r <= size && heap[r]->freq < heap[smallest]->freq) smallest = r;
    if (smallest == idx) break;
    heapSwap(heap[idx], heap[smallest]);
    idx = smallest;
  }
}

void heapPush(Node **heap, int &size, Node *node) {
  heap[++size] = node;
  heapifyUp(heap, size);
}

Node *heapPop(Node **heap, int &size) {
  if (size == 0) return 0;
  Node *top = heap[1];
  heap[1] = heap[size--];
  if (size > 0) heapifyDown(heap, size, 1);
  return top;
}

Node *buildHuffmanTree(const string &msg) {
  int freq[128];
  for (int i = 0; i < 128; ++i) freq[i] = 0;

  for (int i = 0; i < static_cast<int>(msg.size()); ++i) {
    unsigned char c = static_cast<unsigned char>(msg[i]);
    if (c < 128) ++freq[c];
  }

  Node *heap[300];
  int heapSize = 0;
  for (int i = 0; i < 128; ++i) {
    if (freq[i] > 0) heapPush(heap, heapSize, createNode(static_cast<char>(i), freq[i]));
  }

  if (heapSize == 0) return 0;
  if (heapSize == 1) {
    Node *single = heapPop(heap, heapSize);
    Node *root = createNode('\0', single->freq);
    root->left = single;
    return root;
  }

  while (heapSize > 1) {
    Node *a = heapPop(heap, heapSize);
    Node *b = heapPop(heap, heapSize);
    Node *p = createNode('\0', a->freq + b->freq);
    p->left = a;
    p->right = b;
    heapPush(heap, heapSize, p);
  }

  return heapPop(heap, heapSize);
}

void buildCodes(Node *root, string path, string codes[128]) {
  if (root == 0) return;
  if (isLeaf(root)) {
    unsigned char c = static_cast<unsigned char>(root->ch);
    if (path == "") path = "0";
    codes[c] = path;
    return;
  }
  buildCodes(root->left, path + "0", codes);
  buildCodes(root->right, path + "1", codes);
}

string encodeMessage(const string &msg, string codes[128]) {
  string bits = "";
  for (int i = 0; i < static_cast<int>(msg.size()); ++i) {
    unsigned char c = static_cast<unsigned char>(msg[i]);
    if (c < 128) bits += codes[c];
  }
  return bits;
}

string decodeBits(Node *root, const string &bits) {
  string out = "";
  if (root == 0) return out;
  Node *cur = root;
  for (int i = 0; i < static_cast<int>(bits.size()); ++i) {
    if (bits[i] == '0') cur = cur->left;
    else if (bits[i] == '1') cur = cur->right;
    else continue;

    if (cur != 0 && isLeaf(cur)) {
      out += cur->ch;
      cur = root;
    }
  }
  return out;
}

void printCodes(string codes[128]) {
  cout << "\nHuffman Codes:\n";
  for (int i = 0; i < 128; ++i) {
    if (codes[i] != "") {
      char c = static_cast<char>(i);
      if (c == ' ') cout << "[space] : " << codes[i] << "\n";
      else cout << c << " : " << codes[i] << "\n";
    }
  }
}

void deleteTree(Node *root) {
  if (root == 0) return;
  deleteTree(root->left);
  deleteTree(root->right);
  delete root;
}

int main() {
  Node *root = 0;
  string codes[128];
  string lastEncoded = "";

  while (true) {
    cout << "\n1. Compress message\n";
    cout << "2. Decompress last encoded message\n";
    cout << "3. Decompress custom bit string\n";
    cout << "4. Exit\n";
    cout << "Enter choice: ";

    int ch;
    cin >> ch;

    if (ch == 1) {
      cout << "Enter message (single_word): ";
      string msg;
      cin >> msg;

      if (root != 0) {
        deleteTree(root);
        root = 0;
      }
      for (int i = 0; i < 128; ++i) codes[i] = "";

      root = buildHuffmanTree(msg);
      if (root == 0) {
        cout << "Empty/invalid message\n";
        continue;
      }

      buildCodes(root, "", codes);
      printCodes(codes);

      lastEncoded = encodeMessage(msg, codes);
      cout << "Encoded bit string: " << lastEncoded << "\n";
      cout << "Original length (approx bits, 8-bit chars): " << (8 * static_cast<int>(msg.size())) << "\n";
      cout << "Compressed length (bits): " << static_cast<int>(lastEncoded.size()) << "\n";

    } else if (ch == 2) {
      if (root == 0 || lastEncoded == "") {
        cout << "No encoded data available\n";
      } else {
        cout << "Decoded message: " << decodeBits(root, lastEncoded) << "\n";
      }

    } else if (ch == 3) {
      if (root == 0) {
        cout << "Build Huffman tree first using option 1\n";
      } else {
        cout << "Enter bit string: ";
        string bits;
        cin >> bits;
        cout << "Decoded message: " << decodeBits(root, bits) << "\n";
      }

    } else if (ch == 4) {
      break;

    } else {
      cout << "Invalid choice\n";
    }
  }

  if (root != 0) deleteTree(root);
  return 0;
}
