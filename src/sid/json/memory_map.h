/*
LICENSE: BEGIN
===============================================================================
@author Shan Anand
@email anand.gs@gmail.com
@source https://github.com/shan-anand
@brief Json handling using c++
===============================================================================
MIT License

Copyright (c) 2017 Shanmuga (Anand) Gunasekaran

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
===============================================================================
LICENSE: END
*/

#pragma once

#include <exception>
#include <fcntl.h>
#include <unistd.h>
#include <filesystem>
#include <sys/stat.h>
#include <sys/mman.h>

namespace sid::json {

struct memory_map
{
private:
  int    m_fd;
  void*  m_begin;
  void*  m_end;
  size_t m_size;

public:
  const char* begin() const { return (const char*) m_begin; }
  const char* end() const { return (const char*) m_end; }
  size_t size() const { return m_size; }

public:
  memory_map(const std::string& _file)
    : m_fd(-1), m_begin(MAP_FAILED), m_end(nullptr), m_size(0)
  {
    m_fd = ::open(_file.c_str(), O_RDONLY);
    if ( m_fd < 0 )
      throw std::system_error(errno, std::system_category(), _file);
    try
    {
      // Get the file size
      struct stat fileStat;
      if ( ::fstat(m_fd, &fileStat) < 0 )
        throw std::system_error(errno, std::system_category(), "memory_map:fstat");
      m_size = fileStat.st_size;
      // Memory map the entire file
      int flags = MAP_SHARED | MAP_POPULATE;
      m_begin = ::mmap(nullptr, m_size, PROT_READ, flags, m_fd, 0);
      if ( m_begin == MAP_FAILED )
        throw std::system_error(errno, std::system_category(), "memory_map:mmap");
    }
    catch (std::exception&)
    {
      ::close(m_fd);
      throw; // rethrow the exception
    }
    m_end = (void*) (((char*) m_begin) + m_size - 1);
  }

  ~memory_map()
  {
    if ( m_begin != MAP_FAILED )
    {
      ::munmap(m_begin, m_size);
      m_begin = MAP_FAILED;
    }
    if ( m_fd >= 0 )
    {
      ::close(m_fd);
      m_fd = -1;
    }
  }
};

} // namespace sid::json
