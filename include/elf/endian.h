#ifndef ELF_ENDIAN_H
#define ELF_ENDIAN_H

#include <endian.h>
#include <cstdint>
#include <cstddef>
#include <type_traits>

namespace elf::endian {
    enum Type {
        Little,
        Big
    };

    template<Type endian, typename T>
    T convert(T bits) {
        T r = 0;

        if constexpr (endian == Little) {
            for (size_t i = 0; i < sizeof(T); i++) {
                r |= ((T) *(((std::byte *) &bits) + i)) << (i * 8);
            }
        } else {
            for (size_t i = 0; i < sizeof(T); i++) {
                r |= ((T) *(((std::byte *) &bits) + i)) << ((sizeof(T) - i - 1) * 8);
            }
        }

        return r;
    }
}

#endif //ELF_ENDIAN_H
