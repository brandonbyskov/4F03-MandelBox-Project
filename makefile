# to compile with pgcc 
# CXX=pgCC
# CC=pgcc
# to compile on Sharcnet with intel compiler
# CXX=icc
# CC =icc
# to compile with gcc
CXX=g++
CC =gcc

CFLAGS= -O3 -fopenmp
CXXFLAGS= -O3 -fopenmp

PROGRAM_NAME= mandelbox

$(PROGRAM_NAME): main.o print.o timing.o savebmp.o getparams.o 3d.o getcolor.o distance_est.o mandelboxde.o raymarching.o renderer.o init3D.o
	$(CXX) -o $@ $^ $(CFLAGS) $(LDFLAGS)
	rm *.o

run-serial: $(PROGRAM_NAME)
	./$(PROGRAM_NAME)$(EXEXT) params.dat

clean:
	rm *.o $(PROGRAM_NAME)$(EXEEXT) *~
