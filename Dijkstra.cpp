#include <iostream>
#include <random>
#include <ctime>
#include <vector>
#include <list>
#include <string>
using namespace std;

///
///TO DO:
/// graphmaker
/// Debugging

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

class Graph
{
    list<ADJ>* graph;
    size_t _size;
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
                if (weights[i][j] != 0) {
                    tmp.push_back(ADJ{ j,weights[i][j] });
                }
            }
           /* if(!tmp.empty()){
                graph[i] = tmp;
            }*/
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
        if (graph[pointer].empty()) {
            return nullptr;
        }
        if (vicinity_it == graph[0].end()) {
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
        /*name = new int[n];
        key = new int[n];
        index = new int[n];
        for (int i = 0; i < n; i++) {
            key[i] = _key[i];
            index[i] = i;
            name[i] = i;
        }*/
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

void LDG_DIJKSTRA_D_HEAP(int* dist, int* up, Graph& graph, size_t n, size_t d, int s) // O(
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
    dist[s] = key[s] = 0;
    DHeapUnsafe heap{ n,d,name, key,index };
    while (!heap.empty())
    {
        int min_name, min_key;
        heap.get_min(min_name, min_key);
        heap.del_min();
        graph.start_explore_vicinity(min_name); // only name! not index
        ADJ* p = graph.get_ADJ();
        while (p!=nullptr) {
            int j = p->name; // get real name from graph
            int jq = heap.index[j]; // get local index from heap by the name
            int delta = heap.key[jq] - (min_key + p->weight); // old estimate - (distance to min + edge between vertexes)
            if (delta > 0) {
                heap.key[jq] -= delta;
                dist[j] -= delta;
                heap.emersion(jq); // because we reduced the key
                up[j] = min_name;
            }
            graph.go_next();
            p = graph.get_ADJ();
        }
    }
}
void create_graph(int n, int m, int q, int r, int** table)
{
    //    double q = m / (double(n) * n);
    //    q *= 1000.0; // to know how many pro mile edges take in table
    //    int q_int = int(q);
    //    mt19937 weight(time(0));
    //    mt19937 edge(time(0));
    //    for (int i = 0; i < n; i++) {
    //        for (int j = i+1; j < n; j++) { // j = i+1 because table[i][i] = 0 for any i
    //
    //        }
    //    }
    //}
    table[0][0] = 0;
    for (int j = 1; j < n; j++) {
        table[0][j] = 100;
    }
    for (int i = 1; i < n; i++) {
        for (int j = 0; j < n; j++) {
            table[i][j] = 0;
        }
    }
    for (int j = 0; j < n; j++) {
        if (j != 2) {
            table[2][j] = 1;
        }
    }
}
std::string hello_world()
{
    return std::string("Hello, world!\n");
}
// get n = vertex number, m = edges number, q = min weight, r = max weight
int main(int argc,char* argv[]) 
{
    int n, m, q, r;
   /* n = stoi(argv[1]);
    m = stoi(argv[2]);
    q = stoi(argv[3]);
    r = stoi(argv[4]);*/
    n = 5;
    m = 200;
    q = 1;
    r = 120;
    int** table;
    table = new int* [n];
    for (int i = 0; i < n; i++) {
        table[i] = new int[n];
    }
    int* up = new int[n];
    int* dist = new int[n];
    create_graph(n, m, q, r, table);
    Graph g(table,n);
    int d = 3, s = 0;
    LDG_DIJKSTRA_D_HEAP(dist, up, g, n, d, s);
   // std::cout << hello_world();
    for (int i = 0; i < n; i++) {
        cout << "dist[" << i << "] = " << dist[i] << "; ";
    }
    cout << "\n";
    for (int i = 0; i < n; i++) {
        cout << "up[" << i << "] = " << up[i] << "; ";
    }
}