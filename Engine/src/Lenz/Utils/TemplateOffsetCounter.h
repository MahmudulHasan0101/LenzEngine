#pragma once

namespace lenz
{
    template<typename T, typename... Ts>
    struct OffsetOf;

    template<typename T, typename First, typename... Rest>
    struct OffsetOf<T, First, Rest...> {
        static constexpr size_t value =
            std::is_same_v<T, First>
            ? 0
            : sizeof(First) + OffsetOf<T, Rest...>::value;
    };

    template<typename T>
    struct OffsetOf<T> {
        static_assert(!sizeof(T), "Type not in layout");
    };

}