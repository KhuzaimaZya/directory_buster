

all: serial mpi_parallel omp_parallel


serial: serial.cpp LoadWordlist.o
	g++ -o serial serial.cpp LoadWordlist.o -lcurl


mpi_parallel: mpi_parallel.cpp LoadWordlist.o
	mpic++ -o mpi_parallel mpi_parallel.cpp LoadWordlist.o -lcurl


omp_parallel: omp_parallel.cpp LoadWordlist.o
	mpic++ -o omp_parallel omp_parallel.cpp LoadWordlist.o -lcurl -fopenmp


LoadWordlist.o: LoadWordlist.cpp LoadWordlist.h
	g++ -c LoadWordlist.cpp


.PHONY: clean
clean:
	rm -f serial mpi_parallel omp_parallel LoadWordlist.o