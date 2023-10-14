#include <elf/section.h>
#include <elf/endian.h>

template<typename T, elf::endian::Type Endian>
elf::Section<T, Endian>::Section(const T *section, std::shared_ptr<void> buffer)
        : mSection(section), mBuffer(std::move(buffer)) {

}

template<typename T, elf::endian::Type Endian>
std::string elf::Section<T, Endian>::name() {
    return mName;
}

template<typename T, elf::endian::Type Endian>
void elf::Section<T, Endian>::name(std::string_view name) {
    mName = name;
}

template<typename T, elf::endian::Type Endian>
const std::byte *elf::Section<T, Endian>::data() {
    return (const std::byte *) mBuffer.get() + offset();
}

template<typename T, elf::endian::Type Endian>
Elf64_Word elf::Section<T, Endian>::nameIndex() {
    return endian::convert<Endian>(mSection->sh_name);
}

template<typename T, elf::endian::Type Endian>
Elf64_Word elf::Section<T, Endian>::type() {
    return endian::convert<Endian>(mSection->sh_type);
}

template<typename T, elf::endian::Type Endian>
Elf64_Xword elf::Section<T, Endian>::flags() {
    return endian::convert<Endian>(mSection->sh_flags);
}

template<typename T, elf::endian::Type Endian>
Elf64_Addr elf::Section<T, Endian>::address() {
    return endian::convert<Endian>(mSection->sh_addr);
}

template<typename T, elf::endian::Type Endian>
Elf64_Off elf::Section<T, Endian>::offset() {
    return endian::convert<Endian>(mSection->sh_offset);
}

template<typename T, elf::endian::Type Endian>
Elf64_Xword elf::Section<T, Endian>::size() {
    return endian::convert<Endian>(mSection->sh_size);
}

template<typename T, elf::endian::Type Endian>
Elf64_Word elf::Section<T, Endian>::link() {
    return endian::convert<Endian>(mSection->sh_link);
}

template<typename T, elf::endian::Type Endian>
Elf64_Word elf::Section<T, Endian>::info() {
    return endian::convert<Endian>(mSection->sh_info);
}

template<typename T, elf::endian::Type Endian>
Elf64_Xword elf::Section<T, Endian>::addressAlign() {
    return endian::convert<Endian>(mSection->sh_addralign);
}

template<typename T, elf::endian::Type Endian>
Elf64_Xword elf::Section<T, Endian>::entrySize() {
    return endian::convert<Endian>(mSection->sh_entsize);
}

template
class elf::Section<Elf32_Shdr, elf::endian::Little>;

template
class elf::Section<Elf32_Shdr, elf::endian::Big>;

template
class elf::Section<Elf64_Shdr, elf::endian::Little>;

template
class elf::Section<Elf64_Shdr, elf::endian::Big>;