#ifndef __SORT_HPP__DSVISUAL
#define __SORT_HPP__DSVISUAL

#include <thread>
#include <chrono>

#include <dsvisual.hpp>

// config delay
#define CMP_SLEEP_MS 100
#define WRITE_SLEEP_MS 150
#define SWAP_SLEEP_MS 300

static void operateDelay(int microSec) {
#ifdef ENABLE_SORT_DELAY
    std::this_thread::sleep_for(std::chrono::microseconds(microSec));
#endif
}

template <unsigned int ARR_SIZE>
static void bubbleSort(dsvisual::ds::Array<int, ARR_SIZE> &arr) {
    bool exchanged = true;
    for (int i = 0; exchanged && i < arr.size() - 1; i++) {
        exchanged = false;
        for (int j = 0; j < arr.size() - i - 1; j++) {
            operateDelay(CMP_SLEEP_MS);
            if (arr[j] > arr[j + 1]) {
                operateDelay(SWAP_SLEEP_MS);
                std::swap(arr[j], arr[j + 1]);
                exchanged = true;
            }
        }
    }
}

template <unsigned int ARR_SIZE>
void selectionSort(dsvisual::ds::Array<int, ARR_SIZE> &arr) {
    int n = arr.size();
    for (int i = 0; i < n - 1; ++i) {
        int minIndex = i;
        for (int j = i + 1; j < n; ++j) {
            operateDelay(CMP_SLEEP_MS);
            if (arr[j] < arr[minIndex]) {
                operateDelay(WRITE_SLEEP_MS);
                minIndex = j;
            }
        }
        operateDelay(SWAP_SLEEP_MS);
        std::swap(arr[i], arr[minIndex]);
    }
}

template <unsigned int ARR_SIZE>
void insertionSort(dsvisual::ds::Array<int, ARR_SIZE> &arr) {
    for (int i = 1; i < arr.size(); ++i) {
        int key = arr[i];
        int j = i - 1;

        // insert-op
        while (j >= 0 && arr[j] > key) {
            operateDelay(CMP_SLEEP_MS); // arr[j] > key
            operateDelay(WRITE_SLEEP_MS);
            arr[j + 1] = arr[j];
            --j;
        }

        operateDelay(WRITE_SLEEP_MS);
        arr[j + 1] = key;
    }
}


#endif