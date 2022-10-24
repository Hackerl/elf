#include <elf/relocation.h>

template<typename T, elf::endian::Type endian>
elf::Relocation<T, endian>::Relocation(T *relocation) : mRelocation(relocation) {

}

template<typename T, elf::endian::Type endian>
std::shared_ptr<elf::ISymbol> elf::Relocation<T, endian>::symbol() {
    return mSymbol;
}

template<typename T, elf::endian::Type endian>
void elf::Relocation<T, endian>::symbol(std::shared_ptr<ISymbol> symbol) {
    mSymbol = std::move(symbol);
}

template<typename T, elf::endian::Type endian>
Elf64_Addr elf::Relocation<T, endian>::offset() {
    return endian::convert<endian>(mRelocation->r_offset);
}

template<typename T, elf::endian::Type endian>
Elf64_Xword elf::Relocation<T, endian>::info() {
    return endian::convert<endian>(mRelocation->r_info);
}

template<typename T, elf::endian::Type endian>
Elf64_Sxword elf::Relocation<T, endian>::addend() {
    if constexpr (std::is_same_v<T, Elf32_Rel> || std::is_same_v<T, Elf64_Rel>) {
        return 0;
    } else {
        return endian::convert<endian>(mRelocation->r_addend);
    }
}

template<typename T, elf::endian::Type endian>
Elf64_Xword elf::Relocation<T, endian>::type() {
    if constexpr (std::is_same_v<T, Elf32_Rel> || std::is_same_v<T, Elf32_Rela>) {
        return ELF32_R_TYPE(info());
    } else {
        return ELF64_R_TYPE(info());
    }
}

template<typename T, elf::endian::Type endian>
Elf64_Xword elf::Relocation<T, endian>::symbolIndex() {
    if constexpr (std::is_same_v<T, Elf32_Rel> || std::is_same_v<T, Elf32_Rela>) {
        return ELF32_R_SYM(info());
    } else {
        return ELF64_R_SYM(info());
    }
}

elf::RelocationIterator::RelocationIterator(
        const std::byte *relocation,
        size_t size,
        elf::endian::Type endian,
        bool addend,
        elf::SymbolTable symbolTable
) : mRelocation(relocation), mSize(size), mEndian(endian), mAddend(addend), mSymbolTable(std::move(symbolTable)){

}

std::unique_ptr<elf::IRelocation> elf::RelocationIterator::operator*() {
    std::unique_ptr<elf::IRelocation> relocation;

    if (mAddend) {
        if (mSize == sizeof(Elf64_Rela)) {
            if (mEndian == endian::Little) {
                relocation = std::make_unique<Relocation<Elf64_Rela, endian::Little>>((Elf64_Rela *) mRelocation);
            } else {
                relocation = std::make_unique<Relocation<Elf64_Rela, endian::Big>>((Elf64_Rela *) mRelocation);
            }
        } else {
            if (mEndian == endian::Little) {
                relocation = std::make_unique<Relocation<Elf32_Rela, endian::Little>>((Elf32_Rela *) mRelocation);
            } else {
                relocation = std::make_unique<Relocation<Elf32_Rela, endian::Big>>((Elf32_Rela *) mRelocation);
            }
        }
    } else {
        if (mSize == sizeof(Elf64_Rel)) {
            if (mEndian == endian::Little) {
                relocation = std::make_unique<Relocation<Elf64_Rel, endian::Little>>((Elf64_Rel *) mRelocation);
            } else {
                relocation = std::make_unique<Relocation<Elf64_Rel, endian::Big>>((Elf64_Rel *) mRelocation);
            }
        } else {
            if (mEndian == endian::Little) {
                relocation = std::make_unique<Relocation<Elf32_Rel, endian::Little>>((Elf32_Rel *) mRelocation);
            } else {
                relocation = std::make_unique<Relocation<Elf32_Rel, endian::Big>>((Elf32_Rel *) mRelocation);
            }
        }
    }

    relocation->symbol(mSymbolTable[relocation->symbolIndex()]);

    return relocation;
}

elf::RelocationIterator &elf::RelocationIterator::operator++() {
    mRelocation += mSize;
    return *this;
}

elf::RelocationIterator &elf::RelocationIterator::operator+(size_t offset) {
    mRelocation += offset * mSize;
    return *this;
}

bool elf::RelocationIterator::operator==(const elf::RelocationIterator &rhs) {
    return mRelocation == rhs.mRelocation;
}

bool elf::RelocationIterator::operator!=(const elf::RelocationIterator &rhs) {
    return !operator==(rhs);
}

elf::RelocationTable::RelocationTable(elf::Reader reader, std::shared_ptr<ISection> section) : mReader(std::move(reader)), mSection(std::move(section)) {

}

size_t elf::RelocationTable::size() {
    return mSection->size() / mSection->entrySize();
}

std::unique_ptr<elf::IRelocation> elf::RelocationTable::operator[](size_t index) {
    return *(begin() + index);
}

elf::RelocationIterator elf::RelocationTable::begin() {
    return {
            mSection->data(),
            mSection->entrySize(),
            mReader.header()->ident()[EI_DATA] == ELFDATA2LSB ? endian::Little : endian::Big,
            mSection->type() == SHT_RELA,
            SymbolTable{mReader, mReader.sections()[mSection->link()]}
    };
}

elf::RelocationIterator elf::RelocationTable::end() {
    return begin() + size();
}

template class elf::Relocation<Elf32_Rel, elf::endian::Little>;
template class elf::Relocation<Elf32_Rel, elf::endian::Big>;
template class elf::Relocation<Elf64_Rel, elf::endian::Little>;
template class elf::Relocation<Elf64_Rel, elf::endian::Big>;
template class elf::Relocation<Elf32_Rela, elf::endian::Little>;
template class elf::Relocation<Elf32_Rela, elf::endian::Big>;
template class elf::Relocation<Elf64_Rela, elf::endian::Little>;
template class elf::Relocation<Elf64_Rela, elf::endian::Big>;