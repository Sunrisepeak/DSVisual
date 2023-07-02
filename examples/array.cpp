#include <iostream>
#include <thread>
#include <chrono>

#include <dsvisual.hpp>


int main() {

    dsvisual::Array<int, 10> arr;
    dsvisual::Array<int, 5> arr2;

    while (true) {
        static int data = 0;

        for (int i = 0; i < arr.size(); i++) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            
            data = (data + 1) % 100;

            arr[i] = arr2[i % 5] = data;
        }

    }


    return 0;
}