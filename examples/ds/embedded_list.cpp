#include <iostream>
#include <thread>
#include <chrono>

#include <dsvisual.hpp>

using namespace dsvisual;

using MyNode = dstruct::EListNode<ds::Array<int, 4>>;

int main() {
    //PlatformManager::setWindowFPS(60);
    PlatformManager::setRecorder();
    ds::EmbeddedList<MyNode> eList;
    eList.setVisible(true);
    { // data struct visualization
        ds::EmbeddedList<MyNode>::init(eList.headNodePtr());
        MyNode *headNodePtr = eList.headNodePtr();
        MyNode *midNodePtr = nullptr;
        // head-insert
        for (int i = 0; i < 6; i++) {
            auto currNodePtr = new MyNode();
            if (i == 4) {
                midNodePtr = currNodePtr;
            }
            ds::EmbeddedList<MyNode>::add(headNodePtr, currNodePtr, &eList, 100);
        }

        // mid-insert
        for (int i = 0; i < 4; i++) {
            auto currNodePtr = new MyNode();
            ds::EmbeddedList<MyNode>::add(midNodePtr, currNodePtr, &eList, 100);
        }

        // release
        while (!ds::EmbeddedList<MyNode>::empty(headNodePtr)) {
            auto firstNodePtr = MyNode::to_node(headNodePtr->link.next);
            ds::EmbeddedList<MyNode>::del(headNodePtr, MyNode::to_node(headNodePtr->link.next), 1000);
            delete firstNodePtr;
        }
    }

    PlatformManager::waitWindowClosed();

    return 0;
}