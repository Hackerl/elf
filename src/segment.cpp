#include <elf/segment.h>

template<typename T, elf::endian::Type endian>
elf::Segment<T, endian>::Segment(T *segment, std::shared_ptr<void> buffer) : mSegment(segment), mBuffer(std::move(buffer)) {

}

template<typename T, elf::endian::Type endian>
const std::byte *elf::Segment<T, endian>::data() {
    return (const std::byte *) mBuffer.get() + offset();
}

template<typename T, elf::endian::Type endian>
Elf64_Word elf::Segment<T, endian>::type() {
    return endian::convert<endian>(mSegment->p_type);
}

template<typename T, elf::endian::Type endian>
Elf64_Word elf::Segment<T, endian>::flags() {
    return endian::convert<endian>(mSegment->p_flags);
}

template<typename T, elf::endian::Type endian>
Elf64_Off elf::Segment<T, endian>::offset() {
    return endian::convert<endian>(mSegment->p_offset);
}

template<typename T, elf::endian::Type endian>
Elf64_Addr elf::Segment<T, endian>::virtualAddress() {
    return endian::convert<endian>(mSegment->p_vaddr);
}

template<typename T, elf::endian::Type endian>
Elf64_Addr elf::Segment<T, endian>::physicalAddress() {
    return endian::convert<endian>(mSegment->p_paddr);
}

template<typename T, elf::endian::Type endian>
Elf64_Xword elf::Segment<T, endian>::fileSize() {
    return endian::convert<endian>(mSegment->p_filesz);
}

template<typename T, elf::endian::Type endian>
Elf64_Xword elf::Segment<T, endian>::memorySize() {
    return endian::convert<endian>(mSegment->p_memsz);
}

template<typename T, elf::endian::Type endian>
Elf64_Xword elf::Segment<T, endian>::align() {
    return endian::convert<endian>(mSegment->p_align);
}

template class elf::Segment<Elf32_Phdr, elf::endian::Little>;
template class elf::Segment<Elf32_Phdr, elf::endian::Big>;
template class elf::Segment<Elf64_Phdr, elf::endian::Little>;
template class elf::Segment<Elf64_Phdr, elf::endian::Big>;