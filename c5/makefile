EXECS=integrate
MPICC?=mpicc

all: ${EXECS}

integrate: integrate.c
	${MPICC} -o ${EXECS} integrate.c -l m

rebuild: clean integrate

clean:
	rm ${EXECS}
