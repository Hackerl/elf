#include <elf/header.h>

template<typename T, elf::endian::Type endian>
elf::Header<T, endian>::Header(T *header) : mHeader(header) {

}

template<typename T, elf::endian::Type endian>
const unsigned char *elf::Header<T, endian>::ident() {
    return mHeader->e_ident;
}

template<typename T, elf::endian::Type endian>
Elf64_Half elf::Header<T, endian>::type() {
    return endian::convert<endian>(mHeader->e_type);
}

template<typename T, elf::endian::Type endian>
Elf64_Half elf::Header<T, endian>::machine() {
    return endian::convert<endian>(mHeader->e_machine);
}

template<typename T, elf::endian::Type endian>
Elf64_Word elf::Header<T, endian>::version() {
    return endian::convert<endian>(mHeader->e_version);
}

template<typename T, elf::endian::Type endian>
Elf64_Addr elf::Header<T, endian>::entry() {
    return endian::convert<endian>(mHeader->e_entry);
}

template<typename T, elf::endian::Type endian>
Elf64_Off elf::Header<T, endian>::segmentOffset() {
    return endian::convert<endian>(mHeader->e_phoff);
}

template<typename T, elf::endian::Type endian>
Elf64_Off elf::Header<T, endian>::sectionOffset() {
    return endian::convert<endian>(mHeader->e_shoff);
}

template<typename T, elf::endian::Type endian>
Elf64_Word elf::Header<T, endian>::flags() {
    return endian::convert<endian>(mHeader->e_flags);
}

template<typename T, elf::endian::Type endian>
Elf64_Half elf::Header<T, endian>::headerSize() {
    return endian::convert<endian>(mHeader->e_ehsize);
}

template<typename T, elf::endian::Type endian>
Elf64_Half elf::Header<T, endian>::segmentEntrySize() {
    return endian::convert<endian>(mHeader->e_phentsize);
}

template<typename T, elf::endian::Type endian>
Elf64_Half elf::Header<T, endian>::segmentNum() {
    return endian::convert<endian>(mHeader->e_phnum);
}

template<typename T, elf::endian::Type endian>
Elf64_Half elf::Header<T, endian>::sectionEntrySize() {
    return endian::convert<endian>(mHeader->e_shentsize);
}

template<typename T, elf::endian::Type endian>
Elf64_Half elf::Header<T, endian>::sectionNum() {
    return endian::convert<endian>(mHeader->e_shnum);
}

template<typename T, elf::endian::Type endian>
Elf64_Half elf::Header<T, endian>::sectionStrIndex() {
    return endian::convert<endian>(mHeader->e_shstrndx);
}

template class elf::Header<Elf32_Ehdr, elf::endian::Little>;
template class elf::Header<Elf32_Ehdr, elf::endian::Big>;
template class elf::Header<Elf64_Ehdr, elf::endian::Little>;
template class elf::Header<Elf64_Ehdr, elf::endian::Big>;