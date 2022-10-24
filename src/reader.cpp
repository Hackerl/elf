#include <elf/reader.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <filesystem>
#include <algorithm>

bool elf::Reader::load(const std::string &path) {
    size_t length = std::filesystem::file_size(path);

    if (length < EI_NIDENT)
        return false;

    int fd = open(path.c_str(), O_RDONLY);

    if (fd < 0)
        return false;

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
        return false;
    }

    close(fd);

    mBuffer = std::shared_ptr<void>(buffer, [=](void *ptr) {
        munmap(ptr, length);
    });

    auto ident = (unsigned char *) mBuffer.get();

    if (ident[EI_MAG0] != ELFMAG0 ||
        ident[EI_MAG1] != ELFMAG1 ||
        ident[EI_MAG2] != ELFMAG2 ||
        ident[EI_MAG3] != ELFMAG3)
        return false;

    if (ident[EI_CLASS] != ELFCLASS64 && ident[EI_CLASS] != ELFCLASS32)
        return false;

    if (ident[EI_DATA] != ELFDATA2LSB && ident[EI_DATA] != ELFDATA2MSB)
        return false;

    return true;
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
    std::unique_ptr<IHeader> hdr = header();
    std::vector<std::shared_ptr<elf::ISegment>> segments;

    for (Elf64_Half i = 0; i < hdr->segmentNum(); i++) {
        if (hdr->ident()[EI_CLASS] == ELFCLASS64) {
            if (hdr->ident()[EI_DATA] == ELFDATA2LSB) {
                segments.push_back(std::make_shared<Segment<Elf64_Phdr, endian::Little>>(
                        (Elf64_Phdr *) ((std::byte *) mBuffer.get() + hdr->segmentOffset() + i * hdr->segmentEntrySize()),
                        mBuffer
                ));
            } else {
                segments.push_back(std::make_shared<Segment<Elf64_Phdr, endian::Big>>(
                        (Elf64_Phdr *) ((std::byte *) mBuffer.get() + hdr->segmentOffset() + i * hdr->segmentEntrySize()),
                        mBuffer
                ));
            }
        } else {
            if (hdr->ident()[EI_DATA] == ELFDATA2LSB) {
                segments.push_back(std::make_shared<Segment<Elf32_Phdr, endian::Little>>(
                        (Elf32_Phdr *) ((std::byte *) mBuffer.get() + hdr->segmentOffset() + i * hdr->segmentEntrySize()),
                        mBuffer
                ));
            } else {
                segments.push_back(std::make_shared<Segment<Elf32_Phdr, endian::Big>>(
                        (Elf32_Phdr *) ((std::byte *) mBuffer.get() + hdr->segmentOffset() + i * hdr->segmentEntrySize()),
                        mBuffer
                ));
            }
        }
    }

    return segments;
}

std::vector<std::shared_ptr<elf::ISection>> elf::Reader::sections() const {
    std::unique_ptr<IHeader> hdr = header();
    std::vector<std::shared_ptr<elf::ISection>> sections;

    for (Elf64_Half i = 0; i < hdr->sectionNum(); i++) {
        if (hdr->ident()[EI_CLASS] == ELFCLASS64) {
            if (hdr->ident()[EI_DATA] == ELFDATA2LSB) {
                sections.push_back(std::make_shared<Section<Elf64_Shdr, endian::Little>>(
                        (Elf64_Shdr *) ((std::byte *) mBuffer.get() + hdr->sectionOffset() + i * hdr->sectionEntrySize()),
                        mBuffer
                ));
            } else {
                sections.push_back(std::make_shared<Section<Elf64_Shdr, endian::Big>>(
                        (Elf64_Shdr *) ((std::byte *) mBuffer.get() + hdr->sectionOffset() + i * hdr->sectionEntrySize()),
                        mBuffer
                ));
            }
        } else {
            if (hdr->ident()[EI_DATA] == ELFDATA2LSB) {
                sections.push_back(std::make_shared<Section<Elf32_Shdr, endian::Little>>(
                        (Elf32_Shdr *) ((std::byte *) mBuffer.get() + hdr->sectionOffset() + i * hdr->sectionEntrySize()),
                        mBuffer
                ));
            } else {
                sections.push_back(std::make_shared<Section<Elf32_Shdr, endian::Big>>(
                        (Elf32_Shdr *) ((std::byte *) mBuffer.get() + hdr->sectionOffset() + i * hdr->sectionEntrySize()),
                        mBuffer
                ));
            }
        }
    }

    for (const auto &section: sections) {
        section->name((char *) sections[hdr->sectionStrIndex()]->data() + section->nameIndex());
    }

    return sections;
}
