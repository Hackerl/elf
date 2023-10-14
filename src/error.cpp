#include <elf/error.h>

const char *elf::Category::name() const noexcept {
    return "elf";
}

std::string elf::Category::message(int value) const {
    std::string msg;

    switch (value) {
        case INVALID_ELF_HEADER:
            msg = "invalid elf header";
            break;

        case INVALID_ELF_MAGIC:
            msg = "invalid elf magic";
            break;

        case INVALID_ELF_CLASS:
            msg = "invalid elf class";
            break;

        case INVALID_ELF_ENDIAN:
            msg = "invalid elf endian";
            break;

        default:
            msg = "unknown";
            break;
    }

    return msg;
}

const std::error_category &elf::category() {
    static Category instance;
    return instance;
}

std::error_code elf::make_error_code(Error e) {
    return {static_cast<int>(e), category()};
}