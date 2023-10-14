#ifndef ELF_SECTION_H
#define ELF_SECTION_H

#include "endian.h"
#include <elf.h>
#include <string>
#include <memory>

namespace elf {
    class ISection {
    public:
        virtual ~ISection() = default;

    public:
        virtual std::string name() = 0;
        virtual void name(std::string_view name) = 0;
        virtual const std::byte *data() = 0;

    public:
        virtual Elf64_Word nameIndex() = 0;
        virtual Elf64_Word type() = 0;
        virtual Elf64_Xword flags() = 0;
        virtual Elf64_Addr address() = 0;
        virtual Elf64_Off offset() = 0;
        virtual Elf64_Xword size() = 0;
        virtual Elf64_Word link() = 0;
        virtual Elf64_Word info() = 0;
        virtual Elf64_Xword addressAlign() = 0;
        virtual Elf64_Xword entrySize() = 0;
    };

    template<typename T, endian::Type Endian>
    class Section : public ISection {
    public:
        Section(const T *section, std::shared_ptr<void> buffer);

    public:
        std::string name() override;
        void name(std::string_view name) override;
        const std::byte *data() override;

    public:
        Elf64_Word nameIndex() override;
        Elf64_Word type() override;
        Elf64_Xword flags() override;
        Elf64_Addr address() override;
        Elf64_Off offset() override;
        Elf64_Xword size() override;
        Elf64_Word link() override;
        Elf64_Word info() override;
        Elf64_Xword addressAlign() override;
        Elf64_Xword entrySize() override;

    private:
        const T *mSection;
        std::string mName;
        std::shared_ptr<void> mBuffer;
    };
}

#endif //ELF_SECTION_H
