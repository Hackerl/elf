#include <elf/section.h>
#include <elf/endian.h>

template<typename T, elf::endian::Type endian>
elf::Section<T, endian>::Section(T *section, std::shared_ptr<void> buffer) : mSection(section), mBuffer(std::move(buffer)) {

}

template<typename T, elf::endian::Type endian>
std::string elf::Section<T, endian>::name() {
    return mName;
}

template<typename T, elf::endian::Type endian>
void elf::Section<T, endian>::name(std::string_view name) {
    mName = name;
}

template<typename T, elf::endian::Type endian>
const std::byte *elf::Section<T, endian>::data() {
    return (const std::byte *) mBuffer.get() + offset();
}

template<typename T, elf::endian::Type endian>
Elf64_Word elf::Section<T, endian>::nameIndex() {
    return endian::convert<endian>(mSection->sh_name);
}

template<typename T, elf::endian::Type endian>
Elf64_Word elf::Section<T, endian>::type() {
    return endian::convert<endian>(mSection->sh_type);
}

template<typename T, elf::endian::Type endian>
Elf64_Xword elf::Section<T, endian>::flags() {
    return endian::convert<endian>(mSection->sh_flags);
}

template<typename T, elf::endian::Type endian>
Elf64_Addr elf::Section<T, endian>::address() {
    return endian::convert<endian>(mSection->sh_addr);
}

template<typename T, elf::endian::Type endian>
Elf64_Off elf::Section<T, endian>::offset() {
    return endian::convert<endian>(mSection->sh_offset);
}

template<typename T, elf::endian::Type endian>
Elf64_Xword elf::Section<T, endian>::size() {
    return endian::convert<endian>(mSection->sh_size);
}

template<typename T, elf::endian::Type endian>
Elf64_Word elf::Section<T, endian>::link() {
    return endian::convert<endian>(mSection->sh_link);
}

template<typename T, elf::endian::Type endian>
Elf64_Word elf::Section<T, endian>::info() {
    return endian::convert<endian>(mSection->sh_info);
}

template<typename T, elf::endian::Type endian>
Elf64_Xword elf::Section<T, endian>::addressAlign() {
    return endian::convert<endian>(mSection->sh_addralign);
}

template<typename T, elf::endian::Type endian>
Elf64_Xword elf::Section<T, endian>::entrySize() {
    return endian::convert<endian>(mSection->sh_entsize);
}

template class elf::Section<Elf32_Shdr, elf::endian::Little>;
template class elf::Section<Elf32_Shdr, elf::endian::Big>;
template class elf::Section<Elf64_Shdr, elf::endian::Little>;
template class elf::Section<Elf64_Shdr, elf::endian::Big>;