BINARY=bin/listener
OBJECTS=obj/Main.o 				\
        obj/DebugThread.o
HEADERS=src/DebugThread.hpp		\
        src/Messages.hpp

CXX=g++
CXXFLAGS=-std=c++14 -Wall -Wextra -Werror -pedantic -O2

BLD=`tput bold`
RED=`tput setaf 1`
GRN=`tput setaf 2`
YLW=`tput setaf 3`
BLU=`tput setaf 5`
TEL=`tput setaf 6`
NRM=`tput sgr0`
NL=\n

.PHONY: all, clean

all: $(BINARY)

loc:
	@printf "%s[ Lines ]%s " "$(BLD)$(YLW)" $(NRM)
	@cat src/* | wc -l

$(BINARY): $(OBJECTS)
	@mkdir -p bin
	@printf "%s[Linking]%s $@$(NL)" "$(BLD)$(TEL)" $(NRM)
	@$(CXX) $(OBJECTS) -o $@ -lpsapi
	@printf "%s[ Strip ]%s $@$(NL)" "$(BLD)$(TEL)" $(NRM)
	@strip $@
	@printf "%s[Success] Build Succeeded!%s$(NL)" "$(BLD)$(GRN)" $(NRM)

obj/%.o: src/%.cpp $(HEADERS)
	@mkdir -p obj
	@printf "%s[Compile]%s $<$(NL)" "$(BLD)$(BLU)" $(NRM)
	@$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@printf "%s[ Clean ]%s bin$(NL)" "$(BLD)$(RED)" $(NRM)
	@rm -rf bin
	@printf "%s[ Clean ]%s obj$(NL)" "$(BLD)$(RED)" $(NRM)
	@rm -rf obj
