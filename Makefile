REQUIRED = -std=c++23 -fuse-ld=lld

libexec: src/exec.cpp
	clang++ $(REQUIRED) -shared $^ -o libexec.so

docs:
	doxygen config
