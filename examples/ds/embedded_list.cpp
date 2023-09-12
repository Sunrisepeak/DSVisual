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

        // head-insert
        for (int i = 0; i < 6; i++) {
            auto currNodePtr = new MyNode();
            if (i == 4) {
                midNodePtr = currNodePtr;
            }
            eList.add(headNodePtr, currNodePtr, 100);
        }

        // mid-insert
        for (int i = 0; i < 4; i++) {
            auto currNodePtr = new MyNode();
            eList.add(midNodePtr, currNodePtr, 100);
        }

        // release
        while (!ds::EmbeddedList<MyNode>::empty(headNodePtr)) {
            auto firstNodePtr = MyNode::to_node(headNodePtr->link.next);
            eList.del(headNodePtr, MyNode::to_node(headNodePtr->link.next), 100);
            delete firstNodePtr;
        }
    }

    PlatformManager::waitWindowClosed();

    return 0;
}