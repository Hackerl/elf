#include <elf/segment.h>

template<typename T, elf::endian::Type Endian>
elf::Segment<T, Endian>::Segment(const T *segment, std::shared_ptr<void> buffer)
        : mSegment(segment), mBuffer(std::move(buffer)) {

}

template<typename T, elf::endian::Type Endian>
const std::byte *elf::Segment<T, Endian>::data() {
    return (const std::byte *) mBuffer.get() + offset();
}

template<typename T, elf::endian::Type Endian>
Elf64_Word elf::Segment<T, Endian>::type() {
    return endian::convert<Endian>(mSegment->p_type);
}

template<typename T, elf::endian::Type Endian>
Elf64_Word elf::Segment<T, Endian>::flags() {
    return endian::convert<Endian>(mSegment->p_flags);
}

template<typename T, elf::endian::Type Endian>
Elf64_Off elf::Segment<T, Endian>::offset() {
    return endian::convert<Endian>(mSegment->p_offset);
}

template<typename T, elf::endian::Type Endian>
Elf64_Addr elf::Segment<T, Endian>::virtualAddress() {
    return endian::convert<Endian>(mSegment->p_vaddr);
}

template<typename T, elf::endian::Type Endian>
Elf64_Addr elf::Segment<T, Endian>::physicalAddress() {
    return endian::convert<Endian>(mSegment->p_paddr);
}

template<typename T, elf::endian::Type Endian>
Elf64_Xword elf::Segment<T, Endian>::fileSize() {
    return endian::convert<Endian>(mSegment->p_filesz);
}

template<typename T, elf::endian::Type Endian>
Elf64_Xword elf::Segment<T, Endian>::memorySize() {
    return endian::convert<Endian>(mSegment->p_memsz);
}

template<typename T, elf::endian::Type Endian>
Elf64_Xword elf::Segment<T, Endian>::align() {
    return endian::convert<Endian>(mSegment->p_align);
}

template
class elf::Segment<Elf32_Phdr, elf::endian::Little>;

template
class elf::Segment<Elf32_Phdr, elf::endian::Big>;

template
class elf::Segment<Elf64_Phdr, elf::endian::Little>;

template
class elf::Segment<Elf64_Phdr, elf::endian::Big>;