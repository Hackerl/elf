#include <elf/header.h>

template<typename T, elf::endian::Type Endian>
elf::Header<T, Endian>::Header(const T *header) : mHeader(header) {

}

template<typename T, elf::endian::Type Endian>
const unsigned char *elf::Header<T, Endian>::ident() {
    return mHeader->e_ident;
}

template<typename T, elf::endian::Type Endian>
Elf64_Half elf::Header<T, Endian>::type() {
    return endian::convert<Endian>(mHeader->e_type);
}

template<typename T, elf::endian::Type Endian>
Elf64_Half elf::Header<T, Endian>::machine() {
    return endian::convert<Endian>(mHeader->e_machine);
}

template<typename T, elf::endian::Type Endian>
Elf64_Word elf::Header<T, Endian>::version() {
    return endian::convert<Endian>(mHeader->e_version);
}

template<typename T, elf::endian::Type Endian>
Elf64_Addr elf::Header<T, Endian>::entry() {
    return endian::convert<Endian>(mHeader->e_entry);
}

template<typename T, elf::endian::Type Endian>
Elf64_Off elf::Header<T, Endian>::segmentOffset() {
    return endian::convert<Endian>(mHeader->e_phoff);
}

template<typename T, elf::endian::Type Endian>
Elf64_Off elf::Header<T, Endian>::sectionOffset() {
    return endian::convert<Endian>(mHeader->e_shoff);
}

template<typename T, elf::endian::Type Endian>
Elf64_Word elf::Header<T, Endian>::flags() {
    return endian::convert<Endian>(mHeader->e_flags);
}

template<typename T, elf::endian::Type Endian>
Elf64_Half elf::Header<T, Endian>::headerSize() {
    return endian::convert<Endian>(mHeader->e_ehsize);
}

template<typename T, elf::endian::Type Endian>
Elf64_Half elf::Header<T, Endian>::segmentEntrySize() {
    return endian::convert<Endian>(mHeader->e_phentsize);
}

template<typename T, elf::endian::Type Endian>
Elf64_Half elf::Header<T, Endian>::segmentNum() {
    return endian::convert<Endian>(mHeader->e_phnum);
}

template<typename T, elf::endian::Type Endian>
Elf64_Half elf::Header<T, Endian>::sectionEntrySize() {
    return endian::convert<Endian>(mHeader->e_shentsize);
}

template<typename T, elf::endian::Type Endian>
Elf64_Half elf::Header<T, Endian>::sectionNum() {
    return endian::convert<Endian>(mHeader->e_shnum);
}

template<typename T, elf::endian::Type Endian>
Elf64_Half elf::Header<T, Endian>::sectionStrIndex() {
    return endian::convert<Endian>(mHeader->e_shstrndx);
}

template
class elf::Header<Elf32_Ehdr, elf::endian::Little>;

template
class elf::Header<Elf32_Ehdr, elf::endian::Big>;

template
class elf::Header<Elf64_Ehdr, elf::endian::Little>;

template
class elf::Header<Elf64_Ehdr, elf::endian::Big>;