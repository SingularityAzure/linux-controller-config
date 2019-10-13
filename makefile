APP = "bin/Controller Config"
DEPS = src/main.cpp

.PHONY: build run test

build: $(DEPS)
	mkdir -p bin
	g++ src/main.cpp `wx-config --cxxflags --libs` -o $(APP)

run:
	$(APP)

test: build
	$(APP)
