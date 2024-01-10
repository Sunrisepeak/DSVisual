#include <iostream>
#include <thread>
#include <chrono>
#include <memory>

#include <dsvisual.hpp>


int main() {


    hanim::object::imgui::Rectangle rec;
    auto anim = hanim::move(100, 300, 500, 300);
    dsvisual::AOPSeqVec aopSeqVec;
    aopSeqVec.push_back({
        std::make_shared<decltype(anim)>(anim),
        std::make_shared<decltype(rec)>(rec)
    });

    aopSeqVec.push_back({
        std::make_shared<decltype(anim)>(hanim::move(500, 300, 500, 500, 120)),
        std::make_shared<decltype(rec)>(rec)
    });

    dsvisual::WindowManager::render(aopSeqVec);

    std::this_thread::sleep_for(std::chrono::seconds(10));
    return 0;
}