#ifndef ELF_CPP_ERROR_H
#define ELF_CPP_ERROR_H

#include <system_error>

namespace elf {
    enum Error {
        INVALID_ELF_HEADER = 1,
        INVALID_ELF_MAGIC,
        INVALID_ELF_CLASS,
        INVALID_ELF_ENDIAN
    };

    class Category : public std::error_category {
    public:
        [[nodiscard]] const char *name() const noexcept override;
        [[nodiscard]] std::string message(int value) const override;
    };

    const std::error_category &category();
    std::error_code make_error_code(Error e);
}

namespace std {
    template<>
    struct is_error_code_enum<elf::Error> : public true_type {

    };
}

#endif //ELF_CPP_ERROR_H
