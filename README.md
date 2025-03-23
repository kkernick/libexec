# libexec

This repository contains a library/header providing three key functions split into separate namespaces:

1. The `exec` namespace contains the `executor` function, which runs programs in forked children, reducing the overhead of a `popen` call by not creating a shell environment, and supporting both communication *to* the child process via an explicit input string to be fed into STDIN, and communication *from* the child where STDIN, STDERR, both, or neither streams will be returned. The real power of `executor` is that parsing of output is done via *Parser* modules, which need only be functions that accept a FD integer to the output stream, PID integer for the child. The builtin parsers include:
	1. `wait_for`, which discards the output and merely blocks until the child has finished.
	2. `get_pid`, which returns the PID of the child in a non-blocking manner, discarding child output.
	3. `vectorize`, which blocks and splits the output on newlines into a `std::vector<std::string>`
	4. `setorize`, which blocks and splits the output on whitespace into a `std::set<std::vector>`
	5. `dump`, which blocks and returns the output as a single `std::string`
	6. `one_line`, which blocks and returns the first line of output.
	7. `head`, which returns a fixed amount of bytes from the output.
	8. `fd_splitter`, which returns either a set, or vector, that can be split on either a single character or collection of characters via a string.
2. The `file` namespace contains the `parse` function, which reads a file, but uses the same parsers as `exec::executor`, merely sending a `-1` for the PID.
3. The `container` namespace includes functions principally for `splitter`, which splits either a set or vector on either a single or multiple delimiters specified by a `char` or `std::string` respectively. It has been designed as an accumulator function, but an `init` function can return a value if such an accumulator does not exist.

The purpose of the library is to efficiently parse command output and files while accommodating the needs for various programs. It was originally created as part of another project, [SB](https://github.com/kkernick/sb), but has been broken off to be used in other capacities.

## Usage

You can either use this repository as a submodule, and merely add the `exec.cpp` file to your build system, or use the provided Makefile to compile a shared library that you can link against via `-lexec`. A PKGBUILD for Arch-based distributions can be used for the latter.

Documentation, both in man format and doxygen, is available in the `docs` folder.
