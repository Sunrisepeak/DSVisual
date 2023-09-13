#ifndef __HANIM_ANIMATE_DSVISUAL_HPP__HANIM
#define __HANIM_ANIMATE_DSVISUAL_HPP__HANIM

#include <Hanim.hpp>

namespace hanim {
namespace animate {
namespace dsvisual {

struct InsertAnim : public hanim::ComposeAnim {
    InsertAnim(float y, float offset, int frameNumbers = 60) {
        // 1 : 1 : 1 : 1
        int subF = frameNumbers / 4;
        hanim::ComposeAnim::move(-1, 0, -1, y / 2) // move Y 1
            .setFrameNums(subF);
        setFrameTrackIndex(0);

        hanim::ComposeAnim::move(0, -1, offset, -1) // move X
            .setFrameNums(subF);
        setFrameTrackIndex(subF);

        addAnim( // link anim
            hanim::InterpolationAnim(
                hanim::InterpolationAnim::IAType::CUSTOM,
                { 0 }, { 1 },
                subF
            ),
            2 * subF
        );

        hanim::ComposeAnim::move(-1, y / 2 + 1, -1, y) // move Y 2
            .setFrameNums(subF);
        setFrameTrackIndex(3 * subF);

        hanim::ComposeAnim::alpha(0, 150)
            .setFrameNums(frameNumbers);
        setFrameTrackIndex(0);

        setFrameNums(frameNumbers);
    }
};

struct DeleteAnim : public hanim::ComposeAnim {
    DeleteAnim(float deltaY, int frameNumbers = 60) {
        int subF = frameNumbers / 5;
        // step1: move down
        hanim::ComposeAnim::move(-1, 0, -1, deltaY)
            .setFrameNums(subF);
        setFrameTrackIndex(0);

        auto customAnimTemplate = hanim::InterpolationAnim(
            hanim::InterpolationAnim::IAType::CUSTOM,
            { 0 }, { 1 },
            subF
        );

        // step2: prev -> next
        customAnimTemplate.setStartFrame({0});
        customAnimTemplate.setEndFrame({1});
        addAnim(customAnimTemplate, subF);

        // step3: prev <- next
        customAnimTemplate.setStartFrame({1});
        customAnimTemplate.setEndFrame({2});
        addAnim(customAnimTemplate, subF * 2);

        // step4: prev <-x- curr -x-> next
        customAnimTemplate.setStartFrame({2});
        customAnimTemplate.setEndFrame({3});
        addAnim(customAnimTemplate, subF * 3);

        // step5: fade-out and move-L
        customAnimTemplate.setStartFrame({3});
        customAnimTemplate.setEndFrame({4});
        addAnim(customAnimTemplate, subF * 4);

        setFrameNums(frameNumbers);
    }
};

} // dsvisual
} // animate
} // hanim

#endif