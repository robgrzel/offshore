package temp_calc_cpp;

import java.awt.*;
//
//###################################################################
//                   Class ColorContourPlot
//###################################################################
//
//
//  Creator               : Chris Anderson
//  (C) UCLA 1997
//
//###################################################################
//
public class ColorContourPlot extends Frame implements Runnable
{

    public ColorContourPlot()
    {
        //{{INIT_CONTROLS
        setLayout(null);
        addNotify();
        resize(insets().left + insets().right + 545,insets().top + insets().bottom + 346);
        BackButton = new java.awt.Button("<<");
        BackButton.reshape(insets().left + 40,insets().top + 288,50,29);
        add(BackButton);
        ForButton = new java.awt.Button(">>");
        ForButton.reshape(insets().left + 116,insets().top + 288,50,29);
        add(ForButton);
        FrameLabel = new java.awt.Label("Frame",Label.CENTER);
        FrameLabel.reshape(insets().left + 196,insets().top + 288,48,29);
        add(FrameLabel);
        FrameCountLabel = new java.awt.Label("",Label.CENTER);
        FrameCountLabel.reshape(insets().left + 260,insets().top + 288,56,29);
        add(FrameCountLabel);
        setTitle("Color Contour Plot");
        //}}

        //{{INIT_MENUS
        menuBar1 = new java.awt.MenuBar();

        menu1 = new java.awt.Menu("Options");
        menu1.add("Contour Resolution");
        menuBar1.add(menu1);
        setMenuBar(menuBar1);
        //$$ menuBar1.move(0,0);
        //}}


        show();

        captureMode     = true;
        javaVersion     = new String(getJavaVersion());

        Rectangle  R = bounds();
        contourCanvas = new ColorContourCanvas(this);
        int yoffset = insets().top + insets().bottom;
        int xoffset = insets().left + insets().right;

        if(!captureMode)
        {
            contourCanvas.reshape(insets().left + 5,insets().top + 5,
                    R.width  - xoffset - 10,R.height - yoffset - 10);
            BackButton.hide();
            ForButton.hide();
            FrameLabel.hide();
            FrameCountLabel.hide();
        }
        else
        {
            contourCanvas.reshape(insets().left + 5,insets().top + 5,
                    R.width  - xoffset - 10,R.height - yoffset - 60);
            if(javaVersion.equals("1.1")){yoffset = 5;}
            BackButton.reshape(insets().left + 40,R.height - yoffset - 35,50,29);
            ForButton.reshape(insets().left + 116,R.height - yoffset - 35,50,29);
            FrameLabel.reshape(insets().left + 196,R.height - yoffset - 35,48,29);
            FrameCountLabel.reshape(insets().left + 260,R.height - yoffset - 35,56,29);
        }

        add(contourCanvas);

        resDialog = new ContourResolutionDialog(this, this, false);
        resDialog.hide();

        contourData  = new java.util.Vector(10);
        currentFrame = 0;
        RunningAsThread = false;
    }

    //{{DECLARE_CONTROLS
    java.awt.Button BackButton;
    java.awt.Button ForButton;
    java.awt.Label FrameLabel;
    java.awt.Label FrameCountLabel;
    //}}

    //{{DECLARE_MENUS
    java.awt.MenuBar menuBar1;
    java.awt.Menu menu1;
    //}}

    public synchronized void show()
    {
        move(50, 50);
        super.show();
    }

    public void repaint()
    {
        Rectangle  R = bounds();
        int yoffset = insets().top + insets().bottom;
        int xoffset = insets().left + insets().right;


        if(!captureMode)
        {
            if(contourCanvas != null)
            {
                contourCanvas.reshape(insets().left + 5,insets().top + 5,
                        R.width  - xoffset - 10,R.height - yoffset - 10);
            }
        }
        else
        {
            if(contourCanvas != null)
            {
                contourCanvas.reshape(insets().left + 5,insets().top + 5,
                        R.width  - xoffset - 10,R.height - yoffset - 60);
                if(javaVersion.equals("1.1")){yoffset = 5;}
                BackButton.reshape(insets().left + 40,R.height - yoffset - 35,50,29);
                ForButton.reshape(insets().left + 116,R.height - yoffset - 35,50,29);
                FrameLabel.reshape(insets().left + 196,R.height - yoffset - 35,48,29);
                FrameCountLabel.reshape(insets().left + 260,R.height - yoffset - 35,56,29);

            }
        }
        super.repaint();
    }

    public boolean handleEvent(Event event)
    {
        if (event.id == Event.WINDOW_DESTROY)
        {
            if(!RunningAsThread)
            {
                hide();         // hide the Frame
                dispose();      // free the system resources
                System.exit(0); // close the application
                return true;
            }
            else
            {
                hide();
                return true;
            }
        }

        if (event.target == BackButton && event.id == Event.ACTION_EVENT) {
            BackButton_Clicked(event);
            return true;
        }
        if (event.target == ForButton && event.id == Event.ACTION_EVENT) {
            ForButton_Clicked(event);
            return true;
        }
        return super.handleEvent(event);
    }
    public boolean action(Event event, Object arg)
    {
        if (event.target instanceof MenuItem)
        {
            String label = (String) arg;
            if (label.equalsIgnoreCase("Contour Resolution"))
            {
                ContourResolution_Action(event);
                return true;
            }
        }
        return super.action(event, arg);
    }


    void ForButton_Clicked(Event event)
    {
        if(currentFrame < contourData.size())
        {
            currentFrame++;
            contourCanvas.setData((double[][])(contourData.elementAt(currentFrame-1)));
            FrameCountLabel.setText(new String(String.valueOf(currentFrame)));
        }
    }

    void BackButton_Clicked(Event event)
    {
        if(currentFrame > 1)
        {
            currentFrame--;
            contourCanvas.setData((double[][])(contourData.elementAt(currentFrame-1)));
            FrameCountLabel.setText(new String(String.valueOf(currentFrame)));
        }
    }

    void ContourResolution_Action(Event event)
    {
        resDialog.setResolution(contourCanvas.getResolution());
        resDialog.show();
    }

    public void dialogDismissed(Dialog d)
    {
        if(d == resDialog )
        {
            int panelCount =  resDialog.getResolution();
            contourCanvas.setResolution(panelCount);
            contourCanvas.repaint();
        }
    }

    public void setCaptureMode(boolean M)
    {
        captureMode = M;
        contourData.removeAllElements();
        currentFrame = 0;
    }

    public void reset()
    {
        contourData.removeAllElements();
        currentFrame = 0;
    }

    public void setDataRange(double dMin, double dMax)
    {
        contourCanvas.setDataRange(dMin, dMax);
    }

    public void setData(double [][] inputData)
    {
        if(captureMode)
        {
            contourData.addElement(inputData);
            if(contourData.size() == 1)
            {
                contourCanvas.setData(inputData);
                FrameCountLabel.setText("1");
                currentFrame = 1;
            }
        }
        else
        {
            contourCanvas.setData(inputData);
        }
    }

    public void setData(double [] inputData, int xDataCount, int yDataCount)
    {
        double [][] data = new double[xDataCount][yDataCount];
        int i; int j;
        for(i =0; i < xDataCount; i++)
        {
            for(j = 0; j < yDataCount; j++)
            {
                data[i][j] = inputData[j + i*yDataCount];
            }}
        setData(data);
    }



    public void run()
    {
        RunningAsThread = true;
        show();
    }

    public String getJavaVersion()
    {
        java.util.Properties P = System.getProperties();

        java.util.StringTokenizer Tok
                = new java.util.StringTokenizer(P.getProperty("java.version", null),".");

        String version;
        String p1, p2;
        if(Tok.countTokens() > 1)
        {
            p1 = Tok.nextToken();
            p2 = Tok.nextToken();
            version = p1 + "." + p2;
        }
        else
        {
            version = "1.0";
        }
        return version;
    }
    //
// program for testing purposes
//
    static public void main(String args[])
    {
        ColorContourPlot M = new ColorContourPlot();
        M.show();
        M.setData(M.setup2Ddata(40,40));
    }

    private double [][]  setup2Ddata(int m, int n)
    {
        double[][]  data = new double[m][n];
        int i; int j;
        double x; double y;


        double a  = -3.0;                     // create data for surface plot
        double b  =  3.0;
        double c  = -3.0;
        double d =   3.0;

        double hx  = (b-a)/((double)m-1);
        double hy  = (d-c)/((double)n-1);

        for(i =0; i < m; i++)
        {
            x    = a + ((double)i)*hx;
            for(j = 0; j < n; j++)
            {
                y    = c + ((double)j)*hy;
                data[i][j]  = f(x,y);
            }}
        return data;

    }
    private double f(double x,double y)
    {
        return (3*(1-x)*(1-x)*(Math.exp(-x*x - (y+1)*(y+1)))
                -10*(x/5 - x*x*x - y*y*y*y*y)*(Math.exp(-x*x - y*y))
                -(1/3.0)*(Math.exp(-(x+1)*(x+1) - y*y)));
    }

    boolean RunningAsThread;
    boolean captureMode;

    public ColorContourCanvas contourCanvas;
    ContourResolutionDialog   resDialog;

    String javaVersion = new String();

    java.util.Vector contourData;
    int currentFrame;
}