CC=gcc
CFLAGS=-I. -g
CPPFLAGS=-I. -g -std=c++11
DEPS =
OBJ = jagged.o main.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

%.o: %.cpp $(DEPS)
	g++ -c -o $@ $< $(CPPFLAGS)

default: $(OBJ)
	$(CC) -o jagged $^ $(CFLAGS)

clean:
	rm -f *.o *.out test/*.o

run: default
	./jagged

jagged_test: jagged.o test/test_main.o
	g++ jagged.o test/test_main.o -o jagged_test

test-create: jagged_test
	./jagged_test Jagged.Create

test-add: jagged_test
	./jagged_test Jagged.Add

test-remove: jagged_test
	./jagged_test Jagged.Remove

test-pack: jagged_test
	./jagged_test Jagged.Pack

test-unpack: jagged_test
	./jagged_test Jagged.Unpack

test-leak: jagged_test
	./jagged_test Jagged.Leak

test-leak-valgrind: jagged_test
	valgrind --leak-check=full --error-exitcode=1 \
		--show-leak-kinds=all \
		--errors-for-leak-kinds=definite,indirect,possible,reachable \
		./jagged_test

test: jagged_test
	./jagged_test
