#ifndef ELF_ENDIAN_H
#define ELF_ENDIAN_H

#include <endian.h>
#include <cstdint>
#include <type_traits>

namespace elf::endian {
    enum Type {
        Little,
        Big
    };

    template<typename T>
    T little(T bits) {
        if constexpr (std::is_same_v<T, uint16_t>) {
            return le16toh(bits);
        } else if constexpr (std::is_same_v<T, uint32_t>) {
            return le32toh(bits);
        } else if constexpr (std::is_same_v<T, uint64_t>) {
            return le64toh(bits);
        } else if constexpr (std::is_same_v<T, int16_t>) {
            return (int16_t) little((uint16_t) bits);
        } else if constexpr (std::is_same_v<T, int32_t>) {
            return (int32_t) little((uint32_t) bits);
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return (int64_t) little((uint64_t) bits);
        } else {
            static_assert(!sizeof(T *), "type not supported");
        }
    }

    template<typename T>
    T big(T bits) {
        if constexpr (std::is_same_v<T, uint16_t>) {
            return be16toh(bits);
        } else if constexpr (std::is_same_v<T, uint32_t>) {
            return be32toh(bits);
        } else if constexpr (std::is_same_v<T, uint64_t>) {
            return be64toh(bits);
        } else if constexpr (std::is_same_v<T, int16_t>) {
            return (int16_t) big((uint16_t) bits);
        } else if constexpr (std::is_same_v<T, int32_t>) {
            return (int32_t) big((uint32_t) bits);
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return (int64_t) big((uint64_t) bits);
        } else {
            static_assert(!sizeof(T *), "type not supported");
        }
    }

    template<Type endian, typename T>
    T convert(T bits) {
        if constexpr (endian == Little) {
            return little(bits);
        } else {
            return big(bits);
        }
    }
}

#endif //ELF_ENDIAN_H
