#include <iostream>
#include <thread>
#include <chrono>
#include <random>

#include <dsvisual.hpp>

constexpr int ARR_SIZE = 100;

void bubble_sort(dsvisual::ds::Array<int, ARR_SIZE> &arr) {
    bool exchanged = true;
    for (int i = 0; exchanged && i < arr.size() - 1; i++) {
        exchanged = false;
        for (int j = 0; j < arr.size() - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                std::swap(arr[j], arr[j + 1]);
                exchanged = true;
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
            }
        }
    }
}

int main() {

    dsvisual::ds::Array<int, ARR_SIZE> arr;

    arr.setDataVisualExtend(true);

{// test: bad data
    for (int i = 0; i < ARR_SIZE; i++) { arr[i] = ARR_SIZE - i; }
    bubble_sort(arr);
}

    std::this_thread::sleep_for(std::chrono::seconds(2));

{// test: random data
    // Choose a random mean between 1 and 2 * ARR_SIZE
    // https://en.cppreference.com/w/cpp/numeric/random
    std::random_device r;
    std::default_random_engine e1(r());
    std::uniform_int_distribution<int> uniform_dist(1, 2 * ARR_SIZE);

    for (int i = 0; i < ARR_SIZE; i++) { arr[i] = uniform_dist(e1); }
    bubble_sort(arr);
}

    //std::this_thread::sleep_for(std::chrono::seconds(1));

    return 0;
}