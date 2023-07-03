#include <iostream>
#include <thread>
#include <chrono>

#include <dsvisual.hpp>


int main() {

    dsvisual::Array<int, 10> arr1;
    dsvisual::Array<int, 5> arr2;
    dsvisual::Array<int, 20> arr3;

    static int data = 0;

    while (!dsvisual::PlatformManager::windowClosed()) {
        for (int i = 0; i < arr1.size(); i++) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));

            data = (data + 1) % 100;

            arr1[i] = data;
            arr2[i % 5] = data * 2;
        }
    }

    return 0;
}