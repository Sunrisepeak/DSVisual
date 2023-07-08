#include <dsvisual.hpp>


int main() {
    dsvisual::input::KeyRecorder kr;
    while (!dsvisual::PlatformManager::windowClosed()) {
        std::this_thread::sleep_for(std::chrono::microseconds(1));
    }
    return 0;
}