#ifndef ELF_SEGMENT_H
#define ELF_SEGMENT_H

#include "endian.h"
#include <elf.h>
#include <string>
#include <memory>

namespace elf {
    class ISegment {
    public:
        virtual ~ISegment() = default;

    public:
        virtual const std::byte *data() = 0;

    public:
        virtual Elf64_Word type() = 0;
        virtual Elf64_Word flags() = 0;
        virtual Elf64_Off offset() = 0;
        virtual Elf64_Addr virtualAddress() = 0;
        virtual Elf64_Addr physicalAddress() = 0;
        virtual Elf64_Xword fileSize() = 0;
        virtual Elf64_Xword memorySize() = 0;
        virtual Elf64_Xword align() = 0;
    };

    template<typename T, endian::Type Endian>
    class Segment : public ISegment {
    public:
        Segment(const T *segment, std::shared_ptr<void> buffer);

    public:
        const std::byte *data() override;

    public:
        Elf64_Word type() override;
        Elf64_Word flags() override;
        Elf64_Off offset() override;
        Elf64_Addr virtualAddress() override;
        Elf64_Addr physicalAddress() override;
        Elf64_Xword fileSize() override;
        Elf64_Xword memorySize() override;
        Elf64_Xword align() override;

    private:
        const T *mSegment;
        std::shared_ptr<void> mBuffer;
    };
}

#endif //ELF_SEGMENT_H
