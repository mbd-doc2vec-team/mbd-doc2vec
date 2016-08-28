#include "file.h"

#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

File::File(const char *const filename)
  : filename_(filename),
    fd_(-1),
    size_(0),
    buf_(nullptr)
{
  fd_ = open(filename, O_RDONLY);
  if (fd_ < 0) {
    perror("open");
    exit(1);
  }
  struct stat buf;
  if (fstat(fd_, &buf) < 0) {
    perror("stat");
    exit(1);
  }
  size_ = static_cast<size_t>(buf.st_size);
  buf_ = static_cast<uint8_t*>(
    mmap(nullptr, size_, PROT_READ, MAP_SHARED, fd_, 0)
  );
  if (buf_ == MAP_FAILED) {
    perror("mmap");
    exit(1);
  }
  if (madvise(buf_, size_, MADV_SEQUENTIAL) == -1) {
    perror("madvise");
    exit(1);
  }
}

File::~File()
{
  if (close(fd_) == -1) {
    perror("close");
    exit(1);
  }
  if (munmap(buf_, size_) == -1) {
    perror("munmap");
    exit(1);
  }
}
