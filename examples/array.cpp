#include <dsvisual.hpp>



int main() {

    dsvisual::Window w;

    dsvisual::Array<int, 4> arr;


    while (!w.closed()) {
        dsvisual::Array<int, 5> arr2;
        w.addWidget(&arr);
        w.addWidget(&arr2);
        w.render();
    }

    return 0;
}