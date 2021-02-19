CC = gcc
LibsDir = Libs
OutDir = ./Compiled

SharedLibraryArguments = -c -g

ExecutableArguments = -g -lmta_rand -lmta_crypt -lrt

CommonSource := $(CommonFile).c
CommonDestination = $(OutDir)/Common.o

HelperSources := $(subst ./$(LibsDir)/,,$(shell find ./$(LibsDir) -maxdepth 1 -name "*.c"))
HelperTargets := $(patsubst %.c,$(OutDir)/%.o,$(HelperSources))

MainSources := $(subst ./,,$(shell find . -maxdepth 1 -name "*.c"))
MainTargets := $(patsubst %.c,$(OutDir)/%.out,$(MainSources))


$(OutDir)/%.o : $(LibsDir)/%.c
	mkdir -p $(OutDir)
	$(CC) $(SharedLibraryArguments) $< -o $@

$(OutDir)/%.out : %.c $(HelperTargets)
	$(CC) -o $@ $< $(HelperTargets) $(ExecutableArguments)

all: $(MainTargets)

InstallDeps:
	cd /tmp
	wget https://github.com/gavrielk/LinuxCourseCodePub/raw/master/mta_crypt_lib/mta-utils-dev.deb
	sudo dpkg --install mta-utils-dev.deb

.PHONY: clean
clean:
	rm -rf $(OutDir)