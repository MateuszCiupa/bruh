/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package matrixmult;

import matrixmult.MatrixMult.Matrix;

/**
 *
 * @author Krzysztof
 */
public class MyRunnable implements Runnable {
    int pocz, ile;
    Matrix A,B;
    public MyRunnable(int pocz,int ile,Matrix A,Matrix B) {
        this.pocz = pocz;
        this.ile = ile;
        this.A = A;
        this.B = B;
    }
    
    @Override
    public void run() {
        
    }
}
