#include <vector>
#include <map>
#include <string>
#include <unordered_map>
#include <iostream>
#include <set>
#include <array>
#include <numeric>
#include <chrono>
#include <unordered_set>
#include <random>

using namespace std;
using std::chrono::duration;
static const long long mapSize= 10000000;
static const long long accSize= 1000000;

void operations(){
    vector<int> vec = {1,2,3,4,5,6,7,8,9};
    map<string,int> m = {{"bart",12345},{"jenne",34929},{"huber",840284}};
    unordered_map<string,int> um{m.begin(),m.end()};
    set<double> douSet{3.14, 2.5};

    for(auto v: vec) cout << v << " ";
    cout << '\n';
    for(auto p: m) cout << p.first << ", " << p.second << " ";
    cout << '\n';
    for(auto p: um) cout << p.first << ", " << p.second << " ";
    cout << '\n';
    vector<int> vec2 = vec;
    cout << vec.size() << endl;
    cout << vec2.size() << endl;
    vector<int> vec3 = move(vec);
    cout << vec.size() << endl;
    cout << vec3.size() << endl;
    vec3.clear();
    cout << vec3.size() << endl;
    cout << douSet.empty() << endl; // 0
    cout << douSet.max_size() << endl;
    
    std::array<int, 10> arr {1,2,3,4,5,6,7,8,9,10};
    for(auto a: arr) cout << a << " ";
    double sum = accumulate(arr.begin(),arr.end(),0);
    cout << sum;
    cout << '\n';
    //for(int i=0;i<vec2.size();++i) cout << vec2[i] == *(vec2 + i) << '\n'; 
    for(int i=0;i<vec2.size();++i) cout << vec2[i]  << ' ';
    cout << '\n';
    //auto start = std::chrono::system_clock::now();
    //for(long long i = 0; i < accSize; ++i){  }
}

struct MyInt{
    MyInt(int v):val(v){}
    bool operator==(const MyInt& other)const{
        return val == other.val;
    }
    int val;
};
struct MyHash{
    std::size_t operator()(MyInt m)const{
        std::hash<int> hashVal;
        return hashVal(m.val);
    }
};

std::ostream& operator << (std::ostream& st, const MyInt& myIn){
    st << myIn.val;
    return st;
}
typedef std::unordered_map<MyInt, int, MyHash> MyIntMap;

void getInfo(const unordered_set<int>& hash){
    cout << "hash.bucket_count(): " << hash.bucket_count() << endl;;
    cout << "hash.load_factor():" << hash.load_factor() << endl;
}
void fillHash(std::unordered_set<int>& h, int n){

  std::random_device seed;
  // default generator
  std::mt19937 engine(seed());
  // get random numbers 0 - 1000
  std::uniform_int_distribution<> uniformDist(0, 1000);

  for ( int i=1; i<= n; ++i){
    h.insert(uniformDist(engine));
  }

}
void maphashex(){
    MyIntMap myMap{{MyInt(-2), -2},{MyInt(-1), -1},{MyInt(0), 0},{MyInt(1), 1}};
    for(auto m: myMap) std::cout << "{" << m.first << "," << m.second << "} ";
    std::cout << myMap[MyInt(-2)] << std::endl;
    unordered_set<int> hash;
    cout << "hash.max_load_factor " << hash.max_load_factor() << endl;
    getInfo(hash);
    hash.insert(500);
    cout << "hash.bucket(500) " << hash.bucket(500) << endl;
    fillHash(hash, 100);
    getInfo(hash);
    hash.rehash(500);
    getInfo(hash);
    cout << "hash.bucket(500) " << hash.bucket(500) << endl;
}

int main(){
    operations();
    maphashex();
    return 0;
}