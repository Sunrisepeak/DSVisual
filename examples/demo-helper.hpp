#ifndef __DEMO_HELPER_HPP__DSVISUAL
#define __DEMO_HELPER_HPP__DSVISUAL

#include <random>

template <typename DSVType>
static void randomDataGenerator(DSVType &dsv, int rangeL, int rangeR) {
    // test: random data
    // Choose a random mean between 1 and 2 * ARR_SIZE
    // https://en.cppreference.com/w/cpp/numeric/random
    std::random_device r;
    std::default_random_engine e1(r());
    std::uniform_int_distribution<int> uniform_dist(rangeL, rangeR);

    for (auto it = dsv.begin(); it != dsv.end(); it++) {
        *it = uniform_dist(e1);
    }
}

#endif