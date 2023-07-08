#include <iostream>
#include <thread>
#include <chrono>

#include <dsvisual.hpp>


int main() {


{// test: auto create/destory
    dsvisual::ds::Array<int, 10> arr;
    std::this_thread::sleep_for(std::chrono::seconds(2));
}

{// test: dynamic create/destory
    dsvisual::ds::Array<int, 10> arr;
    dstruct::Vector<dsvisual::ds::Array<int, 10> *> arrVec;

    for (int i = 0; i < 3; i++) {
        arrVec.push_back(new decltype(arr)());
    }

    for (auto arrPtr : arrVec) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        delete arrPtr;
    }
}

{// test: track modified element
    dsvisual::ds::Array<int, 10> arr;
    for (int i = 0; i < arr.size(); i++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        arr[i] = i;
    }
}

{// test: track iterator

    dsvisual::ds::Array<int, 10> arr;
    for (auto it = arr.begin(); it != arr.end(); it++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

{// test: track iterator and modify

    dsvisual::ds::Array<int, 10> arr;
    for (auto it = arr.begin(); it != arr.end(); it++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        *it = 0;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

{// test: control index
    dsvisual::ds::Array<int, 10> arr;
    std::this_thread::sleep_for(std::chrono::seconds(5));
}

    std::this_thread::sleep_for(std::chrono::seconds(10));
    return 0;
}