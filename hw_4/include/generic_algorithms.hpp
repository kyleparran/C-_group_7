#pragma once

template<typename InputIt, typename Predicate>
InputIt find_if(InputIt first, InputIt last, Predicate pred) {
    for(; first != last; ++first) {
        if(pred(*first)) {
            return first;
        }
    }
    return last;
}
