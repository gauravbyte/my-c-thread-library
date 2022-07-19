


# Includes the project configurations
include project.conf

#
# Validating project variables defined in project.conf
#
ifndef PROJECT_NAME
$(error Missing PROJECT_NAME. Put variables at project.conf file)
endif
ifndef BINARY
$(error Missing BINARY. Put variables at project.conf file)
endif
ifndef PROJECT_PATH
$(error Missing PROJECT_PATH. Put variables at project.conf file)
endif


# Gets the Operating system name
OS := $(shell uname -s)

# Default shell
SHELL := bash
CUR_DIR = $(CURDIR)


plot:
	gnuplot src/plot.gnu


# Source code directory structure
BINDIR := bin
SRCDIR := src
LOGDIR := log
LIBDIR := lib
TESTDIR := test


# Source code file extension
SRCEXT := c


# Defines the C Compiler
CC := gcc 


# Defines the language standards for GCC
#STD := -std=c99 # See man gcc for more options

# Protection for stack-smashing attack
#STACK := -fstack-protector-all -Wstack-protector

# Specifies to GCC the required warnings
#WARNS := -Wall -Wextra -pedantic # -pedantic warns on language standards

# Flags for compiling
#CFLAGS := -O0 $(STD) $(STACK) $(WARNS)

# Debug options
#DEBUG := -g3 -DDEBUG=1

# Dependency libraries
LIBS := # -lm  -I some/path/to/library









# %.o file names
NAMES := $(notdir $(basename $(wildcard $(SRCDIR)/*.$(SRCEXT))))
OBJECTS :=$(patsubst %,$(LIBDIR)/%.o,$(NAMES))


#
# COMPILATION RULES
#

default: run

# Help message
help:
	@echo "C Project Template"
	@echo
	@echo "Target rules:"
	@echo "    all      - Compiles and generates shared library files and .o file"
	@echo "    test(number)    - Compiles with cmocka and run tests binary file"
	@echo "    clean    - Clean the project by removing binaries"
	@echo "    help     - Prints a help message with target rules"

# Starts a new project using C project template
start:
	@echo "Creating project: $(PROJECT_NAME)"
	@mkdir -pv $(PROJECT_PATH)
	@echo "Copying files from template to new directory:"
	@cp -rvf ./* $(PROJECT_PATH)/
	@echo
	@echo "Go to $(PROJECT_PATH) and compile your project: make"
	@echo "Then execute it: bin/$(BINARY) --help"
	@echo "Happy hacking o/"


# Rule for link and generate the binary file
all: 
	gcc -Wall -fpic -c src/userthread.c -o lib/userthread.o
	gcc -o lib/libmythread.so lib/userthread.o -shared
	unset LD_LIBRARY_PATH
	export LD_LIBRARY_PATH=$(CUR_DIR)/lib:$$LD_LIBRARY_PATH
	echo $$LD_LIBRARY_PATH
	# gcc -o bin/test1 src/test1.c -lmythread
	gcc -o bin/test1 src/test1.c lib/userthread.o
	gcc -o bin/test2 src/test2.c lib/userthread.o
	gcc -o bin/test3 src/test3.c lib/userthread.o
	gcc -o bin/test4 src/test4.c lib/userthread.o
	gcc -o bin/test5 src/test5.c lib/userthread.o
	gcc -o bin/matrix src/matrix.c lib/userthread.o
	gcc -o bin/producer_consumer src/producer_consumer.c lib/userthread.o

run:
	bin/test1
	bin/test2
	bin/test3
	bin/test4
	bin/test5
	bin/matrix 
	bin/producer_consumer 

matrix:
	bin/matrix $(arg)

producer_consumer:
	bin/producer_consumer

test1:
	bin/test1

test2:
	bin/test2

test3:
	bin/test3

test4:
	bin/test3

test5:
	bin/test3

# Rule for object binaries compilation
$(LIBDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@echo -en "$(BROWN)CC $(END_COLOR)";
	$(CC) -c $^ -o $@ $(DEBUG) $(CFLAGS) $(LIBS)


# Rule for run valgrind tool
run: $(OBJECTS)
	$(CC) -o $(BINDIR)/$(BINARY) $+ $(DEBUG) $(CFLAGS) $(LIBS)
	./$(BINDIR)/$(BINARY)
	gnuplot src/plot.gnu
	


# Compile tests and run the test binary


# Rule for cleaning the project
clean:
	@rm -rvf $(BINDIR)/* $(LIBDIR)/* $(LOGDIR)/*;
