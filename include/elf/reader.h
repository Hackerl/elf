#ifndef ELF_READER_H
#define ELF_READER_H

#include "header.h"
#include "segment.h"
#include "section.h"
#include <string>
#include <memory>
#include <vector>
#include <optional>

namespace elf {
    class Reader {
    public:
        bool load(const std::string &path);

    public:
        [[nodiscard]] std::unique_ptr<IHeader> header() const;
        [[nodiscard]] std::vector<std::shared_ptr<ISegment>> segments() const;
        [[nodiscard]] std::vector<std::shared_ptr<ISection>> sections() const;

    public:
        [[nodiscard]] const std::byte *virtualMemory(Elf64_Addr address) const;
        [[nodiscard]] std::optional<std::vector<std::byte>> readVirtualMemory(Elf64_Addr address, Elf64_Xword length) const;

    private:
        std::shared_ptr<void> mBuffer;
    };
}

#endif //ELF_READER_H
