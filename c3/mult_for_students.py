import math, sys, time


def mnoz(dane):
	A = dane[0]
	X = dane[1]

	nrows = len(A)
	ncols = len(A[0])
	y = []
	for i in xrange(nrows):
		s = 0
		for c in range(0, ncols):
			s += A[i][c] * X[c][0]
			#time.sleep(0.1)

		y.append(s)

	return y

def read(fname):
	f = open(fname, "r")
	nr = int(f.readline())
	nc = int(f.readline())

	A = [[0] * nc for x in xrange(nr)]
	r = 0
	c = 0
	for i in range(0,nr*nc):
		A[r][c] = float(f.readline())
		c += 1
		if c == nc:
			c = 0
			r += 1

	return A


ncpus = int(sys.argv[1]) if len(sys.argv) > 1 else 2
fnameA = sys.argv[2] if len(sys.argv) > 2 else "A.dat"
fnameX = sys.argv[3] if len(sys.argv) > 3 else "X.dat"


A = read(fnameA)
X = read(fnameX)



print "Wynik y= ", mnoz([A,X])
