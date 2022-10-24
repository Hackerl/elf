#ifndef ELF_SYMBOL_H
#define ELF_SYMBOL_H

#include "reader.h"

namespace elf {
    class ISymbol {
    public:
        virtual ~ISymbol() = default;

    public:
        virtual std::string name() = 0;
        virtual void name(std::string_view name) = 0;

    public:
        virtual Elf64_Word nameIndex() = 0;
        virtual unsigned char info() = 0;
        virtual unsigned char other() = 0;
        virtual Elf64_Section sectionIndex() = 0;
        virtual Elf64_Addr value() = 0;
        virtual Elf64_Xword size() = 0;
    };

    template<typename T, endian::Type endian>
    class Symbol : public ISymbol {
    public:
        explicit Symbol(T *symbol);

    public:
        std::string name() override;
        void name(std::string_view name) override;

    public:
        Elf64_Word nameIndex() override;
        unsigned char info() override;
        unsigned char other() override;
        Elf64_Section sectionIndex() override;
        Elf64_Addr value() override;
        Elf64_Xword size() override;

    private:
        const T *mSymbol;
        std::string mName;
    };

    class SymbolIterator {
    public:
        SymbolIterator(const std::byte *symbol, size_t size, endian::Type endian, std::shared_ptr<ISection> section);

    public:
        std::unique_ptr<ISymbol> operator*();
        SymbolIterator &operator++();
        SymbolIterator &operator+(size_t offset);

    public:
        bool operator==(const SymbolIterator &rhs);
        bool operator!=(const SymbolIterator &rhs);

    private:
        size_t mSize;
        endian::Type mEndian;
        const std::byte *mSymbol;
        std::shared_ptr<ISection> mSection;
    };

    class SymbolTable {
    public:
        SymbolTable(Reader reader, std::shared_ptr<ISection> section);

    public:
        std::unique_ptr<ISymbol> operator[](size_t index);

    public:
        SymbolIterator begin();
        SymbolIterator end();

    private:
        Reader mReader;
        std::shared_ptr<ISection> mSection;
    };
}

#endif //ELF_SYMBOL_H
