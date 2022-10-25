#include <iostream>
#include <random>
#include <ctime>
#include <vector>
#include <list>
#include <string>
#include <chrono>
using namespace std;

const int k_inf = INT_MAX;

class adjacency_matrix
{   
   int** matrix;
public:
    adjacency_matrix(size_t size)
    {
        std::mt19937 mt(time(NULL));
        int middle = mt();
    }
};

struct ADJ
{
    int name;
    int weight;
    ADJ(int _name = 0, int _weight = 0):name(_name),weight(_weight)
    {}
};
//struct vicinity 
//{
//    ADJ* adj;
//    vicinity* next;
//    vicinity(int _name, int _weight) {
//        adj = new ADJ{ _name,_weight };
//        next = nullptr;
//    }
//    ADJ* operator->() 
//    {
//        return adj;
//    }
//};
//struct vicinity_iterator
//{
//    vicinity** it;
//    vicinity_iterator(vicinity* v) {
//        it = &v;
//    }
//    void go_next() 
//    {
//        it = &((*it)->next);
//    }
//    bool end()
//    {
//        return(*it) == nullptr;
//    }
//};
class Graph
{
    size_t _size;
    list<ADJ>* graph; 
    std::list<ADJ>::iterator vicinity_it; // Итератор по окрестности вершины
    size_t pointer; // указатель на номер вершины
public:
    Graph(int** weights, size_t size): _size(size)
    {
        graph = new list<ADJ>[size];
        for (int i = 0; i < size; i++)
        {
            list<ADJ> tmp;
            for (int j = 0; j < size; j++)
            {
                if (weights[i][j] != k_inf && weights[i][j] != 0) {
                    tmp.push_back(ADJ{ j,weights[i][j] });
                }
            }
            graph[i] = tmp;
        }
        start_explore_vicinity(0);
    }
    ~Graph()
    {
        delete[] graph;
    }
    size_t size() const
    {
        return _size;
    }
    void start_explore_vicinity(size_t i) 
    {
        pointer = i;
        vicinity_it = graph[i].begin();
    }
    void go_next() // go to next node in list
    {
        vicinity_it++;
    }
    ADJ* get_ADJ()
    {
        if (vicinity_it == graph[pointer].end()) { // it is important to compare with end of current list
            return nullptr;
        }
        else
            return &(*vicinity_it);
    }
    void next_iter() // set next vicinity but faster
    {
        pointer++;
    }
    list<ADJ>& operator[](size_t i) const // for getting vicinity
    {
        return graph[i];
    }
};

// straigth copy of arrays => unsafe
struct DHeapUnsafe
{
    size_t n;
    size_t d;
    int* name; // names of vertexes in graph
    int* key; // information connected with i-th name
    int* index; // indexes of objects in d-heap, index[name[i]=i
    void create_queue() // O(n)
    {
        for (int i = n - 1; i >= 0; i--) {
            diving(i);
        }
    }
public:
    //key is k_inf if weight is inf and 0 if it is the vertex itself
    DHeapUnsafe(size_t _n, size_t _d, int* _name, int* _key, int* _index) :n(_n), d(_d), name(_name), key(_key), index(_index)
    {
        create_queue();
    }
    // 
    void diving(int i) // in every iteration minchild - O(d), log d n iterations (hight of a heap)
    {
        // it works without swaps because after while we insert key0 and name0
        // we don't need to do such many swaps, only one and many assignments
        int key0 = key[i], name0 = name[i];
        int child = minchild(i);
        while (child != -1 && key0 > key[child]) {
            key[i] = key[child];
            name[i] = name[child];
            // now index[name[i]] == child. We should fix it
            index[name[i]] = i;
            // go to level down
            i = child;
            child = minchild(i);
        }
        // insert vertex to the right place
        key[i] = key0; 
        name[i] = name0;
        index[name[i]] = i;
    }
    // explanation is in diving
    void emersion(int i) // in every iteration parent - 0(1), log d n iterations (hight of a heap)
    {
        int key0 = key[i], name0 = name[i], father = parent(i);
        while (father != -1 && key0 < key[father]) {
            key[i] = key[father];
            name[i] = name[father];
            index[name[i]] = i;
            i = father;
            father = parent(i);
        }
        key[i] = key0;
        name[i] = name0;
        index[name[i]] = i;
    }

    void del_min() // O(d log d n)
    {
        int name0 = name[0], key0 = key[0];
        name[0] = name[n - 1];
        key[0] = key[n - 1];
        name[n - 1] = name0;
        key[n - 1] = key0;
        n--; // memory losing :(
        if (n > 1) {
            diving(0);
        }
        index[name0] = n;
    }
    // returns index of minchild
    int minchild(int i) const // O(d)
    {
        int left_child = leftchild(i);
        if (left_child == -1) {
            return -1; // leaf
        }
        int right_child = rightchild(i);
        int min_child = left_child;
        // if the last element is left_child
        for (int j = left_child + 1; j <= right_child; j++) {
            if (key[j] < key[min_child]) {
                min_child = j;
            }
        }
        return min_child;
    }
    int leftchild(int i) const // O(1)
    {
        int j = i * d + 1;
        if (j >= n) {
            return -1; // leaf
        }
        else {
            return j;
        }
    }
    int rightchild(int i) const // O(1)
    {
        int j1 = i * d + 1;
        int j = (i + 1) * d;
        if (j1 >= n) {
            return -1; // leaf
        }
        return j < (n - 1) ? j : (n - 1); // min does not work
    }
    int parent(int i) const // O(1)
    {
        if (i == 0) {
            return -1; // root
        }
        else {
            return (i - 1) / d;
        }
    }
    bool empty()
    {
        return n == 0;
    }

    void get_min(int& min_name, int& min_key) const
    {
        min_name = name[0];
        min_key = key[0];
    }
};

void LDG_DIJKSTRA_D_HEAP(int* dist, int* up, Graph& graph, size_t n, size_t d, int s) // O((m+n)log d n)
{
    int* name = new int[n]; // names of vertexes in graph
    int* index = new int[n]; // numbers of vertexes in d-heap
    int* key = new int[n]; // weight of vertex in d-heap
    for (int i = 0; i < n; i++)
    {
        up[i] = k_inf;
        dist[i] = k_inf;
        name[i] = i;
        index[i] = i;
        key[i] = k_inf;
    }
    key[s] = 0;
    DHeapUnsafe heap{ n,d,name, key,index };
    while (!heap.empty())
    {
        int min_name, min_key;
        heap.get_min(min_name, min_key);
        heap.del_min();   
        graph.start_explore_vicinity(min_name); // only name! not index
        dist[min_name] = min_key;
        ADJ* p = graph.get_ADJ();
        while (p!=nullptr) {
            int j = p->name; // get real name from graph
            int jq = heap.index[j]; // get local index from heap by the name
            if (key[jq] > dist[min_name] + p->weight) {
                key[jq] = dist[min_name]+p->weight;
                heap.emersion(jq);
                up[j] = min_name;
            }
            graph.go_next();
            p = graph.get_ADJ();
        }
    }
}
void create_full_graph(int n, int q, int r, int** table) {
    mt19937 weight(time(0));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            table[i][j] = weight() % r + q;
        }
    }
    for (int i = 0; i < n; i++) {
        table[i][i] = 0;
    }
}
void create_graph(int n, int m, int q, int r, int** table)
{
    if (m >=(n*(n-1)/2)) {
        create_full_graph(n, q, r, table);
    }
    double condition = m / (double(n) * n);
    condition *= 1000.0; // to know how many pro mile edges take in table
    int c_int = int(condition);
    mt19937 weight(time(0));
    mt19937 edge(time(0));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            table[i][j] = k_inf;
        }
    }
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (edge() % 1000 <= c_int) {
                table[i][j] = weight() % r + q;
            }
        }
    }
    for (int i = 0; i < n; i++) {
        table[i][i] = 0;
    }
}
void get_args(int& n, int& m, int& q, int& r, int& s, int& d)
{
    cout << "Input n m q r s d\n";
    cin >> n >> m >> q >> r >> s >> d;
}
// get n = vertex number, m = edges number, q = min weight, r = max weight
int main(int argc,char* argv[]) 
{
    int n, m, q, r, s, d;
    if (argc < 6) {
        get_args(n, m, q, r, s, d);
    }
    else {
        n = stoi(argv[1]);
        m = stoi(argv[2]);
        q = stoi(argv[3]);
        r = stoi(argv[4]);
        s = stoi(argv[5]);
        d = stoi(argv[6]);
    }
    int** table;
    table = new int* [n];
    for (int i = 0; i < n; i++) {
        table[i] = new int[n];
    }
    int* up = new int[n];
    int* dist = new int[n];
    create_graph(n, m, q, r, table);
    Graph g(table,n);
    auto begin = std::chrono::steady_clock::now();
    LDG_DIJKSTRA_D_HEAP(dist, up, g, n, d, s);
    auto end = std::chrono::steady_clock::now();
    auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
    int j = 0;
    for (int i = 0; i < n; i++) {
        if (up[i] == k_inf) {
            cout << "up[" << i << "] = " << "inf" << " ";
        }
        else {
            cout << "up[" << i << "] = " << up[i] << " ";
        }
        if (dist[i] == k_inf) {
            cout << "dist[" << i << "] = " << "inf";
            if (j == 4) {
                j = 0;
                cout << "\n";
            }
            else {
                j++;
                cout << " ";
            }
        }
        else {
            cout << "dist[" << i << "] = " << dist[i];
            if (j == 4) {
                j = 0;
                cout << "\n";
            }
            else {
                j++;
                cout << " ";
            }
        }
    }
    cout << "\n\nTime: " << elapsed_ms.count() << "\n";
    return 0;// elapsed_ms.count();
}