#pragma once

namespace TwoThree {
template<class T>
struct node {
  int size;  // количество занятых ключей
  T key[3];
  node<T> *first;   // *first <= key[0];
  node<T> *second;  // key[0] <= *second < key[1];
  node<T> *third;   // key[1] <= *third < key[2];
  node<T> *fourth;  // key[2] <= *fourth.
  node<T> *parent;  //Указатель на родителя нужен для того, потому что адрес корня
                 //может меняться при удалении

  bool find(T k) {  // Этот метод возвращает true, если ключ k находится в
                      // вершине, иначе false.
    for (int i = 0; i < size; ++i)
      if (key[i] == k) return true;
    return false;
  }

  int find_n(T k) {  // Этот метод возвращает true, если ключ k находится в
                      // вершине, иначе false.
    for (int i = 0; i < size; ++i)
      if (key[i] == k) return i;
    return -1;
  }

  void swap(T &x, T &y) {
    int r = x;
    x = y;
    y = r;
  }

  void sort2(T &x, T &y) {
    if (x > y) swap(x, y);
  }

  void sort3(T &x, T &y, T &z) {
    if (x > y) swap(x, y);
    if (x > z) swap(x, z);
    if (y > z) swap(y, z);
  }

  void sort() {  // Ключи в вершинах должны быть отсортированы
    if (size == 1) return;
    if (size == 2) sort2(key[0], key[1]);
    if (size == 3) sort3(key[0], key[1], key[2]);
  }

  void insert_to_node(T k) {  // Вставляем ключ k в вершину (не в дерево)
    key[size] = k;
    size++;
    sort();
  }

  void remove_from_node(T k) {  // Удаляем ключ k из вершины (не из дерева)
    if (size >= 1 && key[0] == k) {
      key[0] = key[1];
      key[1] = key[2];
      size--;
    } else if (size == 2 && key[1] == k) {
      key[1] = key[2];
      size--;
    }
  }

  void become_node2(T k, node<T> *first_,
                    node<T> *second_) {  // Преобразовать в 2-вершину.
    key[0] = k;
    first = first_;
    second = second_;
    third = nullptr;
    fourth = nullptr;
    parent = nullptr;
    size = 1;
  }

  bool is_leaf() {  // Является ли узел листом; проверка используется при
                    // вставке и удалении.
    return (first == nullptr) && (second == nullptr) && (third == nullptr);
  }

  // Создавать всегда будем вершину только с одним ключом
  node(T k)
      : size(1),
        key{k, 0, 0},
        first(nullptr),
        second(nullptr),
        third(nullptr),
        fourth(nullptr),
        parent(nullptr) {}

  node(T k, node<T> *first_, node<T> *second_, node<T> *third_, node<T> *fourth_,
       node<T> *parent_)
      : size(1),
        key{k, 0, 0},
        first(first_),
        second(second_),
        third(third_),
        fourth(fourth_),
        parent(parent_) {}

  friend node<T> *split(
      node<T> *item);  // Метод для разделение вершины при переполнении;
  friend node<T> *insert(node<T> *p, T k);  // Вставка в дерево;
  friend node<T> *search(node<T> *p, T k);  // Поиск в дереве;
  friend node<T> *search_min(node<T> *p);  // Поиск минимального элемента в поддереве;
  friend node<T> *merge(node<T> *leaf);  // Слияние используется при удалении;
  friend node<T> *redistribute(
      node<T> *leaf);  // Перераспределение также используется при удалении;
  friend node<T> *fix(
      node<T> *leaf);  // Используется после удаления для возвращения свойств
                    // дереву (использует merge или redistribute)
  friend node<T> *remove(node<T> *p, T k);  // Собственно, из названия понятно;
};

template <class T>
node<T> *insert(node<T> *p,
             T k) {  // вставка ключа k в дерево с корнем p; всегда возвращаем
                       // корень дерева, т.к. он может меняться
  if (!p)
    return new node<T>(
        k);  // если дерево пусто, то создаем первую 2-3-вершину (корень)

  if (p->is_leaf())
    p->insert_to_node(k);
  else if (k <= p->key[0])
    insert(p->first, k);
  else if ((p->size == 1) || ((p->size == 2) && k <= p->key[1]))
    insert(p->second, k);
  else
    insert(p->third, k);

  return split(p);
}

template <class T>
node<T> *split(node<T> *item) {
  if (item->size < 3) return item;

  node<T> *x = new node<T>(item->key[0], item->first, item->second, nullptr, nullptr,
                     item->parent);  // Создаем две новые вершины,
  node<T> *y = new node<T>(item->key[2], item->third, item->fourth, nullptr, nullptr,
                     item->parent);  // которые имеют такого же родителя, как и
                                     // разделяющийся элемент.
  if (x->first)
    x->first->parent = x;  // Правильно устанавливаем "родителя" "сыновей".
  if (x->second)
    x->second->parent =
        x;  // После разделения, "родителем" "сыновей" является "дедушка",
  if (y->first)
    y->first->parent = y;  // Поэтому нужно правильно установить указатели.
  if (y->second) y->second->parent = y;

  if (item->parent) {
    item->parent->insert_to_node(item->key[1]);

    if (item->parent->first == item)
      item->parent->first = nullptr;
    else if (item->parent->second == item)
      item->parent->second = nullptr;
    else if (item->parent->third == item)
      item->parent->third = nullptr;

    // Дальше происходит своеобразная сортировка ключей при разделении.
    if (item->parent->first == nullptr) {
      item->parent->fourth = item->parent->third;
      item->parent->third = item->parent->second;
      item->parent->second = y;
      item->parent->first = x;
    } else if (item->parent->second == nullptr) {
      item->parent->fourth = item->parent->third;
      item->parent->third = y;
      item->parent->second = x;
    } else {
      item->parent->fourth = y;
      item->parent->third = x;
    }

    node<T> *tmp = item->parent;
    delete item;
    return tmp;
  } else {
    x->parent = item;  // Так как в эту ветку попадает только корень,
    y->parent =
        item;  // то мы "родителем" новых вершин делаем разделяющийся элемент.
    item->become_node2(item->key[1], x, y);
    return item;
  }
}

template <class T>
node<T> *search(node<T> *p, T k) {  // Поиск ключа k в 2-3 дереве с корнем p.
  if (!p) return nullptr;

  if (p->find(k))
    return p;
  else if (k < p->key[0])
    return search(p->first, k);
  else if ((p->size == 2) && (k < p->key[1]) || (p->size == 1))
    return search(p->second, k);
  else if (p->size == 2)
    return search(p->third, k);
}

template <class T>
void findPreSuc(node<T> *root, node<T> *&pre, node<T> *&suc, T key) {
  // Base case
  if (root == NULL) return;

  // If key is present at root
  if (root->key == key) {
    // the maximum value in left subtree is predecessor
    if (root->left != NULL) {
      node<T> *tmp = root->left;
      while (tmp->right) tmp = tmp->right;
      pre = tmp;
    }

    // the minimum value in right subtree is successor
    if (root->right != NULL) {
      node<T> *tmp = root->right;
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

template <class T>
node<T> *search_min(
    node<T> *p) {  // Поиск узла с минимальным элементов в 2-3-дереве с корнем p.
  if (!p) return p;
  if (!(p->first))
    return p;
  else
    return search_min(p->first);
}

template <class T>
node<T> *remove(node<T> *p, T k) {  // Удаление ключа k в 2-3-дереве с корнем p.
  node<T> *item = search(p, k);  // Ищем узел, где находится ключ k

  if (!item) return p;

  node<T> *min = nullptr;
  if (item->key[0] == k)
    min = search_min(item->second);  // Ищем эквивалентный ключ
  else
    min = search_min(item->third);

  if (min) {  // Меняем ключи местами
    int &z = (k == item->key[0] ? item->key[0] : item->key[1]);
    item->swap(z, min->key[0]);
    item = min;  // Перемещаем указатель на лист, т.к. min - всегда лист
  }

  item->remove_from_node(k);  // И удаляем требуемый ключ из листа
  return fix(item);  // Вызываем функцию для восстановления свойств дерева.
}

template <class T>
node<T> *fix(node<T> *leaf) {
  if (leaf->size == 0 &&
      leaf->parent ==
          nullptr) {  // Случай 0, когда удаляем единственный ключ в дереве
    delete leaf;
    return nullptr;
  }
  if (leaf->size !=
      0) {  // Случай 1, когда вершина, в которой удалили ключ, имела два ключа
    if (leaf->parent)
      return fix(leaf->parent);
    else
      return leaf;
  }

  node<T> *parent = leaf->parent;
  if (parent->first->size == 2 || parent->second->size == 2 ||
      parent->size == 2)
    leaf = redistribute(
        leaf);  // Случай 2, когда достаточно перераспределить ключи в дереве
  else if (parent->size == 2 && parent->third->size == 2)
    leaf = redistribute(leaf);  // Аналогично
  else
    leaf =
        merge(leaf);  // Случай 3, когда нужно произвести склеивание и пройтись
                      // вверх по дереву как минимум на еще одну вершину

  return fix(leaf);
}

template <class T>
node<T> *redistribute(node<T> *leaf) {
  node<T> *parent = leaf->parent;
  node<T> *first = parent->first;
  node<T> *second = parent->second;
  node<T> *third = parent->third;

  if ((parent->size == 2) && (first->size < 2) && (second->size < 2) &&
      (third->size < 2)) {
    if (first == leaf) {
      parent->first = parent->second;
      parent->second = parent->third;
      parent->third = nullptr;
      parent->first->insert_to_node(parent->key[0]);
      parent->first->third = parent->first->second;
      parent->first->second = parent->first->first;

      if (leaf->first != nullptr)
        parent->first->first = leaf->first;
      else if (leaf->second != nullptr)
        parent->first->first = leaf->second;

      if (parent->first->first != nullptr)
        parent->first->first->parent = parent->first;

      parent->remove_from_node(parent->key[0]);
      delete first;
    } else if (second == leaf) {
      first->insert_to_node(parent->key[0]);
      parent->remove_from_node(parent->key[0]);
      if (leaf->first != nullptr)
        first->third = leaf->first;
      else if (leaf->second != nullptr)
        first->third = leaf->second;

      if (first->third != nullptr) first->third->parent = first;

      parent->second = parent->third;
      parent->third = nullptr;

      delete second;
    } else if (third == leaf) {
      second->insert_to_node(parent->key[1]);
      parent->third = nullptr;
      parent->remove_from_node(parent->key[1]);
      if (leaf->first != nullptr)
        second->third = leaf->first;
      else if (leaf->second != nullptr)
        second->third = leaf->second;

      if (second->third != nullptr) second->third->parent = second;

      delete third;
    }
  } else if ((parent->size == 2) &&
             ((first->size == 2) || (second->size == 2) ||
              (third->size == 2))) {
    if (third == leaf) {
      if (leaf->first != nullptr) {
        leaf->second = leaf->first;
        leaf->first = nullptr;
      }

      leaf->insert_to_node(parent->key[1]);
      if (second->size == 2) {
        parent->key[1] = second->key[1];
        second->remove_from_node(second->key[1]);
        leaf->first = second->third;
        second->third = nullptr;
        if (leaf->first != nullptr) leaf->first->parent = leaf;
      } else if (first->size == 2) {
        parent->key[1] = second->key[0];
        leaf->first = second->second;
        second->second = second->first;
        if (leaf->first != nullptr) leaf->first->parent = leaf;

        second->key[0] = parent->key[0];
        parent->key[0] = first->key[1];
        first->remove_from_node(first->key[1]);
        second->first = first->third;
        if (second->first != nullptr) second->first->parent = second;
        first->third = nullptr;
      }
    } else if (second == leaf) {
      if (third->size == 2) {
        if (leaf->first == nullptr) {
          leaf->first = leaf->second;
          leaf->second = nullptr;
        }
        second->insert_to_node(parent->key[1]);
        parent->key[1] = third->key[0];
        third->remove_from_node(third->key[0]);
        second->second = third->first;
        if (second->second != nullptr) second->second->parent = second;
        third->first = third->second;
        third->second = third->third;
        third->third = nullptr;
      } else if (first->size == 2) {
        if (leaf->second == nullptr) {
          leaf->second = leaf->first;
          leaf->first = nullptr;
        }
        second->insert_to_node(parent->key[0]);
        parent->key[0] = first->key[1];
        first->remove_from_node(first->key[1]);
        second->first = first->third;
        if (second->first != nullptr) second->first->parent = second;
        first->third = nullptr;
      }
    } else if (first == leaf) {
      if (leaf->first == nullptr) {
        leaf->first = leaf->second;
        leaf->second = nullptr;
      }
      first->insert_to_node(parent->key[0]);
      if (second->size == 2) {
        parent->key[0] = second->key[0];
        second->remove_from_node(second->key[0]);
        first->second = second->first;
        if (first->second != nullptr) first->second->parent = first;
        second->first = second->second;
        second->second = second->third;
        second->third = nullptr;
      } else if (third->size == 2) {
        parent->key[0] = second->key[0];
        second->key[0] = parent->key[1];
        parent->key[1] = third->key[0];
        third->remove_from_node(third->key[0]);
        first->second = second->first;
        if (first->second != nullptr) first->second->parent = first;
        second->first = second->second;
        second->second = third->first;
        if (second->second != nullptr) second->second->parent = second;
        third->first = third->second;
        third->second = third->third;
        third->third = nullptr;
      }
    }
  } else if (parent->size == 1) {
    leaf->insert_to_node(parent->key[0]);

    if (first == leaf && second->size == 2) {
      parent->key[0] = second->key[0];
      second->remove_from_node(second->key[0]);

      if (leaf->first == nullptr) leaf->first = leaf->second;

      leaf->second = second->first;
      second->first = second->second;
      second->second = second->third;
      second->third = nullptr;
      if (leaf->second != nullptr) leaf->second->parent = leaf;
    } else if (second == leaf && first->size == 2) {
      parent->key[0] = first->key[1];
      first->remove_from_node(first->key[1]);

      if (leaf->second == nullptr) leaf->second = leaf->first;

      leaf->first = first->third;
      first->third = nullptr;
      if (leaf->first != nullptr) leaf->first->parent = leaf;
    }
  }
  return parent;
}

template <class T>
node<T> *merge(node<T> *leaf) {
  node<T> *parent = leaf->parent;

  if (parent->first == leaf) {
    parent->second->insert_to_node(parent->key[0]);
    parent->second->third = parent->second->second;
    parent->second->second = parent->second->first;

    if (leaf->first != nullptr)
      parent->second->first = leaf->first;
    else if (leaf->second != nullptr)
      parent->second->first = leaf->second;

    if (parent->second->first != nullptr)
      parent->second->first->parent = parent->second;

    parent->remove_from_node(parent->key[0]);
    delete parent->first;
    parent->first = nullptr;
  } else if (parent->second == leaf) {
    parent->first->insert_to_node(parent->key[0]);

    if (leaf->first != nullptr)
      parent->first->third = leaf->first;
    else if (leaf->second != nullptr)
      parent->first->third = leaf->second;

    if (parent->first->third != nullptr)
      parent->first->third->parent = parent->first;

    parent->remove_from_node(parent->key[0]);
    delete parent->second;
    parent->second = nullptr;
  }

  if (parent->parent == nullptr) {
    node *tmp = nullptr;
    if (parent->first != nullptr)
      tmp = parent->first;
    else
      tmp = parent->second;
    tmp->parent = nullptr;
    delete parent;
    return tmp;
  }
  return parent;
}
}  // namespace TwoThree