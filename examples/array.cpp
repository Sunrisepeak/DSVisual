#include <iostream>
#include <thread>
#include <chrono>

#include <dsvisual.hpp>


int main() {

    dsvisual::Array<int, 10> arr1;
    dsvisual::Array<int, 5> arr2;

    dstruct::Vector<dsvisual::Array<int, 5> *> arrVec;

    for (int i = 0; i < 3; i++) {
        arrVec.push_back(new decltype(arr2)());
    }

    while (!dsvisual::PlatformManager::windowClosed()) {
        // modify arr1 arr2
        for (int i = 0; i < 100; i++) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));

            arr1[i % arr1.size()] = i;
            arr2[i % arr2.size()] = i;

            for (auto arrPtr : arrVec) {
                (*arrPtr)[i % arrPtr->size()] = i;
            }
        }
    }

    for (auto arrPtr : arrVec) {
        delete arrPtr;
    }

    return 0;
}