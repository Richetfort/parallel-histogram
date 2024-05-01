#WARNING : MAKE MIGHT BE SILENCED BY ALIAS COMMAND alias make = 'make -s'

SDIR = src
ODIR = obj
BDIR = bin

COMP = g++
OPTS = -g -O3 -std=c++11 -fopenmp
WOPT = -Wall -Wno-class-memaccess
INCS = -I include
LIBS =

PROGRAMS = $(shell grep -l 'int main(.*)' $(SDIR)/*.cpp)
SOURCES = $(filter-out  $(PROGRAMS),$(wildcard $(SDIR)/*.cpp))
OBJT = $(patsubst $(SDIR)/%, $(ODIR)/%, $(SOURCES:.cpp=.o))
EXET = $(patsubst $(SDIR)/%, $(BDIR)/%, $(PROGRAMS:.cpp=.exe))

CXX       = $(COMP)
CXXFLAGS  = $(OPTS) $(WOPT) $(INCS)

prog : $(EXET)

run : $(BDIR)/main.exe
	$(BDIR)/main.exe

clean :
	rm -f $(ODIR)/*.o $(BDIR)/*.exe

$(ODIR) :
	mkdir -p $(ODIR)

$(OBJT) : | $(ODIR)

$(ODIR)/%.o : $(SDIR)/%.cpp Makefile
	@echo [OBJ] $(CXX) $(OPTS) -c -o $@ $<
	@$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BDIR) :
	mkdir -p $(BDIR)

$(EXET) : | $(BDIR)

$(BDIR)/%.exe : $(SDIR)/%.cpp $(OBJT)
	@echo [EXE] $(CXX) $(OPTS) -o $@ $<
	@${CXX} ${CXXFLAGS}  -o $@ $^ ${LIBS}
