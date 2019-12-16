#pragma once

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <errno.h>
#include <algorithm>
#include <system_error>

namespace utils
{

class UnixFd
{
    int fd_;
public:
    UnixFd() : fd_{-1} {}
    explicit UnixFd(int fd) : fd_{fd} {}

    UnixFd(const UnixFd &) = delete;
    UnixFd(UnixFd &&that) : fd_{that.fd_} { that.fd_ = -1; }
    UnixFd& operator =(const UnixFd &) = delete;

    UnixFd& operator =(UnixFd &&that)
    {
        std::swap(fd_, that.fd_);
        return *this;
    }

    bool valid() const { return fd_ >= 0; }

    operator int() const { return fd_; }

    void close()
    {
        if (fd_ >= 0)
            ::close(fd_);
        fd_ = -1;
    }

    ~UnixFd() { close(); }
};

struct Span
{
    const char *data;
    size_t size;
};

class MemoryMap
{
    void *addr_;
    size_t length_;

    void destroy_()
    {
        if (length_)
            ::munmap(addr_, length_);
        addr_ = nullptr;
        length_ = 0;
    }

public:
    MemoryMap() : addr_{nullptr}, length_{0} {}

    MemoryMap(const UnixFd &file, size_t length)
        : addr_{::mmap(nullptr, length, PROT_READ, MAP_PRIVATE, file, 0)}
        , length_{length}
    {
        if (addr_ == MAP_FAILED) {
            const auto saved_errno = errno;
            throw std::system_error(saved_errno, std::generic_category(), "mmap");
        }
    }

    MemoryMap(const MemoryMap &) = delete;

    MemoryMap(MemoryMap &&that)
        : addr_{that.addr_}
        , length_{that.length_}
    {
        that.addr_ = nullptr;
        that.length_ = 0;
    }

    MemoryMap& operator =(const MemoryMap &) = delete;

    MemoryMap& operator =(MemoryMap &&that)
    {
        std::swap(addr_, that.addr_);
        std::swap(length_, that.length_);
        return *this;
    }

    explicit operator Span() const
    {
        return {reinterpret_cast<char *>(addr_), length_};
    }

    ~MemoryMap() { destroy_(); }
};

template<class T>
static T check_retval(T retval, const char *what)
{
    if (retval >= 0)
        return retval;
    const auto saved_errno = errno;
    throw std::system_error(saved_errno, std::generic_category(), what);
}

static UnixFd open_for_reading(const char *path)
{
    return UnixFd(check_retval(
        ::open(path, O_RDONLY),
        path
    ));
}

static size_t file_size(const UnixFd &fd)
{
    struct stat s;
    check_retval(
        fstat(fd, &s),
        "fstat"
    );
    return s.st_size;
}

}
