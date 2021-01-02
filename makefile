CC = gcc
MainFile = main
OutDir = ./Compiled

SharedLibraryArguments = -c -g

ExecutableArguments = -g -lmta_rand -lmta_crypt -lpthread

MainSource := $(MainFile).c
MainDestination = $(OutDir)/LinuxThreads.out

HelperSources := $(subst ./,,$(shell find . -maxdepth 1 -name "*.c" ! -name "$(MainSource)"))
HelperTargets := $(patsubst %.c,$(OutDir)/%.so,$(HelperSources))



$(OutDir)/%.so : %.c
	mkdir -p $(OutDir)
	$(CC) $(SharedLibraryArguments) $< -o $@

$(MainDestination) : $(MainSource) $(HelperTargets)
	$(CC) -o $(MainDestination) $(MainSource) $(HelperTargets) $(ExecutableArguments)

all: $(MainDestination) $(HelperTargets)

InstallDeps:
	cd /tmp
	wget https://github.com/gavrielk/LinuxCourseCodePub/raw/master/mta_crypt_lib/mta-utils-dev.deb
	sudo dpkg --install mta-utils-dev.deb

.PHONY: clean
clean:
	rm -rf $(OutDir)