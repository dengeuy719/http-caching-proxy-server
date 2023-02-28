#include <map>
#include <list>
#include <iostream>

int main() {
    std::map<int, typename std::list<std::pair<int, int>>::iterator> map;
    std::list<std::pair<int, int>> list;
    list.push_front(std::make_pair(1, 1));
    auto it = list.begin();
    map.insert(std::make_pair(1, it));
    list.push_front(std::make_pair(2,2));
    std::cout << it->second << std::endl;
    it->second = 5;
    std::cout << list.begin()->second << std::endl;
    list.push_front(std::make_pair(3,3));
    std::cout << it->second << std::endl;
    list.splice(list.begin(), list, it);
    std::cout << map.at(1)->second;
    return 0;
}