#include <vector>
#include <set>
#include <string>
#include <functional>
#include <iostream>

/**
 * @brief The namespace containing operations related to sets and vectors, primarily splitting.
 */
namespace container {

  /**
   * @brief A compatability overload to check if a given character is contained in a single or multiple delimeters.
   * @param v: The character to check.
   * @param d: The single delimiter to check.
   * @returns: Whether v is equal to d.
   */
  inline bool contains(const char& v, const char& d) {return v == d;}

  /**
   * @brief A compatability overload to check if a given character is contained in a single or multiple delimeters.
   * @param v: The character to check.
   * @param d: The collection of delimiters to check against
   * @returns: Whether v is contained within d.
   */
  inline bool contains(const char& v, const std::string_view& d) {return d.contains(v);}

  /**
   * @brief A compatability overload to emplace a value into either a vector or set.
   * @param working: The set to emplace into.
   * @param val: The value to emplace.
   * @note This performs working.emplace(val).
   */
  inline void emplace(std::set<std::string>& working, const std::string_view& val) {working.emplace(val);}

  /**
   * @brief A compatability overload to emplace a value into either a vector or set.
   * @param working: The vector to emplace into.
   * @param val: The value to emplace.
   * @note This performs working.emplace_back(val).
   */
  inline void emplace(std::vector<std::string>& working, const std::string_view& val) {working.emplace_back(val);}

  /**
   * @brief Split a string on a delimieter(s)
   * @tparam C: The accumulator container type, either a vector or set.
   * @tparam T: The delimiter type, can either be a single character, or a string of characters.
   * @param working: The accumulator.
   * @param str: The string to split.
   * @param delim: The delimiter to split on
   * @param escape: Ignore delimiters bound in quotes.
   */
  template <typename C, typename T> void split(C& working, const std::string_view& str, const T& delim, const bool& escape = false) {

    // Ignore our delim if its contained within quotes.
    bool wrapped = false;
    for (size_t r_bound = 0, l_bound = 0; r_bound <= str.length(); ++r_bound) {
      if (!wrapped && (r_bound == str.length() || contains(str[r_bound], delim))) {
        if (r_bound != l_bound) emplace(working, std::string_view(&str[l_bound], r_bound - l_bound));
        l_bound = r_bound + 1;
      }

      // If we hit a quote, we ignore the delimeters until we reach the match.
      // We also skip over the quote itself when emplacing.
      else if (escape && str[r_bound] == '\'') {
        if (r_bound == l_bound)
          ++l_bound;
        else if (contains(str[r_bound + 1], delim)) {
          emplace(working, std::string_view(&str[l_bound], r_bound - l_bound));
          l_bound = r_bound + 1;
        }
        wrapped ^= 1;
      }
    }
  }


  /**
   * @brief Compose a return value from an accumulator function.
   * @tparam T The return type (IE the accumulator type).
   * @tparam A The function to wrap.
   * @tparam ...Args Additional arguments to the function.
   * @param fun: The function.
   * @param args: Additional arguments.
   * @returns The accumulated results from the function.
   */
  template <class T, class A, class ...Args> T init(A&& fun, Args&&... args) {
    T ret;
    fun(ret, std::forward<Args>(args)...);
    return ret;
  }
}


/**
 * @brief The namespace pertaining to file handling and parsing.
 */
namespace file {

  /**
   * @brief Parse a file
   * @tparam R: The return type.
   * @param path: The path to the file.
   * @param parser: The parser to use on the file.
   * @returns The parsed contents.
   */
  template <class R> R parse(const std::filesystem::path& path, const std::function<R(const int&, const int&)>& parser) {
    return parser(open(path.c_str(), O_RDONLY), -1);
  }
}


/**
 * @brief The namespace pertaining to program execution and output parsing.
 */
namespace exec {

  /// Policy for the executor, describing what should be captured.
  typedef enum {
    NONE,   ///< Do no capture any output.
    STDOUT, ///< Capture STDOUT
    STDERR, ///< Capture STDERR
    ALL     ///< Capture both STDOUT and STDERR into one stream.
  } capture;

  /// Arguments passed to the executor.
  typedef struct options {
    const capture& cap = NONE;    ///< The capture policy.
    const std::string& in = "";   ///< Any STDIN to be fed into the child process.
    const bool& verbose = false;  ///< Whether to be verbose (IE printing the command to STDOUT).
  } options;

  /**
   * @brief Zip a container.
   * @param cmd: The container to zip.
   * @param verbose: Whether to print the zipped the contents.
   * @returns The zipped contents.
   * @note This is used to convert a initializer list or vector into a format execve can use.
   */
  template <class T = std::initializer_list<std::string_view>> std::vector<const char*> zip(const T& cmd, const bool& verbose = false) {
    std::vector<const char*> argv; argv.reserve(cmd.size() + 1);
    if (verbose) {
      std::cout << "EXEC: ";
       for (const std::string_view& v : cmd) {
         std::cout << v << ' ';
         argv.emplace_back(v.data());
       }
       std::cout << std::endl;
    }
    else for (const std::string_view& v : cmd) argv.emplace_back(v.data());
    argv.emplace_back(nullptr);
    return argv;
  }

  /**
   * @brief A Blocking Parser that waits for the PID to exit.
   * @param fd: The FD on the attached pipe.
   * @param pid: The PID of the process.
   * @warning This parser is blocking.
   */
  void wait_for(const int& fd, const int& pid = -1);

  /**
   * @brief A Non-Blocking Parser that returns the PID.
   * @param fd: The FD on the attached pipe.
   * @param pid: The PID of the process.
   * @returns: The PID
   */
  int get_pid(const int& fd, const int& pid = -1);

  /**
   * @brief A Blocking Parser that splits the output into a vector
   * @param fd: The FD on the attached pipe.
   * @param pid: The PID of the process.
   * @returns The output, split on newlines.
   * @note If you need to split on something other than '\n', use fd_splitter
   * @warning This parser is blocking.
   */
  std::vector<std::string> vectorize(const int& fd, const int& pid = -1);

  /**
   * @brief A Blocking Parser that splits the output into a set
   * @param fd: The FD on the attached pipe.
   * @param pid: The PID of the process.
   * @returns The output, split on spaces.
   * @note If you need to split on something other than ' ', use fd_splitter
   * @warning This parser is blocking.
   */
  std::set<std::string> setorize(const int& fd, const int& pid = -1);

  /**
   * @brief A Blocking Parser that returns the output in a single string.
   * @param fd: The FD on the attached pipe.
   * @param pid: The PID of the process.
   * @returns: The output
   * @warning This parser is blocking.
   */
  std::string dump(const int& fd, const int& pid = -1);

  /**
   * @brief A Blocking Parser that returns the first line of output.
   * @param fd: The FD on the attached pipe.
   * @param pid: The PID of the process.
   * @returns: The first line.
   * @warning This parser is blocking.
   */
  std::string one_line(const int& fd, const int& pid = -1);

  /**
   * @brief A Blocking Parser that returns the first count of the output.
   * @tparam count: The amount of characters to return.
   * @param fd: The FD on the attached pipe.
   * @param pid: The PID of the process.
   * @returns: The output.
   * @warning This parser is blocking.
   */
  template <uint_fast8_t count> std::string head(const int& fd, const int& pid) {
    std::array<char, count> buffer;
    if (read(fd, buffer.data(), count) != count)
      throw std::runtime_error("Failed to read file!");
    close(fd);
    if (pid > 0) kill(pid, SIGTERM);
    return std::string(buffer.data(), count);
  }

  /**
   * @brief Split the contents of an FD.
   * @tparam C: The container to return.
   * @tparam delim: The delimiter to use.
   * @param fd: The FD of the file/process.
   * @param pid: The PID of the process.
   * @returns The split output/contents.
   */
  template <class C, char delim, bool escape = false> C fd_splitter(const int& fd, const int& pid) {
    C result;
    auto contents = dump(fd, pid);
    container::split<C, char>(result, contents, delim, escape);
    return result;
  }

  /**
   * @brief Execute a command.
   * @tparam R: What to return from the child.
   * @tparam T: The container holding the command.
   * @param cmd: The command to run.
   * @param parser: The parser to use on the output.
   * @param opts: Additional options.
   * @returns The selected output from the parser.
   */
  template <class R = void, class T = std::initializer_list<std::string_view>> R execute(
      const T& cmd,
      const std::function<R(const int&, const int&)>& parser = [](const int& fd, const int& pid) {close(fd);},
      const options& opts = {}
  )  {

    // Open a pipe for communication from the child
    int output[2];
    if (pipe(output) == -1) throw std::runtime_error("Failed to setup pipe");

    // Open a pipe for comunication to the child, if needed.
    int input[2] = {-1, -1};
    if (!opts.in.empty()) {
      if (pipe(input) == -1) throw std::runtime_error("Failed to setup pipe");
    }

    auto pid = fork();
    if (pid < 0) throw std::runtime_error("Failed to fork");

    else if (pid == 0) {
      // Zip the command, open our standard files to the pipe.
      auto argv = zip(cmd, opts.verbose);

      close(output[0]);
      switch (opts.cap) {
        case STDOUT: dup2(output[1], STDOUT_FILENO); break;
        case STDERR: dup2(output[1], STDERR_FILENO); break;
        case ALL:  dup2(output[1], STDOUT_FILENO); dup2(output[1], STDERR_FILENO); break;
        default: break;
      }

      if (!opts.in.empty()) {
        close(input[1]);
        dup2(input[0], STDIN_FILENO);
        close(input[1]);
      }

      // Close the pipe and execute.
      close(output[1]);
      execvp(argv[0], const_cast<char* const*>(argv.data()));
    }

    // If there is STDIN, send it to the child.
    if (!opts.in.empty()) {
      close(input[0]);
      write(input[1], opts.in.c_str(), opts.in.length());
      close(input[1]);
    }

    // Close the pipe and parser the other side.
    close(output[1]);
    return parser(output[0], pid);
  }
}
