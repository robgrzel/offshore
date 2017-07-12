package temp_calc_cpp;

import java.awt.*;
//
//###################################################################
//                   Class TemperatureApp
//###################################################################
//
//
//  Creator               : Chris Anderson
//  (C) UCLA 1997
//
//###################################################################
//
public class TemperatureApp extends Frame
{

    public TemperatureApp() {

        //{{INIT_CONTROLS
        setLayout(null);
        addNotify();
        resize(insets().left + insets().right + 467,insets().top + insets().bottom + 356);
        setBackground(new Color(16777215));
        RunButton = new java.awt.Button("Run");
        RunButton.reshape(insets().left + 195,insets().top + 316,76,27);
        add(RunButton);
        label1 = new java.awt.Label("Run Parameters",Label.CENTER);
        label1.reshape(insets().left + 28,insets().top + 104,132,29);
        label1.setFont(new Font("Dialog", Font.BOLD, 14));
        label1.setBackground(new Color(16762880));
        add(label1);
        label2 = new java.awt.Label("Simulation Parameters ",Label.CENTER);
        label2.reshape(insets().left + 28,insets().top + 12,176,29);
        label2.setFont(new Font("Dialog", Font.BOLD, 14));
        label2.setBackground(new Color(16762880));
        add(label2);
        label3 = new java.awt.Label("Diffusivity");
        label3.reshape(insets().left + 28,insets().top + 60,84,29);
        add(label3);
        label4 = new java.awt.Label("Final Time");
        label4.reshape(insets().left + 248,insets().top + 200,88,29);
        label4.setFont(new Font("Dialog", Font.BOLD, 12));
        add(label4);
        label5 = new java.awt.Label("Initial Time");
        label5.reshape(insets().left + 28,insets().top + 200,88,29);
        add(label5);
        label6 = new java.awt.Label("Number of Time Steps");
        label6.reshape(insets().left + 28,insets().top + 248,148,29);
        add(label6);
        label7 = new java.awt.Label("Number of X panels");
        label7.reshape(insets().left + 28,insets().top + 152,136,29);
        add(label7);
        label8 = new java.awt.Label("Number of Y panels");
        label8.reshape(insets().left + 248,insets().top + 152,132,29);
        add(label8);
        label9 = new java.awt.Label("Output Every ");
        label9.reshape(insets().left + 248,insets().top + 248,96,29);
        add(label9);
        diffusivityField = new java.awt.TextField();
        diffusivityField.setText(".1");
        diffusivityField.reshape(insets().left + 128,insets().top + 60,50,35);
        diffusivityField.setFont(new Font("TimesRoman", Font.BOLD, 14));
        add(diffusivityField);
        xPanelField = new java.awt.TextField();
        xPanelField.setText("40");
        xPanelField.reshape(insets().left + 180,insets().top + 156,50,35);
        xPanelField.setFont(new Font("TimesRoman", Font.BOLD, 14));
        add(xPanelField);
        yPanelField = new java.awt.TextField();
        yPanelField.setText("40");
        yPanelField.reshape(insets().left + 388,insets().top + 156,50,35);
        yPanelField.setFont(new Font("TimesRoman", Font.BOLD, 14));
        add(yPanelField);
        initialTimeField = new java.awt.TextField();
        initialTimeField.setText("0.0");
        initialTimeField.reshape(insets().left + 180,insets().top + 200,50,35);
        initialTimeField.setFont(new Font("TimesRoman", Font.BOLD, 14));
        add(initialTimeField);
        finalTimeField = new java.awt.TextField();
        finalTimeField.setText("1.0");
        finalTimeField.reshape(insets().left + 388,insets().top + 200,50,35);
        finalTimeField.setFont(new Font("TimesRoman", Font.BOLD, 14));
        add(finalTimeField);
        stepsField = new java.awt.TextField();
        stepsField.setText("1000");
        stepsField.reshape(insets().left + 180,insets().top + 248,50,35);
        stepsField.setFont(new Font("TimesRoman", Font.BOLD, 14));
        add(stepsField);
        outputEveryField = new java.awt.TextField();
        outputEveryField.setText("100");
        outputEveryField.reshape(insets().left + 388,insets().top + 248,50,35);
        outputEveryField.setFont(new Font("TimesRoman", Font.BOLD, 14));
        add(outputEveryField);
        StopButton = new java.awt.Button("Stop");
        StopButton.reshape(insets().left + 345,insets().top + 316,76,27);
        add(StopButton);
        QuitButton = new java.awt.Button("Quit");
        QuitButton.reshape(insets().left + 45,insets().top + 316,76,27);
        add(QuitButton);
        setTitle("Temperature Evolution Application");
        //}}

        temperatureRun         = new TempCalcJava();
        //{{INIT_MENUS
        //}}
    }

    public synchronized void show()
    {
        move(50, 50);
        super.show();
    }

    public boolean handleEvent(Event event)
    {
        if (event.id == Event.WINDOW_DESTROY)
        {
            System.exit(0);        // kill the application
        }
        if (event.target == RunButton && event.id == Event.ACTION_EVENT)
        {
            RunButton_Clicked(event);
            return true;
        }
        if (event.target == StopButton && event.id == Event.ACTION_EVENT) {
            StopButton_Clicked(event);
            return true;
        }
        if (event.target == QuitButton && event.id == Event.ACTION_EVENT)
        {
            System.exit(0);        // kill the application
            return true;
        }
        return super.handleEvent(event);
    }

    //{{DECLARE_CONTROLS
    java.awt.Button RunButton;
    java.awt.Label label1;
    java.awt.Label label2;
    java.awt.Label label3;
    java.awt.Label label4;
    java.awt.Label label5;
    java.awt.Label label6;
    java.awt.Label label7;
    java.awt.Label label8;
    java.awt.Label label9;
    java.awt.TextField diffusivityField;
    java.awt.TextField xPanelField;
    java.awt.TextField yPanelField;
    java.awt.TextField initialTimeField;
    java.awt.TextField finalTimeField;
    java.awt.TextField stepsField;
    java.awt.TextField outputEveryField;
    java.awt.Button StopButton;
    java.awt.Button QuitButton;
    //}}

    void RunButton_Clicked(Event event)
    {

//
//  Set data for temperature run
//
        temperatureRun.a           = 0.0;
        temperatureRun.b           = 1.0;
        temperatureRun.c           = 0.0;
        temperatureRun.d           = 1.0;

        temperatureRun.diffusivity = (new Double(diffusivityField.getText())).doubleValue();

        temperatureRun.m           = (new Integer(xPanelField.getText())).intValue();
        temperatureRun.n           = (new Integer(yPanelField.getText())).intValue();
        double initialTime         = (new Double(initialTimeField.getText())).doubleValue();
        double finalTime           = (new Double(finalTimeField.getText())).doubleValue();

        temperatureRun.nSteps      = (new Integer(stepsField.getText())).intValue();
        temperatureRun.dt          = (finalTime - initialTime)/((double)temperatureRun.nSteps);
        temperatureRun.nOut        = (new Integer(outputEveryField.getText())).intValue();
//
//  Set for graphics output
//
        temperatureRun.contourPlot = new ColorContourPlot();
        temperatureRun.contourPlot.setCaptureMode(true);
        temperatureRun.contourPlot.setDataRange(-1.0,1.0);
//
//  Set up and start the threads for the contour plot and the
//  calculation
//
        Thread current = Thread.currentThread(); // capture current thread

        Thread contourThread     = new Thread(temperatureRun.contourPlot);
        contourThread.start();

        TempRunThread = new Thread(temperatureRun);
        TempRunThread.setPriority(current.getPriority() -1);
        TempRunThread.start();
    }
    void StopButton_Clicked(Event event)
    {
        try{TempRunThread.stop();}catch(Exception e){};
    }
    //
//#########################################################
//                MAIN
//#########################################################
//
    public static void main(String args[])
    {
        TemperatureApp Tapp = new TemperatureApp();
        Tapp.show();
    }
//
//  Class Data Members
//

    TempCalcJava temperatureRun;
    Thread        TempRunThread;
    //{{DECLARE_MENUS
    //}}
}