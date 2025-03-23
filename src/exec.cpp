#include <functional>
#include <filesystem>
#include <csignal>

#include <fcntl.h>
#include <sys/wait.h>

#include "exec.hpp"

namespace exec {

  /// The buffer size used for reading.
  constexpr uint_fast16_t buffer_size = 1024;

  // Blocking Parser that merely waits until the pid closes.
  void wait_for(const int& fd, const int& pid) {close(fd); int state; waitpid(pid, &state, 0);}

  // Non-Blocking Parser that detaches and returns the PID.
  int get_pid(const int& fd, const int& pid) {close(fd); return pid;}

  // Blocking Parser that splits output into a vector based on newlines.
  std::vector<std::string> vectorize(const int& fd, const int& pid) {return fd_splitter<std::vector<std::string>, '\n'>(fd, pid);}

  // Blocking Parser that splits output into a set based on spaces.
  std::set<std::string> setorize(const int& fd, const int& pid) {return fd_splitter<std::set<std::string>, ' '>(fd, pid);}

  // Blocking Parser that dumps output into a single string.
  std::string dump(const int& fd, const int& pid) {
    std::string result;
    std::array<char, buffer_size> buffer;
    int bytes = read(fd, buffer.data(), buffer_size);
    while (bytes > 0) {
      result.reserve(bytes);
      result.append(buffer.data(), bytes);
      bytes = read(fd, buffer.data(), buffer_size);
    }
    close(fd);
    return result;
  }

  // Blocking Parser that dumps the first line of output and returns immediately.
  std::string one_line(const int& fd, const int& pid) {
    std::string result;
    std::array<char, buffer_size> buffer = {0};
    int bytes = read(fd, buffer.data(), buffer_size), index = 0;
    while (bytes > 0 && index != std::string::npos)  {
      result.reserve(buffer_size);
      result.append(buffer.data(), bytes);
      bytes =  read(fd, buffer.data(), buffer_size);
      index = result.rfind('\n');
    }
    close(fd);
    if (pid > 0) kill(pid, SIGTERM);
    return result.substr(0, index);
  }
}
