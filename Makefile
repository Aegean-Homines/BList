PRG=gnu.exe
PRG2=gnu-32.exe
GCC=g++-5
GCCFLAGS=-Wall -Werror -Wextra -std=c++11 -pedantic -Wconversion -O -Wno-unused-result

OBJECTS0=PRNG.cpp
DRIVER0=driver.cpp

VALGRIND_OPTIONS=-q --leak-check=full
DIFF_OPTIONS=-y --strip-trailing-cr --suppress-common-lines -b

OSTYPE := $(shell uname)
ifeq ($(OSTYPE),Linux)
CYGWIN=
else
CYGWIN=-Wl,--enable-auto-import
endif

gcc0:
	$(GCC) -o $(PRG) $(CYGWIN) $(DRIVER0) $(OBJECTS0) $(GCCFLAGS)
gcc0_32:
	$(GCC) -o $(PRG2) $(CYGWIN) $(DRIVER0) $(OBJECTS0) $(GCCFLAGS) -m32
1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46:
	#echo "running test$@"
	#@echo "should run in less than 200 ms"
	@echo "should run in less than 200 ms"
	./$(PRG) $@ >studentout$@
	@echo "lines after the next are mismatches with master output -- see out$@"
	diff out$@ studentout$@ $(DIFF_OPTIONS)
clean : 
	rm *.exe student* difference*