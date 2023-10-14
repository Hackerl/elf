#ifndef ELF_RELOCATION_H
#define ELF_RELOCATION_H

#include "symbol.h"

namespace elf {
    class IRelocation {
    public:
        virtual ~IRelocation() = default;

    public:
        virtual std::shared_ptr<ISymbol> symbol() = 0;
        virtual void symbol(std::shared_ptr<ISymbol> symbol) = 0;

    public:
        virtual Elf64_Addr offset() = 0;
        virtual Elf64_Xword info() = 0;
        virtual Elf64_Sxword addend() = 0;
        virtual Elf64_Xword type() = 0;
        virtual Elf64_Xword symbolIndex() = 0;
    };

    template<typename T, endian::Type Endian>
    class Relocation : public IRelocation {
    public:
        explicit Relocation(const T *relocation);

    public:
        std::shared_ptr<ISymbol> symbol() override;
        void symbol(std::shared_ptr<ISymbol> symbol) override;

    public:
        Elf64_Addr offset() override;
        Elf64_Xword info() override;
        Elf64_Sxword addend() override;
        Elf64_Xword type() override;
        Elf64_Xword symbolIndex() override;

    private:
        const T *mRelocation;
        std::shared_ptr<ISymbol> mSymbol;
    };

    class RelocationIterator {
    public:
        RelocationIterator(const std::byte *relocation, size_t size, endian::Type endian, bool addend, SymbolTable symbolTable);

    public:
        std::unique_ptr<IRelocation> operator*();
        RelocationIterator &operator++();
        RelocationIterator &operator+(size_t offset);

    public:
        bool operator==(const RelocationIterator &rhs);
        bool operator!=(const RelocationIterator &rhs);

    private:
        bool mAddend;
        size_t mSize;
        endian::Type mEndian;
        SymbolTable mSymbolTable;
        const std::byte *mRelocation;
    };

    class RelocationTable {
    public:
        RelocationTable(Reader reader, std::shared_ptr<ISection> section);

    public:
        size_t size();

    public:
        std::unique_ptr<IRelocation> operator[](size_t index);

    public:
        RelocationIterator begin();
        RelocationIterator end();

    private:
        Reader mReader;
        std::shared_ptr<ISection> mSection;
    };
}

#endif //ELF_RELOCATION_H
