#pragma once

namespace AVL {
template <class T>
struct node  // структура для представления узлов дерева
{
  T key;
  unsigned char height;
  node<T>* left;
  node<T>* right;
  node(T k) {
    key = k;
    left = right = 0;
    height = 1;
  }
};

template <class T>
unsigned char height(node<T>* p) {
  return p ? p->height : 0;
}

template <class T>
int bfactor(node<T>* p) {
  return height(p->right) - height(p->left);
}

template <class T>
void fixheight(node<T>* p) {
  unsigned char hl = height(p->left);
  unsigned char hr = height(p->right);
  p->height = (hl > hr ? hl : hr) + 1;
}

template <class T>
node<T>* rotateright(node<T>* p)  // правый поворот вокруг p
{
  node<T>* q = p->left;
  p->left = q->right;
  q->right = p;
  fixheight(p);
  fixheight(q);
  return q;
}

template <class T>
node<T>* rotateleft(node<T>* q)  // левый поворот вокруг q
{
  node<T>* p = q->right;
  q->right = p->left;
  p->left = q;
  fixheight(q);
  fixheight(p);
  return p;
}

template <class T>
node<T>* balance(node<T>* p)  // балансировка узла p
{
  fixheight(p);
  if (bfactor(p) == 2) {
    if (bfactor(p->right) < 0) p->right = rotateright(p->right);
    return rotateleft(p);
  }
  if (bfactor(p) == -2) {
    if (bfactor(p->left) > 0) p->left = rotateleft(p->left);
    return rotateright(p);
  }
  return p;  // балансировка не нужна
}

template <class T>
node<T>* insert(node<T>* p, T k)  // вставка ключа k в дерево с корнем p
{
  if (!p) return new node<T>(k);
  if (k < p->key)
    p->left = insert(p->left, k);
  else
    p->right = insert(p->right, k);
  return balance(p);
}

template <class T>
node<T>* findmin(node<T>* p)  // поиск узла с минимальным ключом в дереве p
{
  return p->left ? findmin(p->left) : p;
}

template <class T>
node<T>* removemin(
    node<T>* p)  // удаление узла с минимальным ключом из дерева p
{
  if (p->left == 0) return p->right;
  p->left = removemin(p->left);
  return balance(p);
}

template <class T>
node<T>* remove(node<T>* p, T k)  // удаление ключа k из дерева p
{
  if (!p) return 0;
  if (k < p->key)
    p->left = remove(p->left, k);
  else if (k > p->key)
    p->right = remove(p->right, k);
  else  //  k == p->key
  {
    node<T>* q = p->left;
    node<T>* r = p->right;
    delete p;
    if (!r) return q;
    node<T>* min = findmin(r);
    min->right = removemin(r);
    min->left = q;
    return balance(min);
  }
  return balance(p);
}

// This function finds predecessor and successor of key in BST.
// It sets pre and suc as predecessor and successor respectively
template <class T>
void findPreSuc(node<T>* root, node<T>*& pre, node<T>*& suc, T key) {
  // Base case
  if (root == NULL) return;

  // If key is present at root
  if (root->key == key) {
    // the maximum value in left subtree is predecessor
    if (root->left != NULL) {
      node<T>* tmp = root->left;
      while (tmp->right) tmp = tmp->right;
      pre = tmp;
    }

    // the minimum value in right subtree is successor
    if (root->right != NULL) {
      node<T>* tmp = root->right;
      while (tmp->left) tmp = tmp->left;
      suc = tmp;
    }
    return;
  }

  // If key is smaller than root's key, go to left subtree
  if (root->key > key) {
    suc = root;
    findPreSuc(root->left, pre, suc, key);
  } else  // go to right subtree
  {
    pre = root;
    findPreSuc(root->right, pre, suc, key);
  }
}


// C function to search a given key in a given BST
template <class T>
node<T>* search(node<T>* root, T key) {
  // Base Cases: root is null or key is present at root
  if (root == NULL || root->key == key) return root;

  // Key is greater than root's key
  if (root->key < key) return search(root->right, key);

  // Key is smaller than root's key
  return search(root->left, key);
}
}  // namespace AVL