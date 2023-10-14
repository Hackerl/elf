#ifndef ELF_HEADER_H
#define ELF_HEADER_H

#include "endian.h"
#include <elf.h>
#include <cstddef>

namespace elf {
    class IHeader {
    public:
        virtual ~IHeader() = default;

    public:
        virtual const unsigned char *ident() = 0;

    public:
        virtual Elf64_Half type() = 0;
        virtual Elf64_Half machine() = 0;
        virtual Elf64_Word version() = 0;
        virtual Elf64_Addr entry() = 0;
        virtual Elf64_Off segmentOffset() = 0;
        virtual Elf64_Off sectionOffset() = 0;
        virtual Elf64_Word flags() = 0;
        virtual Elf64_Half headerSize() = 0;
        virtual Elf64_Half segmentEntrySize() = 0;
        virtual Elf64_Half segmentNum() = 0;
        virtual Elf64_Half sectionEntrySize() = 0;
        virtual Elf64_Half sectionNum() = 0;
        virtual Elf64_Half sectionStrIndex() = 0;
    };

    template<typename T, endian::Type Endian>
    class Header : public IHeader {
    public:
        explicit Header(const T *header);

    public:
        const unsigned char *ident() override;

    public:
        Elf64_Half type() override;
        Elf64_Half machine() override;
        Elf64_Word version() override;
        Elf64_Addr entry() override;
        Elf64_Off segmentOffset() override;
        Elf64_Off sectionOffset() override;
        Elf64_Word flags() override;
        Elf64_Half headerSize() override;
        Elf64_Half segmentEntrySize() override;
        Elf64_Half segmentNum() override;
        Elf64_Half sectionEntrySize() override;
        Elf64_Half sectionNum() override;
        Elf64_Half sectionStrIndex() override;

    private:
        const T *mHeader;
    };
}

#endif //ELF_HEADER_H
