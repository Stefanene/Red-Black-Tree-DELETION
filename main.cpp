/*
  This is a Red-Black Tree Insertion and Deletion algorithm by Stefan Ene
   Works Cited:
    Using all the code from my RBT Insertion
    Deletion method implementation with help from https://www.geeksforgeeks.org/red-black-tree-set-3-delete-2/
*/

#include <iostream>
#include <cstring>
#include <fstream>
//colors
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define BLUE    "\033[34m"

#include "node.h"

using namespace std;

//struct Trunk for print method
struct Trunk {
  Trunk *prev;
  char* str;

  Trunk(Trunk *prev, char* str) {
    this -> prev = prev;
    this -> str = str;
  }
};

void showTrunks(Trunk *p) {//helper method for printing
  if (p == NULL)
    return;

  showTrunks(p -> prev);

  cout << p -> str;
}

//functions
void ADD(Node* &head, Node* &curr, Node*& prev, int val);
void READ(Node* &head);
void PRINT(Node* root, Trunk *prev, bool isLeft);
void parse(char* in, int* modif, int &count);
bool SEARCH(Node* curr, int val);
//functions for deletion
void DELETE(Node* &head, Node* &v);
Node* getSibling(Node* &x);
Node* successor(Node* &x);
Node* replaceNode(Node* &x);
void fixDoubleBlack(Node* &head, Node* &x);
bool hasRedChild(Node* &x);
void swapNodeValues(Node* &u, Node* &v);
//functions for balancing/fixing tree
void balance(Node* &head, Node* &curr);
void rotateLeft(Node* &head, Node* &curr);
void rotateRight(Node* &head, Node* &curr);

//MAIN function
int main() {
  //variables
  char input[10]; //user input
  char read[10000];  //file input
  int modif[100]; //parsed input for insertion
  Node* head = NULL;
  //program
  cout << "=========================" << endl;
  cout << "Welcome to my Red-Black Tree INSERTION." << endl;
  bool run = true;
  while (run) {
    //cout << RED << "hello world" << RESET << endl;
    //cout << BLUE << "YO" << RESET << endl;
    cout << endl << "You can: add, read, print, search, delete, and quit." << endl;
    cin.get(input, 10);
    cin.clear();
    cin.ignore(10000, '\n');
    if (strcmp(input, "add") == 0) {
      int val;
      cout << ">Add value: ";
      cin >> val;
      cin.clear();
      cin.ignore(10000, '\n');
      Node* curr = NULL;
      Node* prev = NULL;
      curr = head;
      ADD(head, curr, prev, val);
      if(curr != head) balance(head, curr);
      cout << endl << val << " has been added:" << endl;
    }
    else if (strcmp(input, "read") == 0) {
      //make sure arrays are clear
      READ(head);
    }
    else if (strcmp(input, "print") == 0) {
      cout << "=========================" << endl;
      PRINT(head, NULL, false);
      cout << "=========================" << endl;
    }
    else if (strcmp(input, "search") == 0) {
      int val;
      cout << ">Input value to search for: ";
      cin >> val;
      cin.clear();
      cin.ignore(10000, '\n');
      bool s = SEARCH(head, val);
      if (s == true) {
	cout << val << " is in the tree." << endl;
      } else {
	cout << val << " is NOT in the tree." << endl;
      }
    }
    else if (strcmp(input, "delete") == 0) {
      while (true) {
	int val = 0;
	cout << ">Input value to delete: ";
	cin >> val;
	cin.clear();
	cin.ignore(10000, '\n');
	bool d = SEARCH(head, val);
	if (d == false) {
	  cout << endl << "Value not found, try agian." << endl;
	} else {
	  //find node to be deleted
	  Node* v = head;
	  while (v->getData() != val) {
	    if (val < v->getData()) {
	      v = v->getLeft();
	    } else if (val > v->getData()) {
	      v = v->getRight();
	    }
	  }
	  DELETE(head, v);
	  break;
	}
      }
    }
    else if (strcmp(input, "quit") == 0) {
      cout << endl << "Thanks for using my program!" << endl;
      cout << "=========================" << endl;
      run = false;
    }
    else {
      cout << endl << "Invalid input. Try again." << endl;
    }
  }
  return 0;
}

//balance method to keep Red-Black properties of tree
//https://www.geeksforgeeks.org/c-program-red-black-tree-insertion/
void balance(Node* &head, Node* &curr) {
  Node* parent = NULL;
  Node* grandparent = NULL;
  while ((curr != head) && (curr->getColor() != 0) &&
	 ((curr->getParent())->getColor() == 1)) {
    parent = curr->getParent();
    grandparent = parent->getParent();
    //Case A: parent = left child of grandparent
    if (parent == grandparent->getLeft()) {
      Node* uncle = grandparent->getRight();
      //Case 1: uncle = red, then only recolor
      if (uncle != NULL && uncle->getColor() != 0) {
	grandparent->setColor(1);//red
	parent->setColor(0);//black
	uncle->setColor(0);//black
	curr = grandparent;
      }
      else {
	//Case 2: curr = right child of parent, then rotate left
	if (curr == parent->getRight()) {
	  rotateLeft(head, parent);
	  curr = parent;
	  parent =  curr->getParent();
	}
	//Case 3: curr - left child of parent, then rotate right
	rotateRight(head, grandparent);
	//swap colors of parent and grandparent
	int tempC = parent->getColor();
	parent->setColor(grandparent->getColor());
	grandparent->setColor(tempC);
	curr = parent;
      }
    }
    //Case B: parent = right child of grandparent
    else {
      Node* uncle = grandparent->getLeft();
      //Case 1: uncle = red, then onyl recolor
      if (uncle != NULL && uncle->getColor() != 0) {
	grandparent->setColor(1);//red
	parent->setColor(0);//black
	uncle->setColor(0);//black
	curr = grandparent;
      }
      else {
	//Case 2: curr = left child of parent, then rotate right
	if (curr == parent->getLeft()) {
	  rotateRight(head, parent);
	  curr = parent;
	  parent = curr->getParent();
	}
	//Case 3: curr = right child of parent, then rotate left
	rotateLeft(head, grandparent);
	//swap color of parent and grandparent
	int tempC = parent->getColor();
	parent->setColor(grandparent->getColor());
	grandparent->setColor(tempC);
	curr = parent;
      }
    }
  }
  head->setColor(0);
}

void rotateLeft(Node* &head, Node* &curr) {
  Node* rightPtr = curr->getRight(); 
  //begin rotation
  curr->setRight(rightPtr->getLeft()); 
  if (curr->getRight() != NULL) {
    (curr->getRight())->setParent(curr); 
  }
  rightPtr->setParent(curr->getParent());
  //if working with head
  if (curr->getParent() == NULL) {
    head = rightPtr; 
  } else if (curr == (curr->getParent())->getLeft()) {
    (curr->getParent())->setLeft(rightPtr); 
  } else {
    (curr->getParent())->setRight(rightPtr); 
  }  
  rightPtr->setLeft(curr); 
  curr->setParent(rightPtr);
}

void rotateRight(Node* &head, Node* & curr) {
  Node *leftPtr = curr->getLeft(); 
  //being rotation
  curr->setLeft(leftPtr->getRight()); 
  if (curr->getLeft() != NULL) {
    (curr->getLeft())->setParent(curr); 
  }  
  leftPtr->setParent(curr->getParent());
  //if working with head
  if (curr->getParent() == NULL) {
    head = leftPtr; 
  } else if (curr == (curr->getParent())->getLeft()) {
    (curr->getParent())->setLeft(leftPtr); 
  } else {
    (curr->getParent())->setRight(leftPtr);
  }
  leftPtr->setRight(curr); 
  curr->setParent(leftPtr);
}

//ADD method from my previous BTS project
void ADD(Node* &head, Node* &curr, Node*& prev, int val) {
  if (head == NULL) {
    head = new Node();
    curr = head;
    head->setData(val);
    head->setColor(0);  //head is always black
    return;
  } else {
    if (val < curr->getData()) {  //lower goes left
      prev = curr;
      curr = curr->getLeft();
      if (curr == NULL) {  //if empty then insert
	curr = new Node();
	curr->setData(val);
	prev->setLeft(curr);
	curr->setParent(prev);
	balance(head, curr);  //balance tree
	return;
      } else {  //if !empty then keep going
	ADD(head, curr, prev, val);
      }
    } else {
      prev = curr;
      curr = curr->getRight();  //higher goes right
      if (curr == NULL) {  //if empty then inset
	curr = new Node();
	curr->setData(val);
	prev->setRight(curr);
	curr->setParent(prev);
	balance(head, curr);  //balance tree
	return;
      } else {  //if !empty then keep going
	ADD(head, curr, prev, val);
      }
    }
  }
}

//READ function from file
void READ(Node* &head) {
  char in[10000];
  char fileName[100];
  int modif[100];
  int siz = 0;
  int count = 0;
  memset(in, 0, 10000);
  memset(fileName, 0, 100);
  memset(modif, 0, 100);
  //read in file name
  //Credit Omar Nassar's Heap project for file input code
  cout << endl << "What is the name of the file?" << endl << ">> ";
  cin.get(fileName, 30);
  cin.clear();
  cin.ignore(100000, '\n');
  streampos size;
  ifstream file(fileName, ios::in | ios::binary | ios::ate);
  if (file.is_open()) {
    size = file.tellg();
    file.seekg(0, ios::beg);
    file.read(in, size);
    file.close();
    parse(in, modif, count);  //parse input to modif array
    cout << "IN: ";
    for (int i = 0; i < 100; i++) {
      if (modif[i] != 0) {
	cout << modif[i] << " ";
	siz++;
      }
    }
    cout << endl << "Size: " << siz << endl;;
    //add to tree
    Node* curr = NULL;
    Node* prev = NULL;
    for (int i = 0; i < siz; i++) {
      if(modif[i] == 0) break;
      curr = head;
      ADD(head, curr, prev, modif[i]);
    }
  } 
}

//print functions from my previous BTS project
void PRINT(Node* root, Trunk *prev, bool isLeft) {
  if (root == NULL) {
    return;
  }
  char* prev_str = (char*)("    ");
  Trunk *trunk = new Trunk(prev, prev_str);
  PRINT(root->getLeft(), trunk, true);
  if (!prev) {
    trunk -> str = (char*)("---");
  }
  else if (isLeft) {
    trunk -> str = (char*)(".---");
    prev_str = (char*)("   |");
  }
  else {
    trunk -> str = (char*)("`---");
    prev -> str = prev_str;
  }
  showTrunks(trunk);
  if (root->getColor() == 0) {  //black
    cout << BLUE << root->getData() << RESET << endl;
  } else {  //red
    cout << RED << root->getData() << RESET << endl;
  }
  if (prev) {
    prev -> str = prev_str;
  }
  trunk -> str = (char*)("   |");
  PRINT(root->getRight(), trunk, false);
}

//parse method from my previous BTS project
void parse(char* in, int* modif, int &count) {
  int l = 0;  //keeps track of # of chars before space
  for (int i = 0; i < strlen(in); i++) {
    if (in[i] == ' ') {
      if (l == 1) {
	int temp = 0;
	temp = in[i-1] - '0';
	modif[count] = temp;
	count++;
	l = 0;
      } else {
	int temp = 0;
	for (int a = 0; a < l; a++) {
	  temp = 10 * temp + (in[i-l+a] - '0');
	}
	modif[count] = temp;
	count++;
	l = 0;
      }
    } else {
      int temp = 0;
      l++;
      if (i == strlen(in) - 1) {  //last possible pair of chars
	for (int a = 0; a < l; a++) {
	  temp = 10 * temp + (in[i+a+1-l] - '0');
	}
	modif[count] = temp;
	count++;
      }
    }
  } 
}

bool SEARCH(Node* curr, int val) {
  if (val == curr->getData()) {
    return true;
  }
  else if (val < curr->getData()) {  //lower goes left
    if (curr->getLeft() != NULL) {
      SEARCH(curr->getLeft(), val);  //recursion
    } else {
      return false;
    }
  }
  else {  //higher goes right
    if (curr->getRight() != NULL) {
      SEARCH(curr->getRight(), val);  //recursion
    } else {
      return false;
    }
  }
}

void swapValues(Node* &u, Node* &v) { 
  //swap int values between the two given nodes
  int temp; 
  temp = u->getData(); 
  u->setData(v->getData()); 
  v->setData(temp); 
}

Node* getSibling(Node* &v) {
  if (v->getParent() == NULL) {
    return NULL;
  }
  if (v == v->getParent()->getLeft()) {
    return v->getParent()->getRight();
  } else {
    return v->getParent()->getLeft();
  }
}

Node* successor(Node* &x) {
  //get left most value of right subtree
  Node* a = x;
  while (a->getLeft() != NULL) {
    a = a->getLeft();
  }
  return a;
}

Node* replaceNode(Node* &x) {
  //if node has 2 children 
  if (x->getLeft() != NULL && x->getRight() != NULL) {
    Node* r = x->getRight();
    return successor(r); 
  }
  //if node has no children 
  else if (x->getLeft() == NULL && x->getRight() == NULL) {
    return NULL;
  }
  //if node had one child 
  else {
    if (x->getLeft() != NULL) { 
      return x->getLeft();
    } else {
      return x->getRight();
    }
  }
}

void DELETE(Node* &head, Node* &v) {
  Node* u = replaceNode(v);
  Node* parent = v->getParent();
  //make bool that keeps track of both black
  bool bothBlack = ((u==NULL || u->getColor()==0) && (v==NULL || v->getColor()==0));

  if (u == NULL) {
    //then v has no children
    if (v == head) {
      //make head null
      head = NULL;
    } else {
      if (bothBlack) {
	fixDoubleBlack(head, v);
      } else {
	//one is red
	//then make sibling red
	if (getSibling(v) != NULL)
	  getSibling(v)->setColor(1);
      }
      //delete v from tree
      if (v == parent->getLeft()) {
	parent->setLeft(NULL);
      } else {
	parent->setRight(NULL);
      }
    }
    //delete v
    v->~Node();
    return;
  }

  //if v has 1 child
  if (v->getRight() == NULL || v->getLeft() == NULL) {
    if (v == head) {
      //assign value of u to v
      v->setData(u->getData());
      v->setLeft(NULL);
      v->setRight(NULL);
      //delete u
      u->~Node();
    } else {
      //detach v from tree and move u up
      if (v == parent->getLeft()) {
	parent->setLeft(u);
      } else {
	parent->setRight(u);
      }
      //delete v
      v->~Node();
      u->setParent(parent);
      if(bothBlack) {
	fixDoubleBlack(head, u);
      } else {
	//if one is red, color u black
	u->setColor(0);
      }
    }
    return;
  }

  //if program got here, v has two children
  //then, swap with successor and recurse
  swapValues(u, v);
  DELETE(head, u);
}

void fixDoubleBlack(Node* &head, Node* &x) {
  if (x == head)
    return;

  Node* sibling = getSibling(x);
  Node* parent = x->getParent();

  if (sibling == NULL) {
    //if no sibling, push doublebalck up
    fixDoubleBlack(head, parent);
  } else {
    if (sibling->getColor() == 1) {
      //silbing is red
      parent->setColor(1); //red
      sibling->setColor(0); //black
      if (sibling == parent->getLeft()) {
	rotateRight(head, parent);
      } else {
	rotateLeft(head, parent);
      }
      fixDoubleBlack(head, x);
    } else {
      //sibling is black
      if (hasRedChild(sibling)) {
	//has at least 1 red child
	if (sibling->getLeft() != NULL && sibling->getLeft()->getColor() == 1) {
	  //sibling's left child is red
	  if (sibling == parent->getLeft()) {
	    //left left
	    sibling->getLeft()->setColor(sibling->getColor());
	    sibling->setColor(parent->getColor());
	    rotateRight(head, parent);
	  } else {
	    //right left
	    sibling->getLeft()->setColor(parent->getColor());
	    rotateRight(head, sibling);
	    rotateLeft(head, parent);
	  }
	} else {
	  //sibling's right child is red
	  if (sibling == parent->getLeft()) {
	    //left right
	    sibling->getRight()->setColor(parent->getColor());
	    rotateLeft(head, sibling);
	    rotateRight(head, parent);
	  } else {
	    //right right
	    sibling->getRight()->setColor(sibling->getColor());
	    sibling->setColor(parent->getColor());
	    rotateLeft(head, parent);
	  }
	}
	parent->setColor(0); //black
      } else {
	//two black children
	sibling->setColor(1); //red
	if (parent->getColor() == 0) {
	  fixDoubleBlack(head, parent); //recursion
	} else {
	  parent->setColor(0); //black
	}
      }
    }
  }
}

bool hasRedChild(Node* &x) { 
  if (x->getLeft() != NULL && x->getLeft()->getColor() == 1) {
    return true;
  } else if (x->getRight() != NULL && x->getRight()->getColor() == 1) {
    return true;
  } else {
    return false;
  }
}
