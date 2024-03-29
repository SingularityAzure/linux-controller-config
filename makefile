SRCDIR = src
BINDIR = bin
OBJDIR = obj

APP = controller-config
_DEPS = joysticks.h joystick_maps.h
DEPS = $(patsubst %,$(SRCDIR)/%,$(_DEPS)) makefile
_OBJS = main.o
OBJS = $(patsubst %,$(OBJDIR)/Release/%,$(_OBJS))
OBJS_D = $(patsubst %,$(OBJDIR)/Debug/%,$(_OBJS))

_OBJS_C = joysticks.o joystick_maps.o
OBJS_C = $(patsubst %,$(OBJDIR)/Release/c/%,$(_OBJS_C))
OBJS_C_D = $(patsubst %,$(OBJDIR)/Debug/c/%,$(_OBJS_C))

$(OBJDIR)/Release/%.o: $(SRCDIR)/%.cpp $(DEPS)
	@mkdir -p $(@D)
	g++ -c -o $@ $< `wx-config --cxxflags` -g -O2 -DNDEBUG

$(OBJDIR)/Debug/%.o: $(SRCDIR)/%.cpp $(DEPS)
	@mkdir -p $(@D)
	g++ -c -o $@ $< `wx-config --cxxflags` -g -rdynamic

$(OBJDIR)/Release/c/%.o: $(SRCDIR)/%.c $(DEPS)
	@mkdir -p $(@D)
	gcc -c -o $@ $< -g -O2 -DNDEBUG

$(OBJDIR)/Debug/c/%.o: $(SRCDIR)/%.c $(DEPS)
	@mkdir -p $(@D)
	gcc -c -o $@ $< -g -rdynamic

.PHONY: build builddebug run rundebug test testdebug clean install uninstall

build: $(OBJS) $(OBJS_C)
	@mkdir -p $(BINDIR)
	g++ $(OBJS) $(OBJS_C) `wx-config --libs` -o $(BINDIR)/$(APP)

builddebug: $(OBJS_D) $(OBJS_C_D)
	@mkdir -p $(BINDIR)
	g++ $(OBJS_D) $(OBJS_C_D) `wx-config --libs` -o $(BINDIR)/$(APP)_Debug

run:
	$(BINDIR)/$(APP)

rundebug:
	$(BINDIR)/$(APP)_Debug

test: build
	$(BINDIR)/$(APP)

testdebug: builddebug
	$(BINDIR)/$(APP)_Debug

clean:
	rm -f -R bin obj

install:
	cp $(BINDIR)/$(APP) /usr/bin/$(APP)
	cp controller-config.desktop /usr/share/applications/controller-config.desktop
	mkdir -p /usr/share/controller-config
	cp images/Icon.svg /usr/share/controller-config/Icon.svg

uninstall:
	rm /usr/bin/$(APP)
	rm /usr/share/applications/controller-config.desktop
	rm -r /usr/share/controller-config
