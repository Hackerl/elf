#include <elf/reader.h>
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
        if (ident[EI_DATA] == ELFDATA2LSB) {
            return std::make_unique<Header<Elf64_Ehdr, endian::Little>>((Elf64_Ehdr *) mBuffer.get());
        } else {
            return std::make_unique<Header<Elf64_Ehdr, endian::Big>>((Elf64_Ehdr *) mBuffer.get());
        }
    } else {
        if (ident[EI_DATA] == ELFDATA2LSB) {
            return std::make_unique<Header<Elf32_Ehdr, endian::Little>>((Elf32_Ehdr *) mBuffer.get());
        } else {
            return std::make_unique<Header<Elf32_Ehdr, endian::Big>>((Elf32_Ehdr *) mBuffer.get());
        }
    }
}

std::vector<std::shared_ptr<elf::ISegment>> elf::Reader::segments() const {
    std::unique_ptr<IHeader> header = this->header();
    std::vector<std::shared_ptr<elf::ISegment>> segments;

    for (Elf64_Half i = 0; i < header->segmentNum(); i++) {
        if (header->ident()[EI_CLASS] == ELFCLASS64) {
            if (header->ident()[EI_DATA] == ELFDATA2LSB) {
                segments.push_back(std::make_shared<Segment<Elf64_Phdr, endian::Little>>(
                        (Elf64_Phdr *) ((std::byte *) mBuffer.get() + header->segmentOffset() + i * header->segmentEntrySize()),
                        mBuffer
                ));
            } else {
                segments.push_back(std::make_shared<Segment<Elf64_Phdr, endian::Big>>(
                        (Elf64_Phdr *) ((std::byte *) mBuffer.get() + header->segmentOffset() + i * header->segmentEntrySize()),
                        mBuffer
                ));
            }
        } else {
            if (header->ident()[EI_DATA] == ELFDATA2LSB) {
                segments.push_back(std::make_shared<Segment<Elf32_Phdr, endian::Little>>(
                        (Elf32_Phdr *) ((std::byte *) mBuffer.get() + header->segmentOffset() + i * header->segmentEntrySize()),
                        mBuffer
                ));
            } else {
                segments.push_back(std::make_shared<Segment<Elf32_Phdr, endian::Big>>(
                        (Elf32_Phdr *) ((std::byte *) mBuffer.get() + header->segmentOffset() + i * header->segmentEntrySize()),
                        mBuffer
                ));
            }
        }
    }

    return segments;
}

std::vector<std::shared_ptr<elf::ISection>> elf::Reader::sections() const {
    std::unique_ptr<IHeader> header = this->header();
    std::vector<std::shared_ptr<elf::ISection>> sections;

    for (Elf64_Half i = 0; i < header->sectionNum(); i++) {
        if (header->ident()[EI_CLASS] == ELFCLASS64) {
            if (header->ident()[EI_DATA] == ELFDATA2LSB) {
                sections.push_back(std::make_shared<Section<Elf64_Shdr, endian::Little>>(
                        (Elf64_Shdr *) ((std::byte *) mBuffer.get() + header->sectionOffset() + i * header->sectionEntrySize()),
                        mBuffer
                ));
            } else {
                sections.push_back(std::make_shared<Section<Elf64_Shdr, endian::Big>>(
                        (Elf64_Shdr *) ((std::byte *) mBuffer.get() + header->sectionOffset() + i * header->sectionEntrySize()),
                        mBuffer
                ));
            }
        } else {
            if (header->ident()[EI_DATA] == ELFDATA2LSB) {
                sections.push_back(std::make_shared<Section<Elf32_Shdr, endian::Little>>(
                        (Elf32_Shdr *) ((std::byte *) mBuffer.get() + header->sectionOffset() + i * header->sectionEntrySize()),
                        mBuffer
                ));
            } else {
                sections.push_back(std::make_shared<Section<Elf32_Shdr, endian::Big>>(
                        (Elf32_Shdr *) ((std::byte *) mBuffer.get() + header->sectionOffset() + i * header->sectionEntrySize()),
                        mBuffer
                ));
            }
        }
    }

    for (const auto &section: sections) {
        section->name((char *) sections[header->sectionStrIndex()]->data() + section->nameIndex());
    }

    return sections;
}

const std::byte *elf::Reader::virtualMemory(Elf64_Addr address) const {
    std::vector<std::shared_ptr<elf::ISegment>> segments = this->segments();

    auto it = std::find_if(
            segments.begin(),
            segments.end(),
            [=](const auto &segment) {
                if (segment->type() != PT_LOAD)
                    return false;

                return address >= segment->virtualAddress() && address <= segment->virtualAddress() + segment->fileSize() - 1;
            }
    );

    if (it == segments.end())
        return nullptr;

    return it->operator*().data() + address - it->operator*().virtualAddress();
}

std::optional<std::vector<std::byte>> elf::Reader::readVirtualMemory(Elf64_Addr address, Elf64_Xword length) const {
    std::vector<std::shared_ptr<elf::ISegment>> segments = this->segments();

    auto it = std::find_if(
            segments.begin(),
            segments.end(),
            [=](const auto &segment) {
                if (segment->type() != PT_LOAD)
                    return false;

                return address >= segment->virtualAddress() && address <= segment->virtualAddress() + segment->fileSize() - 1;
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

std::optional<elf::Reader> elf::openFile(const std::filesystem::path &path) {
    std::error_code ec;
    size_t length = std::filesystem::file_size(path, ec);

    if (ec || length < EI_NIDENT)
        return std::nullopt;

    int fd = open(path.string().c_str(), O_RDONLY);

    if (fd < 0)
        return std::nullopt;

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
        return std::nullopt;
    }

    close(fd);

    auto ident = (unsigned char *) buffer;

    if (ident[EI_MAG0] != ELFMAG0 ||
        ident[EI_MAG1] != ELFMAG1 ||
        ident[EI_MAG2] != ELFMAG2 ||
        ident[EI_MAG3] != ELFMAG3) {
        munmap(buffer, length);
        return std::nullopt;
    }

    if (ident[EI_CLASS] != ELFCLASS64 && ident[EI_CLASS] != ELFCLASS32) {
        munmap(buffer, length);
        return std::nullopt;
    }

    if (ident[EI_DATA] != ELFDATA2LSB && ident[EI_DATA] != ELFDATA2MSB) {
        munmap(buffer, length);
        return std::nullopt;
    }

    return Reader(std::shared_ptr<void>(buffer, [=](void *ptr) {
        munmap(ptr, length);
    }));
}
