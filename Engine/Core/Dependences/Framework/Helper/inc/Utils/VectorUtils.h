//
// Created by Monika on 01.09.2021.
//

#ifndef GAMEENGINE_VECTORUTILS_H
#define GAMEENGINE_VECTORUTILS_H

#include <vector>
#include <cctype>
#include <functional>

namespace Framework::Helper {
    class VectorUtils {
    public:
        VectorUtils() = delete;
        VectorUtils(VectorUtils &) = delete;
        ~VectorUtils() = delete;
        VectorUtils operator=(VectorUtils&) = delete;
    public:
        template<typename T> static std::vector<T> Combine(const std::vector<T>& A, const std::vector<T>& B) {
            std::vector<T> AB(A.size() + B.size()); // preallocate memory
            AB.insert(AB.end(), A.begin(), A.end());
            AB.insert(AB.end(), B.begin(), B.end());
            return AB;
        }
    };

    namespace Utils {
        template<typename T, typename U>
        bool ForEach(const std::function<bool(T& type, const uint32_t& index)> &fn, std::vector<U>& vector) {
            uint32_t index = 0;
            for (T& folder : vector) {
                if (!fn(folder, index))
                    return false;
                ++index;
            }
            return true;
        }

        template<typename T, typename U>
        bool ForEach(const std::function<bool(const T& type, const uint32_t& index)> &fn, const std::vector<U>& vector) {
            uint32_t index = 0;
            for (const T& folder : vector) {
                if (!fn(folder, index))
                    return false;
                ++index;
            }
            return true;
        }
    }
}

#endif //GAMEENGINE_VECTORUTILS_H