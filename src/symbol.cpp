#include <elf/symbol.h>

template<typename T, elf::endian::Type endian>
elf::Symbol<T, endian>::Symbol(T *symbol) : mSymbol(symbol) {

}

template<typename T, elf::endian::Type endian>
std::string elf::Symbol<T, endian>::name() {
    return mName;
}

template<typename T, elf::endian::Type endian>
void elf::Symbol<T, endian>::name(std::string_view name) {
    mName = name;
}

template<typename T, elf::endian::Type endian>
Elf64_Word elf::Symbol<T, endian>::nameIndex() {
    return endian::convert<endian>(mSymbol->st_name);
}

template<typename T, elf::endian::Type endian>
unsigned char elf::Symbol<T, endian>::info() {
    return mSymbol->st_info;
}

template<typename T, elf::endian::Type endian>
unsigned char elf::Symbol<T, endian>::other() {
    return mSymbol->st_other;
}

template<typename T, elf::endian::Type endian>
Elf64_Section elf::Symbol<T, endian>::sectionIndex() {
    return endian::convert<endian>(mSymbol->st_shndx);
}

template<typename T, elf::endian::Type endian>
Elf64_Addr elf::Symbol<T, endian>::value() {
    return endian::convert<endian>(mSymbol->st_value);
}

template<typename T, elf::endian::Type endian>
Elf64_Xword elf::Symbol<T, endian>::size() {
    return endian::convert<endian>(mSymbol->st_size);
}

elf::SymbolIterator::SymbolIterator(
        const std::byte *symbol,
        size_t size,
        endian::Type endian,
        std::shared_ptr<ISection> section
) : mSymbol(symbol), mSize(size), mEndian(endian), mSection(std::move(section)) {

}

std::unique_ptr<elf::ISymbol> elf::SymbolIterator::operator*() {
    std::unique_ptr<elf::ISymbol> symbol;

    if (mSize == sizeof(Elf64_Sym)) {
        if (mEndian == endian::Little) {
            symbol = std::make_unique<Symbol<Elf64_Sym, endian::Little>>((Elf64_Sym *) mSymbol);
        } else {
            symbol = std::make_unique<Symbol<Elf64_Sym, endian::Big>>((Elf64_Sym *) mSymbol);
        }
    } else {
        if (mEndian == endian::Little) {
            symbol = std::make_unique<Symbol<Elf32_Sym, endian::Little>>((Elf32_Sym *) mSymbol);
        } else {
            symbol = std::make_unique<Symbol<Elf32_Sym, endian::Big>>((Elf32_Sym *) mSymbol);
        }
    }

    if (!symbol->nameIndex())
        return symbol;

    symbol->name((char *) mSection->data() + symbol->nameIndex());

    return symbol;
}

elf::SymbolIterator &elf::SymbolIterator::operator++() {
    mSymbol += mSize;
    return *this;
}

elf::SymbolIterator &elf::SymbolIterator::operator+(size_t offset) {
    mSymbol += offset * mSize;
    return *this;
}

bool elf::SymbolIterator::operator==(const elf::SymbolIterator &rhs) {
    return mSymbol == rhs.mSymbol;
}

bool elf::SymbolIterator::operator!=(const elf::SymbolIterator &rhs) {
    return !operator==(rhs);
}

elf::SymbolTable::SymbolTable(elf::Reader reader, std::shared_ptr<ISection> section) : mReader(std::move(reader)), mSection(std::move(section)) {

}

std::unique_ptr<elf::ISymbol> elf::SymbolTable::operator[](size_t index) {
    return *(begin() + index);
}

elf::SymbolIterator elf::SymbolTable::begin() {
    return {
            mSection->data(),
            mSection->entrySize(),
            mReader.header()->ident()[EI_DATA] == ELFDATA2LSB ? endian::Little : endian::Big,
            mReader.sections()[mSection->link()]
    };
}

elf::SymbolIterator elf::SymbolTable::end() {
    return begin() + mSection->size() / mSection->entrySize();
}

template class elf::Symbol<Elf32_Sym, elf::endian::Little>;
template class elf::Symbol<Elf32_Sym, elf::endian::Big>;
template class elf::Symbol<Elf64_Sym, elf::endian::Little>;
template class elf::Symbol<Elf64_Sym, elf::endian::Big>;