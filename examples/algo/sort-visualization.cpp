#include <dsvisual.hpp>

#include <examples/demo-helper.hpp>
#include <examples/algo/sort.hpp>

constexpr int ARR_SIZE = 1000;

int main() {

    // framework config
    dsvisual::PlatformManager::setWindowFPS(120);
    //dsvisual::PlatformManager::setRecorder(true); // save to video
    dsvisual::PlatformManager::setRootWindowSize(1920, 1080);

    dsvisual::ds::Array<int, ARR_SIZE> arr;

    // arr visual-config
    arr.setPos(0, 0);
    arr.setSize(1920, 1080);
    arr.setVisible(true);
    arr.setDataVisualExtend(true);

    // arr control/operate/use
    randomDataGenerator(arr, 1, ARR_SIZE);
    //bubbleSort<ARR_SIZE>(arr);
    selectionSort<ARR_SIZE>(arr, true);

    dsvisual::PlatformManager::waitWindowClosed();

    return 0;
}