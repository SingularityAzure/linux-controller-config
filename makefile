SRCDIR = src
BINDIR = bin
OBJDIR = obj

APP = Controller_Config
_DEPS =
DEPS = $(patsubst %,$(SRCDIR)/%,$(_DEPS)) makefile
_OBJS = main.o
OBJS = $(patsubst %,$(OBJDIR)/Release/%,$(_OBJS))
OBJS_D = $(patsubst %,$(OBJDIR)/Debug/%,$(_OBJS))

$(OBJDIR)/Release/%.o: $(SRCDIR)/%.cpp $(DEPS)
	@mkdir -p $(@D)
	g++ -c -o $@ $< `wx-config --cxxflags` -g -O2 -DNDEBUG

$(OBJDIR)/Debug/%.o: $(SRCDIR)/%.cpp $(DEPS)
	@mkdir -p $(@D)
	g++ -c -o $@ $< `wx-config --cxxflags` -g -rdynamic

.PHONY: build builddebug run rundebug test testdebug

build: $(OBJS)
	@mkdir -p $(BINDIR)
	g++ $(OBJS) `wx-config --libs` -o $(BINDIR)/$(APP)

builddebug: $(OBJS_D)
	@mkdir -p $(BINDIR)
	g++ $(OBJS_D) `wx-config --libs` -o $(BINDIR)/$(APP)_Debug

run:
	$(BINDIR)/$(APP)

rundebug:
	$(BINDIR)/$(APP)_Debug

test: build
	$(BINDIR)/$(APP)

testdebug: builddebug
	$(BINDIR)/$(APP)_Debug
