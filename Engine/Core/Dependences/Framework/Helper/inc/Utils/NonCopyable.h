//
// Created by Monika on 16.01.2022.
//

#ifndef SRENGINE_NONCOPYABLE_H
#define SRENGINE_NONCOPYABLE_H

#include <macros.h>

namespace SR_UTILS_NS {
    class NonCopyable {
    protected:
        constexpr NonCopyable() = default;
        virtual ~NonCopyable() = default;

        NonCopyable(const NonCopyable&) = delete;
        NonCopyable& operator=(const NonCopyable&) = delete;
    };
}

#endif //SRENGINE_NONCOPYABLE_H
