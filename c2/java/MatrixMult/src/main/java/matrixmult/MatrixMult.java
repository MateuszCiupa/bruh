package matrixmult;

import java.io.*;
import static java.lang.Math.sqrt;
import java.util.Scanner;
import java.util.logging.Level;
import java.util.logging.Logger;

public class MatrixMult {
    public static int x, y, B_pionowo = 0, B_poziomo = 0, A_pionowo = 0, A_poziomo = 0, sciana;
    public static float sum, frobenius;

    public static void main(String[] args) throws FileNotFoundException {
        MatrixMult mm = new MatrixMult();
        mm.begin(args);
    }

    protected void begin(String[] args) throws FileNotFoundException {
        Matrix A, B;
        A = read("A.txt");
        B = read("B.txt");
        int x, y, numberOfMnozenia, obliczeniaNaWatek, xc = 0, yc = 0;
        if (A.rows() == B.cols()) {
            x = B.rows();
            y = A.cols();
            numberOfMnozenia = x * y;
            B_pionowo = 1;
            A_poziomo = 1;
            sciana = A.rows();
        } else {
            x = A.rows();
            y = B.cols();
            numberOfMnozenia = x * y;
            B_poziomo = 1;
            A_pionowo = 1;
            sciana = B.rows();
        }
        Matrix C = new Matrix(x, y);
        System.out.println("Wczytalem A:");
        print(A);

        System.out.println("\nWczytalem B:");
        print(B);

        int threads = 5;
        obliczeniaNaWatek = numberOfMnozenia / threads;
        
        for (int i = 0; i < threads; i++) {
            Runnable r = new MyRunnable(xc, yc, obliczeniaNaWatek, A, B, C);
            new Thread(r).start();
            yc = yc + (xc + obliczeniaNaWatek) / y;
            xc = (xc + obliczeniaNaWatek) % y;
        }
        try {
            Thread.sleep(100);
        } catch (InterruptedException ex) {
            Logger.getLogger(MatrixMult.class.getName()).log(Level.SEVERE, null, ex);
        }
        System.out.println("");
        System.out.println("A * B:");
        print(C);
        System.out.println("Suma elementów: " + sum);
        System.out.println("Norma Frobeniusa: " + sqrt(frobenius));
        
    }

    protected Matrix read(String fname) throws FileNotFoundException {
        File f = new File(fname);
        Scanner scanner = new Scanner(f);

        int rows = scanner.nextInt();
        int cols = scanner.nextInt();
        Matrix res = new Matrix(rows, cols);

        for (int r = 0; r < res.rows(); r++) {
            for (int c = 0; c < res.cols(); c++) {
                res.set(r, c, scanner.nextFloat());
            }
        }
        return res;
    }

    protected void print(Matrix m) {
        System.out.println("[");
        for (int r = 0; r < m.rows(); r++) {

            for (int c = 0; c < m.cols(); c++) {
                System.out.print(m.get(r, c));
                System.out.print(" ");
            }

            System.out.println("");
        }
        System.out.println("]");
    }

    public class MyRunnable implements Runnable {

        int xc, yc, ile;
        Matrix A, B, C;

        public MyRunnable(int xc, int yc, int ile, Matrix A, Matrix B, Matrix C) {
            this.xc = xc;
            this.yc = yc;
            this.ile = ile;
            this.A = A;
            this.B = B;
            this.C = C;
        }

        @Override
        public void run() {
            float s;
            int xxc = yc, yyc = xc;
            for (int i = 0; i < ile; i++) {
                s = 0;
                if (B_pionowo == 1 && A_poziomo == 1) {
                    for (int j = 0; j < sciana; j++) {
                        s += A.get(j, yyc) * B.get(xxc, j);
                    }
                    C.set(xxc, yyc, s);
                    frobenius = frobenius + s*s;
                    sum = sum + s;
                    yyc++;
                    if (yyc >= sciana - 1) {
                        xxc++;
                        yyc = 0;
                    }
                } else if (B_poziomo == 1 && A_pionowo == 1) {
                    for (int j = 0; j < sciana; j++) {
                        s += A.get(xxc, j) * B.get(j, yyc);
                    }
                    C.set(xxc, yyc, s);
                    frobenius = frobenius + s*s;
                    sum = sum + s;
                    yyc++;
                    if (yyc >= sciana - 1) {
                        xxc++;
                        yyc = 0;
                    }
                }
            }
        }
    }

    public class Matrix {

        private int ncols;
        private int nrows;
        private float _data[];

        public Matrix(int r, int c) {
            this.ncols = c;
            this.nrows = r;
            _data = new float[c * r];
        }

        public float get(int r, int c) {
            return _data[r * ncols + c];
        }

        public void set(int r, int c, float v) {
            _data[r * ncols + c] = v;
        }

        public int rows() {
            return nrows;
        }

        public int cols() {
            return ncols;
        }
    }

}
