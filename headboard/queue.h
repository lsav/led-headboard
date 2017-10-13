// a super basic queue backed by a linkedlist
// class does not manage memory lifecycle of objects stored
template <typename T>
class queue {
  public:
    queue();
    T* front();
    void enqueue(T* t);
    void pop();
    bool empty();
    int size();
  private:
    // backing data structure
    class Node {
      public:
        T* data;
        Node* next;
    };
    Node* head;
    Node* tail;
    int length;
};

template <typename T>
queue<T>::queue() : head(NULL), tail(NULL), length(0) {}

template <typename T>
T* queue<T>::front() {
  if (head == NULL) {
    return NULL;
  }
  return head->data;
}

// append t onto the end of the queue
template <typename T>
void queue<T>::enqueue(T* t) {
  Node* n = new Node();
  n->data = t;
  n->next = NULL;

  if (empty()) {
    head = n;
    tail = n;
  } else {
    tail->next = n;
    tail = n;
  }

  length++;
}

// destroy the element at the front of the queue
template <typename T>
void queue<T>::pop() {
  if (empty()) {
    return;
  }

  Node* tmp = head;
  head = head->next;
  if (head == NULL) {
    tail = NULL;
  }
  delete tmp;
  length--;

}

template <typename T>
bool queue<T>::empty() {
  return length == 0;
}

template <typename T>
int queue<T>::size() {
  return length;
}
