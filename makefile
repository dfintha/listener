CLIBINARY=bin/listener-cli
GUIBINARY=bin/listener-gui

CLIOBJECTS=obj/cli/Main.o           \
           obj/cli/DebugThread.o    \
           obj/cli/Window.o
GUIOBJECTS=obj/gui/Main.o           \
           obj/gui/DebugThread.o    \
           obj/gui/Window.o

HEADERS=src/DebugThread.hpp         \
        src/Messages.hpp            \
        src/Window.hpp

PACKAGE=bin/listener.zip

CXX=g++
CXXFLAGS=-std=c++14 -Wall -Wextra -Werror -pedantic -O2

MT="/cygdrive/c/Program Files (x86)/Windows Kits/10/bin/x86/mt.exe"
MANIFEST="listener-gui.exe.manifest"
WINGUIBIN="bin\\listener-gui.exe"

BLD=`tput bold`
RED=`tput setaf 1`
GRN=`tput setaf 2`
YLW=`tput setaf 3`
BLU=`tput setaf 5`
TEL=`tput setaf 6`
NRM=`tput sgr0`
NL=\n

.PHONY: all, clean

all: cli gui
	@printf "%s[Success] All Builds Succeeded!%s$(NL)" "$(BLD)$(GRN)" $(NRM)

deploy: $(PACKAGE)
	@printf "%s[Success] Deployment Succeeded!%s$(NL)" "$(BLD)$(GRN)" $(NRM)

$(PACKAGE): cli gui
	@printf "%s[  ZIP  ] Zipping...%s$(NL)" "$(BLD)$(YLW)" $(NRM)
	@mkdir -p tmp
	@cp `find -L -O3 /usr -name cygwin1.dll` tmp/
	@cp `find -L -O3 /usr -name cyggcc_s-seh-1.dll` tmp/
	@cp `find -L -O3 /usr -name cygstdc++-6.dll` tmp/
	@cp $(CLIBINARY) tmp/
	@cp $(GUIBINARY) tmp/
	@cp $(MANIFEST) tmp/
	@cd tmp && zip ../bin/listener-current.zip ./* > /dev/null 2>&1
	@rm -rf tmp

gui: $(GUIBINARY)
	@printf "%s[Success] GUI Build Succeeded!%s$(NL)" "$(BLD)$(GRN)" $(NRM)

cli: $(CLIBINARY)
	@printf "%s[Success] CLI Build Succeeded!%s$(NL)" "$(BLD)$(GRN)" $(NRM)

loc:
	@printf "%s[ Lines ]%s " "$(BLD)$(YLW)" $(NRM)
	@cat src/* | wc -l

$(CLIBINARY): $(CLIOBJECTS)
	@mkdir -p bin
	@printf "%s[Linking]%s $@$(NL)" "$(BLD)$(TEL)" $(NRM)
	@$(CXX) $(CLIOBJECTS) -o $@ -lpsapi
	@printf "%s[ Strip ]%s $@$(NL)" "$(BLD)$(TEL)" $(NRM)
	@strip $@

$(GUIBINARY): $(GUIOBJECTS)
	@mkdir -p bin
	@printf "%s[Linking]%s $@$(NL)" "$(BLD)$(TEL)" $(NRM)
	@$(CXX) $(GUIOBJECTS) -o $@ -lpsapi -lwinmm -lgdi32 -Wl,--subsystem,windows
	@printf "%s[ MFest ]%s $@$(NL)" "$(BLD)$(TEL)" $(NRM)
	@$(MT) -nologo -manifest $(MANIFEST) -outputresource:$(WINGUIBIN)

obj/cli/%.o: src/%.cpp $(HEADERS)
	@mkdir -p obj/cli
	@printf "%s[Compile]%s $<$(NL)" "$(BLD)$(BLU)" $(NRM)
	@$(CXX) $(CXXFLAGS) -c $< -o $@

obj/gui/%.o: src/%.cpp $(HEADERS)
	@mkdir -p obj/gui
	@printf "%s[Compile]%s $<$(NL)" "$(BLD)$(BLU)" $(NRM)
	@$(CXX) $(CXXFLAGS) -DLISTENER_WINDOWED -c $< -o $@

clean:
	@printf "%s[ Clean ]%s bin$(NL)" "$(BLD)$(RED)" $(NRM)
	@rm -rf bin
	@printf "%s[ Clean ]%s obj$(NL)" "$(BLD)$(RED)" $(NRM)
	@rm -rf obj

