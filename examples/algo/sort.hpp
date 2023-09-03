#ifndef __SORT_HPP__DSVISUAL
#define __SORT_HPP__DSVISUAL

#include <dsvisual.hpp>

template <unsigned int ARR_SIZE = 100>
static void bubbleSort(dsvisual::ds::Array<int, ARR_SIZE> &arr, bool enableFrameSync = false) {
    bool exchanged = true;
    for (int i = 0; exchanged && i < arr.size() - 1; i++) {
        exchanged = false;
        for (int j = 0; j < arr.size() - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                std::swap(arr[j], arr[j + 1]);
                exchanged = true;
                if (enableFrameSync)
                    dsvisual::PlatformManager::frameSync();
            }
        }
    }
}

template <unsigned int ARR_SIZE = 100>
void selectionSort(dsvisual::ds::Array<int, ARR_SIZE> &arr, bool enableFrameSync = false) {
    int n = arr.size();
    for (int i = 0; i < n - 1; ++i) {
        int minIndex = i;
        for (int j = i + 1; j < n; ++j) {
            if (arr[j] < arr[minIndex]) {
                minIndex = j;
            }
        }
        std::swap(arr[i], arr[minIndex]);
        if (enableFrameSync)
            dsvisual::PlatformManager::frameSync();
    }
}


#endif