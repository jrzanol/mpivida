# Name of your MPI program
PROGRAM = mpivida

# Your MPI C compiler
MPICC = mpicc

#DO NOT CHANGE
RESULT = result.txt

# The program file. Note: If you have multiple source files, add them here
SRCS = mpivida.c

# Rules
all: $(PROGRAM)

$(PROGRAM): $(SRCS)
		$(MPICC) -o $(PROGRAM) $(SRCS)

run: $(PROGRAM)
		mpirun -np 4 -hosts worker1,worker2 ./$(PROGRAM) >> $(RESULT)
clean:
		rm -f $(PROGRAM)
