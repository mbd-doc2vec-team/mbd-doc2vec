#ifndef DATA_PROCESSING_FILE_H
#define DATA_PROCESSING_FILE_H

#include <stddef.h>
#include <stdint.h>

// Class that stores an mmaped file, offering a basic interface
// for clients to access the raw buffer of that file.
// The file is read only, and has ownership of its own resources.
// Handles obtained from the File are only valid for the lifetime
// of the File.
class File {
public:
  // Constructs a File with the given filename.
  // If there is an error constructing the file, prints
  // an error to stderr and exits the program.
  File(const char *const filename);

  // Delete copy constructor and assignment.
  File(const File&) = delete;
  File &operator=(const File&) = delete;

  // Releases the resources associated with the File.
  // If there is an error releasing resources, prints
  // an error to stderr and exits the program.
  ~File();

  // Returns the filename associated with the File.
  const char *filename() const { return filename_; }

  // Returns the raw memory buffer that the File manages.
  const uint8_t *buffer() const { return buf_; }

  // Returns the total size of the File.
  size_t size() const { return size_; }
private:
  // The filename of the File.
  const char *const filename_;

  // The file descriptor of the File.
  int fd_;

  // The size of the File.
  size_t size_;

  // The underlying buffer that the File uses.
  uint8_t *buf_;
};

#endif // DATA_PROCESSING_FILE_H
