package temp_calc_cpp;

import java.awt.*;
//
//###################################################################
//                   Class ColorContourCanvas
//###################################################################
//
//
//  Creator               : Chris Anderson
//  (C) UCLA 1997
//
//###################################################################
//
public class ColorContourCanvas extends java.awt.Canvas
{
    public ColorContourCanvas(ColorContourPlot parent)
    {
        super();
        Parent      = parent;

        xDataCount = 0;
        yDataCount = 0;

        xCanvasPanelCount = 60;
        yCanvasPanelCount = 60;

        contourData  = null;
        dataRangeMin = 0.0;
        dataRangeMax = 0.0;

        colorCount  = 64;
        colorArray = new Color[colorCount];
        setupColors();
    }

    public void setData(double [] inputData, int xCount, int yCount)
    {
//
//  This routine assumes that the incoming data is stored by ROWS
//
        contourData = new double[xCount*yCount];
        System.arraycopy(inputData,0,contourData,0,xCount*yCount);
        xDataCount = xCount;
        yDataCount = yCount;
        repaint();
    }

    public void setData(double [][] inputData)
    {
        xDataCount = inputData.length;
        yDataCount = inputData[0].length;
        contourData = new double[xDataCount*yDataCount];
        int i;
        for(i = 0; i < xDataCount; i++)
        {
            System.arraycopy(inputData[i],0,contourData,i*yDataCount,yDataCount);
        }
        repaint();
    }

    public void setDataRange(double dMin, double dMax)
    {
        dataRangeMin = dMin;
        dataRangeMax = dMax;
    }

    public void setResolution(int PanelCount)
    {
        xCanvasPanelCount = PanelCount;
        yCanvasPanelCount = PanelCount;
    }

    public int  getResolution()
    {
        return xCanvasPanelCount;
    }
    public void setDataRange()
    {
        if(contourData != null)
        {
            dataRangeMin = contourData[0];
            dataRangeMax = contourData[0];
            double val;
            int i; int j;
            for(i =0;  i < xDataCount; i++)
            {
                for(j = 0; j < yDataCount; j++)
                {
                    val = contourData[j + i*yDataCount];
                    if(val < dataRangeMin) dataRangeMin = val;
                    if(val > dataRangeMax) dataRangeMax = val;
                }}
        }
    }
    //
// Using bilinear interpolation of the input data; Data is
// assumed to be at the nodes of a grid. The rectangle color value
// is taken to be the value at the center of the rectangle of
// a "canvas" grid decomposed into xCanvasPanelCount X yCanvasPanelCount
// panels.
//
    public void paint(Graphics g)
    {

        if(contourData == null) return;

        if(Math.abs(dataRangeMax- dataRangeMin) < 1.0e-10) setDataRange();
        if(Math.abs(dataRangeMax- dataRangeMin) < 1.0e-10) return;

        Dimension D = size();

        int boxWidth  = D.width/xCanvasPanelCount;
        int boxHeight = D.height/yCanvasPanelCount;
        if(boxWidth  <= 1)
        {
            xCanvasPanelCount = D.width/2;
            boxWidth  =  D.width/xCanvasPanelCount;
        }
        if(boxHeight <= 1)
        {
            yCanvasPanelCount = D.height/2;
            boxHeight = D.height/yCanvasPanelCount;
        }
        int xOffset = (D.width  - xCanvasPanelCount*boxWidth)/2;
        int yOffset = (D.height - yCanvasPanelCount*boxHeight)/2;
        int xCanvasLocation;
        int yCanvasLocation;


        double canvasHx; double canvasHy;
        double dataHx;   double dataHy;
        canvasHx = 1.0/((double)(xCanvasPanelCount));
        canvasHy = 1.0/((double)(yCanvasPanelCount));
        dataHx   = 1.0/((double)(xDataCount-1));
        dataHy   = 1.0/((double)(yDataCount-1));

        double xLocation;
        double yLocation;

        int xDataIndex;
        int yDataIndex;

        int i; int j;
        double colorLocation;
        double dataValue;
        int colorIndex;

        double p; double q;
        double v1; double v2; double v3; double v4;
        double phi0; double phi1;

        for(i =0; i < xCanvasPanelCount; i++)
        {
            xCanvasLocation  = xOffset + i*boxWidth;
            xLocation        = canvasHx/2.0 + ((double)i)*canvasHx;
            xDataIndex = (int)(xLocation/dataHx);
            for(j = 0; j < yCanvasPanelCount; j++)
            {
                yCanvasLocation  = yOffset + j*boxHeight;
                yLocation        = canvasHy/2.0 + ((double)j)*canvasHy;
                yDataIndex = (int)(yLocation/dataHy);
                //
                // do bilinear interpolation
                //
                p = (xLocation - xDataIndex*dataHx)/dataHx;
                q = (yLocation - yDataIndex*dataHy)/dataHy;
                v1 = contourData[yDataIndex + xDataIndex*yDataCount];
                v2 = contourData[yDataIndex+ (xDataIndex+1)*yDataCount];
                v3 = contourData[(yDataIndex + 1) + (xDataIndex+1)*yDataCount];
                v4 = contourData[(yDataIndex + 1) + xDataIndex*yDataCount];
                phi0 = v1 + p*(v2 - v1);
                phi1 = v4 + p*(v3 - v4);
                dataValue = phi0 + q*(phi1 - phi0);

                colorLocation =
                        ((colorCount-1.0)*(dataValue - dataRangeMin))/(dataRangeMax - dataRangeMin);
                colorIndex = (int)colorLocation;
                if(colorIndex >= colorCount) colorIndex = colorCount-1;
                if(colorIndex < 0) colorIndex = 0;
                g.setColor(colorArray[colorIndex]);
                g.fillRect(xCanvasLocation,yCanvasLocation,boxWidth,boxHeight);
            }}
        g.setColor(Color.black);
    }



    public void setupColors()
    {
//
//  Setting up colors based upon hue....
//
        int i;
        float hinc = (float)(1.0/colorCount);
        float h = hinc;
        float saturation = (float)(0.8);
        float intensity  = (float)(0.9);
        for(i = 0; i < colorCount; i++)
        {
            int color = Color.HSBtoRGB(h, saturation, intensity);
            h +=  hinc;
            colorArray[i] = new Color(color);
        }
    }


    Container Parent;

    int xCanvasPanelCount;  // determines the resolution of the canvas
    int yCanvasPanelCount;

    int xDataCount;
    int yDataCount;

    double [] contourData;
    double dataRangeMin;
    double dataRangeMax;

    int colorCount;
    Color[] colorArray;

}