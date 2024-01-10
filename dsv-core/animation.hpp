#ifndef __ANIMATION_HPP__DSVISUAL
#define __ANIMATION_HPP__DSVISUAL

#include <memory>

#include <Hanim.hpp>
#include <dstruct.hpp>

namespace dsvisual {

struct AOP {
    std::shared_ptr<hanim::HAnimate> anim;
    std::shared_ptr<hanim::HObject> obj;
};

using AOPSeqVec = dstruct::Vector<AOP>;

}


#endif