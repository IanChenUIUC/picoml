CXX        ?= g++
CXXFLAGS   ?= -std=c++17 -Wall -Wextra
BISON      ?= bison
BISONFLAGS ?= -Wall -Wcounterexamples -v
FLEX       ?= flex

PROGS     := lexer parser
COMMON    := lex.yy.cpp picoml.cpp print.cpp
GENERATED := picoml.tab.cpp picoml.tab.h picoml.output lex.yy.cpp

all: $(PROGS)

lexer: lex_main.cpp $(COMMON) picoml.h picoml.tab.h
	$(CXX) $(CXXFLAGS) -o $@ lex_main.cpp $(COMMON) $(LDLIBS)

parser: parse_main.cpp picoml.tab.cpp $(COMMON) picoml.h picoml.tab.h
	$(CXX) $(CXXFLAGS) -o $@ parse_main.cpp picoml.tab.cpp $(COMMON) $(LDLIBS)

picoml.tab.cpp picoml.tab.h picoml.output &: picoml.y
	$(BISON) $(BISONFLAGS) -o picoml.tab.cpp --header=picoml.tab.h picoml.y

lex.yy.cpp: picoml.flex picoml.tab.h
	$(FLEX) -o lex.yy.cpp picoml.flex

clean:
	rm -f $(PROGS) $(GENERATED)

.PHONY: all clean
