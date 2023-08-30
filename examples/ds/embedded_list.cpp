#include <iostream>
#include <thread>
#include <chrono>

#include <dsvisual.hpp>

/*
https://cs.android.com/android/kernel/superproject/+/common-android-mainline:common/include/linux/types.h
https://cs.android.com/android/kernel/superproject/+/common-android-mainline:common/include/linux/list.h

// Note: compiler buildin __builtin_offsetof
#define offsetof(type, member) \
    (int((type*)0)->m)

struct List {
    List *next;
};

struct List {
    List *prev, *next;
};

struct ListNode {
    List *next;
    Type data;
}

*/
using namespace dsvisual;

using MyNode = dstruct::EListNode<ds::Array<int, 4>>;

int main() {

    ds::EmbeddedList<MyNode> eList;
    //eList.setPos(100, 100);
    //eList.setSize(600, 600);
    eList.setVisible(true);

    ds::EmbeddedList<MyNode>::init(eList.headNodePtr());


    for (int i = 0; i < 10; i++) {
        auto nodePtr = new MyNode();
        ds::EmbeddedList<MyNode>::add(eList.headNodePtr(), nodePtr, &eList);
    }

    PlatformManager::waitWindowClosed();

    return 0;
}