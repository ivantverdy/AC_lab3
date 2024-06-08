#ifndef SPLAYTREE_H
#define SPLAYTREE_H

#include <complex>
#include <iostream>
#include <string>
#include <stack>

template<typename T>
class SplayTree {
private:
    using Complex = std::complex<double>;

    struct SplayNode {
        SplayNode *leftChild;
        SplayNode *rightChild;
        SplayNode *parent;
        Complex data;

        SplayNode(const Complex &key = Complex())
                : leftChild(nullptr), rightChild(nullptr), parent(nullptr), data(key) {}

        ~SplayNode() = default; // Destructor simplified, no recursive calls here
    } *root;

private:
    SplayNode *_Minimum(SplayNode *localRoot) const {
        SplayNode *minimum = localRoot;

        while (minimum->leftChild != nullptr)
            minimum = minimum->leftChild;

        return minimum;
    }

    SplayNode *_Maximum(SplayNode *localRoot) const {
        SplayNode *maximum = localRoot;

        while (maximum->rightChild != nullptr)
            maximum = maximum->rightChild;

        return maximum;
    }

    SplayNode *_Search(const Complex &key) {
        SplayNode *searchedElement = root;

        while (searchedElement != nullptr) {
            if (searchedElement->data.imag() < key.imag() ||
                (searchedElement->data.imag() == key.imag() && searchedElement->data.real() < key.real())) {
                searchedElement = searchedElement->rightChild;
            } else if (key.imag() < searchedElement->data.imag() ||
                       (key.imag() == searchedElement->data.imag() && key.real() < searchedElement->data.real())) {
                searchedElement = searchedElement->leftChild;
            } else {
                _Splay(searchedElement);
                return searchedElement;
            }
        }

        return nullptr;
    }

    void _LeftRotate(SplayNode *localRoot) {
        SplayNode *rightChild = localRoot->rightChild;

        localRoot->rightChild = rightChild->leftChild;
        if (rightChild->leftChild != nullptr)
            rightChild->leftChild->parent = localRoot;

        _Transplant(localRoot, rightChild);

        rightChild->leftChild = localRoot;
        rightChild->leftChild->parent = rightChild;
    }

    void _RightRotate(SplayNode *localRoot) {
        SplayNode *leftChild = localRoot->leftChild;

        localRoot->leftChild = leftChild->rightChild;
        if (leftChild->rightChild != nullptr)
            leftChild->rightChild->parent = localRoot;

        _Transplant(localRoot, leftChild);

        leftChild->rightChild = localRoot;
        leftChild->rightChild->parent = leftChild;
    }

    void _Transplant(SplayNode *localParent, SplayNode *localChild) {
        if (localParent->parent == nullptr)
            root = localChild;
        else if (localParent == localParent->parent->leftChild)
            localParent->parent->leftChild = localChild;
        else
            localParent->parent->rightChild = localChild;

        if (localChild != nullptr)
            localChild->parent = localParent->parent;
    }

    void _Splay(SplayNode *pivotElement) {
        while (pivotElement != root) {
            if (pivotElement->parent == root) {
                if (pivotElement == pivotElement->parent->leftChild) {
                    _RightRotate(pivotElement->parent);
                } else {
                    _LeftRotate(pivotElement->parent);
                }
            } else {
                SplayNode *parent = pivotElement->parent;
                SplayNode *grandparent = parent->parent;

                // Zig-Zig
                if (pivotElement == parent->leftChild && parent == grandparent->leftChild) {
                    _RightRotate(grandparent);
                    _RightRotate(parent);
                } else if (pivotElement == parent->rightChild && parent == grandparent->rightChild) {
                    _LeftRotate(grandparent);
                    _LeftRotate(parent);
                }
                    // Zig-Zag
                else if (pivotElement == parent->rightChild && parent == grandparent->leftChild) {
                    _LeftRotate(parent);
                    _RightRotate(grandparent);
                } else if (pivotElement == parent->leftChild && parent == grandparent->rightChild) {
                    _RightRotate(parent);
                    _LeftRotate(grandparent);
                }
            }
        }
    }

    void _PrintTree(SplayNode* root, int depth = 0) const {
        if (root == nullptr) {
            std::cout << std::string(4 * depth, ' ') << "- <empty>" << std::endl;
            return;
        }

        _PrintTree(root->rightChild, depth + 1);

        std::cout << std::string(4 * depth, ' ') << "- " << root->data << std::endl;

        _PrintTree(root->leftChild, depth + 1);
    }

public:
    SplayTree() : root(nullptr) {}

    ~SplayTree() {
        std::stack<SplayNode*> nodes;
        if (root != nullptr) {
            nodes.push(root);
        }

        while (!nodes.empty()) {
            SplayNode* current = nodes.top();
            nodes.pop();

            if (current->leftChild != nullptr) {
                nodes.push(current->leftChild);
            }
            if (current->rightChild != nullptr) {
                nodes.push(current->rightChild);
            }

            delete current;
        }
        root = nullptr; // Reset root to nullptr after deleting all nodes
    }

    void Insert(const Complex &key) {
        SplayNode *preInsertPlace = nullptr;
        SplayNode *insertPlace = root;

        while (insertPlace != nullptr) {
            preInsertPlace = insertPlace;

            if (insertPlace->data.imag() < key.imag() ||
                (insertPlace->data.imag() == key.imag() && insertPlace->data.real() < key.real())) {
                insertPlace = insertPlace->rightChild;
            } else {
                insertPlace = insertPlace->leftChild;
            }
        }

        SplayNode *insertElement = new SplayNode(key);
        insertElement->parent = preInsertPlace;

        if (preInsertPlace == nullptr)
            root = insertElement;
        else if (preInsertPlace->data.imag() < insertElement->data.imag() ||
                 (preInsertPlace->data.imag() == insertElement->data.imag() && preInsertPlace->data.real() < insertElement->data.real())) {
            preInsertPlace->rightChild = insertElement;
        } else {
            preInsertPlace->leftChild = insertElement;
        }

        _Splay(insertElement);
    }

    void Remove(const Complex &key) {
        SplayNode *removeElement = _Search(key);

        if (removeElement != nullptr) {
            if (removeElement->rightChild == nullptr)
                _Transplant(removeElement, removeElement->leftChild);
            else if (removeElement->leftChild == nullptr)
                _Transplant(removeElement, removeElement->rightChild);
            else {
                SplayNode *newLocalRoot = _Minimum(removeElement->rightChild);

                if (newLocalRoot->parent != removeElement) {
                    _Transplant(newLocalRoot, newLocalRoot->rightChild);
                    newLocalRoot->rightChild = removeElement->rightChild;
                    newLocalRoot->rightChild->parent = newLocalRoot;
                }

                _Transplant(removeElement, newLocalRoot);

                newLocalRoot->leftChild = removeElement->leftChild;
                newLocalRoot->leftChild->parent = newLocalRoot;

                _Splay(newLocalRoot);
            }

            delete removeElement;
        }
    }

    bool Search(const Complex &key) { return _Search(key) != nullptr; }

    bool isEmpty() const { return root == nullptr; }

    Complex Successor(const Complex &key) {
        SplayNode *node = _Search(key);
        if (node) {
            SplayNode *successorNode = _Minimum(node->rightChild);
            if (successorNode) return successorNode->data;
        }
        return Complex();
    }

    Complex Predecessor(const Complex &key) {
        SplayNode *node = _Search(key);
        if (node) {
            SplayNode *predecessorNode = _Maximum(node->leftChild);
            if (predecessorNode) return predecessorNode->data;
        }
        return Complex();
    }

    void PrintTree() const {
        _PrintTree(root);
    }
};

void printMenu() {
    std::cout << "1. Insert" << std::endl;
    std::cout << "2. Find" << std::endl;
    std::cout << "3. Erase" << std::endl;
    std::cout << "4. Show Tree" << std::endl;
    std::cout << "5. Exit" << std::endl;
    std::cout << "Enter your choice: ";
}

int main() {
    SplayTree<std::complex<double>> tree;
    int choice;
    double real, imag;

    do {
        printMenu();
        std::cin >> choice;

        switch (choice) {
            case 1:
                std::cout << "Enter real part: ";
                std::cin >> real;
                std::cout << "Enter imaginary part: ";
                std::cin >> imag;
                tree.Insert(std::complex<double>(real, imag));
                break;
            case 2:
                std::cout << "Enter real part: ";
                std::cin >> real;
                std::cout << "Enter imaginary part: ";
                std::cin >> imag;
                if (tree.Search(std::complex<double>(real, imag))) {
                    std::cout << "Element found." << std::endl;
                    tree.PrintTree();
                } else {
                    std::cout << "Element not found." << std::endl;
                }
                break;
            case 3:
                std::cout << "Enter real part: ";
                std::cin >> real;
                std::cout << "Enter imaginary part: ";
                std::cin >> imag;
                tree.Remove(std::complex<double>(real, imag));
                break;
            case 4:
                tree.PrintTree();
                break;
            case 5:
                std::cout << "Exit" << std::endl;
                break;
            default:
                std::cout << "Wrong choice" << std::endl;
        }
    } while (choice != 5);

    return 0;
}

#endif // SPLAYTREE_H
