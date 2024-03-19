//
// Created by Yancey on 2023/12/18.
//

#pragma once

#ifndef CHELPER_VECTORVIEW_H
#define CHELPER_VECTORVIEW_H

#include "Exception.h"

namespace CHelper {

    template<class T>
    class VectorView {
    public:
        std::shared_ptr<std::vector<T>> vector = nullptr;
        size_t start, end;

        VectorView(const std::shared_ptr<std::vector<T>> vector, size_t start, size_t end)
                : vector(vector),
                  start(start),
                  end(end) {
#if CHelperDebug == true
            if (HEDLEY_UNLIKELY(start > end)) {
                throw Exception::WrongRange(start, end);
            }
#endif
        }

        [[nodiscard]] inline bool isEmpty() const {
            return start >= end;
        };

        [[nodiscard]] inline bool hasValue() const {
            return start < end;
        };

        inline const T &operator[](size_t which) const {
            return vector->at(start + which);
        }

        [[nodiscard]] inline size_t size() const {
            return end - start;
        }

        [[nodiscard]] inline auto beginIterator() const {
            return vector->begin() + start;
        }

        [[nodiscard]] inline auto endIterator() const {
            return vector->begin() + end;
        }

        template<typename Function>
        inline Function forEach(Function function) const {
            return std::for_each(vector->begin() + start, vector->begin() + end, function);
        }
    };

} // CHelper

#endif //CHELPER_VECTORVIEW_H
