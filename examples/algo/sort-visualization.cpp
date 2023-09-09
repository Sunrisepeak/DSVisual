#include <thread>

#include <dsvisual.hpp>

#include <examples/demo-helper.hpp>
#include <examples/algo/sort.hpp>

constexpr int ARR_SIZE = 500;

int main() {

    // framework config
    dsvisual::PlatformManager::setWindowFPS(120);
    dsvisual::PlatformManager::setRootWindowSize(1920, 1080);
    dsvisual::PlatformManager::setRecorder(true); // save to video

    dsvisual::ds::Array<int, ARR_SIZE> arr1, arr2;

    // arr visual-config
    arr1.setName("Selection sort"); arr2.setName("Insertion sort");
    arr1.setPos(100, 100); arr2.setPos(1010, 100);
    arr1.setSize(810, 880); arr2.setSize(810, 880);
    arr1.setVisible(true); arr2.setVisible(true);
    arr1.setDataVisualExtend(true); arr2.setDataVisualExtend(true);

    // arr control/operate/use
    randomDataGenerator(arr1, 1, ARR_SIZE);
    for (int i = 0; i < arr2.size(); i++) arr2[i] = arr1[i];

    //bubbleSort<ARR_SIZE>(arr);
    auto t1 = std::thread([&]{ selectionSort<ARR_SIZE>(arr1); });
    auto t2 = std::thread([&]{ insertionSort<ARR_SIZE>(arr2); });

    t1.join();
    t2.join();

    dsvisual::PlatformManager::waitWindowClosed();

    return 0;
}