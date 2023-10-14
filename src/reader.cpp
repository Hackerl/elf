#include <elf/reader.h>
#include <elf/error.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <filesystem>
#include <algorithm>

elf::Reader::Reader(std::shared_ptr<void> buffer) : mBuffer(std::move(buffer)) {

}

std::unique_ptr<elf::IHeader> elf::Reader::header() const {
    auto ident = (unsigned char *) mBuffer.get();

    if (ident[EI_CLASS] == ELFCLASS64) {
        if (ident[EI_DATA] == ELFDATA2LSB)
            return std::make_unique<Header<Elf64_Ehdr, endian::Little>>((const Elf64_Ehdr *) mBuffer.get());
        else
            return std::make_unique<Header<Elf64_Ehdr, endian::Big>>((const Elf64_Ehdr *) mBuffer.get());
    } else {
        if (ident[EI_DATA] == ELFDATA2LSB)
            return std::make_unique<Header<Elf32_Ehdr, endian::Little>>((const Elf32_Ehdr *) mBuffer.get());
        else
            return std::make_unique<Header<Elf32_Ehdr, endian::Big>>((const Elf32_Ehdr *) mBuffer.get());
    }
}

std::vector<std::shared_ptr<elf::ISegment>> elf::Reader::segments() const {
    auto header = this->header();
    std::vector<std::shared_ptr<elf::ISegment>> segments;

    for (Elf64_Half i = 0; i < header->segmentNum(); i++) {
        if (header->ident()[EI_CLASS] == ELFCLASS64) {
            auto segment = (const Elf64_Phdr *) (
                    (const std::byte *) mBuffer.get() +
                    header->segmentOffset() +
                    i * header->segmentEntrySize()
            );

            if (header->ident()[EI_DATA] == ELFDATA2LSB)
                segments.push_back(std::make_shared<Segment<Elf64_Phdr, endian::Little>>(segment, mBuffer));
            else
                segments.push_back(std::make_shared<Segment<Elf64_Phdr, endian::Big>>(segment, mBuffer));
        } else {
            auto segment = (const Elf32_Phdr *) (
                    (const std::byte *) mBuffer.get() +
                    header->segmentOffset() +
                    i * header->segmentEntrySize()
            );

            if (header->ident()[EI_DATA] == ELFDATA2LSB)
                segments.push_back(std::make_shared<Segment<Elf32_Phdr, endian::Little>>(segment, mBuffer));
            else
                segments.push_back(std::make_shared<Segment<Elf32_Phdr, endian::Big>>(segment, mBuffer));
        }
    }

    return segments;
}

std::vector<std::shared_ptr<elf::ISection>> elf::Reader::sections() const {
    auto header = this->header();
    std::vector<std::shared_ptr<elf::ISection>> sections;

    for (Elf64_Half i = 0; i < header->sectionNum(); i++) {
        if (header->ident()[EI_CLASS] == ELFCLASS64) {
            auto section = (const Elf64_Shdr *) (
                    (std::byte *) mBuffer.get() +
                    header->sectionOffset() +
                    i * header->sectionEntrySize()
            );

            if (header->ident()[EI_DATA] == ELFDATA2LSB)
                sections.push_back(std::make_shared<Section<Elf64_Shdr, endian::Little>>(section, mBuffer));
            else
                sections.push_back(std::make_shared<Section<Elf64_Shdr, endian::Big>>(section, mBuffer));
        } else {
            auto section = (const Elf32_Shdr *) (
                    (std::byte *) mBuffer.get() +
                    header->sectionOffset() +
                    i * header->sectionEntrySize()
            );

            if (header->ident()[EI_DATA] == ELFDATA2LSB)
                sections.push_back(std::make_shared<Section<Elf32_Shdr, endian::Little>>(section, mBuffer));
            else
                sections.push_back(std::make_shared<Section<Elf32_Shdr, endian::Big>>(section, mBuffer));
        }
    }

    for (const auto &section: sections)
        section->name((char *) sections[header->sectionStrIndex()]->data() + section->nameIndex());

    return sections;
}

const std::byte *elf::Reader::virtualMemory(Elf64_Addr address) const {
    auto segments = this->segments();

    auto it = std::find_if(
            segments.begin(),
            segments.end(),
            [=](const auto &segment) {
                if (segment->type() != PT_LOAD)
                    return false;

                return address >= segment->virtualAddress() &&
                       address <= segment->virtualAddress() + segment->fileSize() - 1;
            }
    );

    if (it == segments.end())
        return nullptr;

    return it->operator*().data() + address - it->operator*().virtualAddress();
}

std::optional<std::vector<std::byte>> elf::Reader::readVirtualMemory(Elf64_Addr address, Elf64_Xword length) const {
    auto segments = this->segments();

    auto it = std::find_if(
            segments.begin(),
            segments.end(),
            [=](const auto &segment) {
                if (segment->type() != PT_LOAD)
                    return false;

                return address >= segment->virtualAddress() &&
                       address <= segment->virtualAddress() + segment->fileSize() - 1;
            }
    );

    if (it == segments.end())
        return std::nullopt;

    if (it->operator*().virtualAddress() + it->operator*().fileSize() - address < length)
        return std::nullopt;

    return std::vector<std::byte>{
            it->operator*().data() + address - it->operator*().virtualAddress(),
            it->operator*().data() + address - it->operator*().virtualAddress() + length
    };
}

tl::expected<elf::Reader, std::error_code> elf::openFile(const std::filesystem::path &path) {
    std::error_code ec;
    size_t length = std::filesystem::file_size(path, ec);

    if (ec != std::errc())
        return tl::unexpected(ec);

    if (length < EI_NIDENT)
        return tl::unexpected(Error::INVALID_ELF_HEADER);

    int fd = open(path.string().c_str(), O_RDONLY);

    if (fd < 0)
        return tl::unexpected(std::error_code(errno, std::system_category()));

    void *buffer = mmap(
            nullptr,
            length,
            PROT_READ,
            MAP_PRIVATE,
            fd,
            0
    );

    if (buffer == MAP_FAILED) {
        close(fd);
        return tl::unexpected(std::error_code(errno, std::system_category()));
    }

    close(fd);

    auto ident = (unsigned char *) buffer;

    if (ident[EI_MAG0] != ELFMAG0 ||
        ident[EI_MAG1] != ELFMAG1 ||
        ident[EI_MAG2] != ELFMAG2 ||
        ident[EI_MAG3] != ELFMAG3) {
        munmap(buffer, length);
        return tl::unexpected(Error::INVALID_ELF_MAGIC);
    }

    if (ident[EI_CLASS] != ELFCLASS64 && ident[EI_CLASS] != ELFCLASS32) {
        munmap(buffer, length);
        return tl::unexpected(Error::INVALID_ELF_CLASS);
    }

    if (ident[EI_DATA] != ELFDATA2LSB && ident[EI_DATA] != ELFDATA2MSB) {
        munmap(buffer, length);
        return tl::unexpected(Error::INVALID_ELF_ENDIAN);
    }

    return Reader(std::shared_ptr<void>(buffer, [=](void *ptr) {
        munmap(ptr, length);
    }));
}
