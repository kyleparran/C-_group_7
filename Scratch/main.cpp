#include <iostream>
using namespace std;

template <class T>
class MyVector{
public:   
    T container[20];
    int index;
    MyVector(){
    index=0;
    }
    void push_back(T val){
        container[index] = val;
        index++;
    }
}
// CRTP curiousity template recurring pattern
int main(){
    MyVector<int> integers;
    MyVector<string> strings;
}