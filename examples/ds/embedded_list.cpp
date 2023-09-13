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

        assert(headNodePtr->link.next = headNodePtr->link.prev); // empty test

        // tail-insert
        auto insertPtr = headNodePtr;
        for (int i = 0; i < 2; i++) {
            auto currNodePtr = new MyNode();
            eList.add(insertPtr, currNodePtr, 200);
            insertPtr = currNodePtr;
        }

        // mid-insert
        for (int i = 0; i < 3; i++) {
            auto currNodePtr = new MyNode();
            eList.add(insertPtr, currNodePtr, 200);
        }

        // release
        while (!ds::EmbeddedList<MyNode>::empty(headNodePtr)) {
            auto firstNodePtr = MyNode::to_node(headNodePtr->link.next);
            eList.del(headNodePtr, MyNode::to_node(headNodePtr->link.next), 200);
            delete firstNodePtr;
        }
    }

    PlatformManager::waitWindowClosed();

    return 0;
}