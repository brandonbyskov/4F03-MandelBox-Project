# to compile with pgcc 
# CXX=pgCC
# CC=pgcc
# to compile on Sharcnet with intel compiler
# CXX=icc -O3 -fopenmp
# CC =icc -O3 -fopenmp
# to compile with gcc
CXX=g++
CC =gcc

CFLAGS= -O3 -funroll-loops -fopenmp
CXXFLAGS= -O3 -funroll-loops -fopenmp

PROGRAM_NAME= mandelbox

$(PROGRAM_NAME): main.o print.o timing.o savebmp.o getparams.o 3d.o getcolor.o distance_est.o mandelboxde.o raymarching.o renderer.o init3D.o
	$(CXX) -o $@ $^ $(CFLAGS) $(LDFLAGS)
	#rm *.o

run-serial: $(PROGRAM_NAME)
	export OMP_NUM_THREADS=1
	./$(PROGRAM_NAME)$(EXEXT) params/params%04d.dat images/f%04d.bmp 0 7199 3840 2160

run-parallel: $(PROGRAM_NAME)
	export OMP_NUM_THREADS=32
	./$(PROGRAM_NAME)$(EXEXT) params/params%04d.dat images/f%04d.bmp 0 7199 3840 2160

clean:
	rm *.o $(PROGRAM_NAME)$(EXEEXT) *~
