package temp_calc_cpp;
//
//###################################################################
//                   Class TempCalcJava
//###################################################################
//
//   
//  Creator               : Chris Anderson
//  (C) UCLA 1997
//
//###################################################################
//
public class TempCalcJava implements Runnable
{
    //
//  Problem parameters
//
    double diffusivity;
    double a; double b;
    double c; double d;
    //
//  Runtime parameters
//
    int  m; int n;

    int       nSteps;
    int       nOut;
    double    dt;

    double [] Tarray;
    double [] workArray;
    //
//  Graphics object (for output)
//
    ColorContourPlot contourPlot;

    public TempCalcJava()
    {
        diffusivity = 0.0;

        a = 0.0;  b= 0.0;
        c = 0.0;  d= 0.0;

        n           = 0;
        nSteps      = 0;
        nOut        = 0;
        dt          = 0.0;
        Tarray      = null;
        workArray   = null;
        contourPlot = null;
    }

    public void run()
    {
//
//  Allocate space for solution and work arrays
//
        double[] Tarray     = new double[m*n];
        double[] workArray  = new double[m*n];
//
//
        initializeTemperature(Tarray,m,n,a,b,c,d);
        if(contourPlot != null) contourPlot.setData(Tarray,m,n);

        int i;
        for(i = 1; i <= nSteps; i++)
        {
            evolveTemperature(Tarray,m,n,a,b,c,d,dt,diffusivity,workArray);
            if((i%nOut)== 0)
            {
                if(contourPlot != null) contourPlot.setData(Tarray,m,n);
                System.out.println("Step : " + i);
            }
        }
    }

    public static void main(String [] args)
    {
//
//  create a tempCalcJava object instance
//
        TempCalcJava temperatureRun = new TempCalcJava();
//
//  Set Problem parameters
//
        temperatureRun.diffusivity = 0.1;
        temperatureRun.a           = 0.0;
        temperatureRun.b           = 1.0;
        temperatureRun.c           = 0.0;
        temperatureRun.d           = 1.0;
//
//  Set Runtime parameters
//
        temperatureRun.m            = 10;
        temperatureRun.n            = 20;
        temperatureRun.nSteps       = 100;
        temperatureRun.nOut         = 10;
        temperatureRun.dt           = 0.01;

        temperatureRun.run();
    }
    //
//  native method declarations
//
    public native void initializeTemperature(double[] Tarray, int m, int n,
                                             double a, double b, double c, double d);

    public native void evolveTemperature(double[] Tarray, int m, int n,
                                         double a, double b, double c, double d, double dt, double diffusivity,
                                         double[] workArray);
    //
// Load DLL (or shared library) which contains implementation of native methods
//
    static
    {
        System.loadLibrary("TempCalcJava");
    }

}