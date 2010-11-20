CC=g++
#FLAGS=-Wall
INCLUDES=src

tests: src/test.h src/vector.h test/vector.cpp
	${CC} ${FLAGS} -I${INCLUDES} test/vector.cpp -o test-vector
	./test-vector
	