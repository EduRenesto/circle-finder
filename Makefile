CXX := g++
CXXFLAGS := -std=c++1z
LDFLAGS := -lopencv_imgcodecs -lopencv_highgui -lopencv_imgproc -lopencv_core

all: obj/main.o
	${CXX} obj/main.o ${LDFLAGS} -o circleFinder

obj/main.o: src/main.cc
	${CXX} ${CXXFLAGS} -c src/main.cc -o obj/main.o
