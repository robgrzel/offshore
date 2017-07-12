package com.codebind;

import com.jcraft.jsch.*;

import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import static javax.swing.SwingUtilities.updateComponentTreeUI;

/**
 * Created by robgrzel on 19.06.17.
 */
class MyThread1 extends Thread {
    int k;
    static int n;
    private volatile boolean toExit;

    public MyThread1(int i) {
        k = i;
        n++;
    }

    @Override
    public void run() {
        toExit = false;
        //Your Code

       while (!toExit) {
            System.out.println("Thread no. " + k + "/" + currentThread().getId());
            // Your code
            try {
                sleep(100);
            } catch (Exception e) {
                System.out.println(e);
            }
        }

        System.out.println("Thread no. " + k + " exit...");
    }

    public void exit() {
        toExit = true;

    }


}

public class App {

    static App app;

    static private int numServers = 6;

    private JPanel panelMain;
    private JLabel JLabel1;
    private JLabel JLabel2;
    private JLabel JLabel3;
    private JLabel JLabel4;
    private JLabel JLabel5;
    private JLabel JLabel6;
    private JButton DCButton;
    private JProgressBar progressBar1;

    private JLabel labels[] = {JLabel1, JLabel2, JLabel3, JLabel4, JLabel5, JLabel6};

    public Color hoverColor = new Color(200, 50, 50);
    public Color activeColor = new Color(0, 255, 128);
    public Color transparent = new Color(0, 0, 0, 0);
    public Color statusColor[] = {Color.WHITE, Color.BLACK};


    private int status = 0;
    private String statusText = "";

    static private String hosts[];
    static private String user = "student";

    static private JSch jsch[];
    static private Session jschSession[];


    private static int id = 0;

    private static ExecutorService exec;
    private static MyThread1 threads[];

    private App() {


        DCButton.addActionListener(new ActionListener() {
            /**
             * Invoked when an action occurs.
             *
             * @param e
             */
            //@Override
            public void actionPerformed(ActionEvent e) {


                if (status == 0) {
                    status = 1;
                    statusText = "connected\n";
                    statusColor[0] = activeColor;
                    statusColor[1] = Color.BLACK;

                } else {
                    status = 0;
                    statusText = "disconnected\n";
                    statusColor[0] = hoverColor;
                    statusColor[1] = Color.WHITE;
                }

                for (int i = 0; i < App.numServers; i++) {
                    if (status == 1) {
                        threads[i] = new MyThread1(i);
                        exec.execute(threads[i]);
                    } else if (status == 0) {
                        threads[i].exit();
                    }

                    labels[i].setBackground(statusColor[0]);
                    labels[i].setForeground(statusColor[1]);

                }

                DCButton.setText(statusText);

                if (status == 0) {
                    exec.shutdownNow();
                }

            }
        });


    }

    public static void main(String[] args) {
        try {
            // Set cross-platform Java L&F (also called "Metal")
            UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());

        } catch (UnsupportedLookAndFeelException e) {
            System.out.println("1:" + e);
        } catch (ClassNotFoundException e) {
            System.out.println("2:" + e);
        } catch (InstantiationException e) {
            System.out.println("3:" + e);
        } catch (IllegalAccessException e) {
            System.out.println("4:" + e);
        }

        app = new App(); //Create and show the GUI.

        JFrame frame = new JFrame("App");
        frame.setContentPane(app.panelMain);
        frame.setDefaultCloseOperation(WindowConstants.EXIT_ON_CLOSE);
        frame.pack();
        frame.setVisible(true);

        initHosts();
        initJsch();
        initJschSession();

        setLabelsColors();

        initThreads();

        for (int i = 0; i < 6; i++) {
            try {
                System.out.println(user);
                System.out.println(hosts[i]);

                jsch[i] = new JSch();
                jschSession[i] = jsch[i].getSession(user, hosts[i], 22);

            } catch (Exception e) {
                System.out.println(e);
            }
        }
        for (int i = 0; i < 6; i++) {
            try {
                System.out.println(user);
                System.out.println(hosts[i]);

                System.out.println(jsch[i]);
                System.out.println(jschSession[i]);

            } catch (Exception e) {
                System.out.println(e);
            }
        }


    }

    public static void setLabelsColors() {

        for (int i = 0; i < App.numServers; i++) {
            System.out.println(i);
            app.labels[i].setForeground(Color.WHITE);
            app.labels[i].setBackground(Color.BLACK);

        }
    }

    public static void setJsch(JSch[] jsch) {
        App.jsch = jsch;
    }

    public static void setJschSession(Session[] jschSession) {
        App.jschSession = jschSession;
    }

    public static void setHosts(String[] hosts) {
        App.hosts = hosts;
    }

    private static void initJsch() {
        App.jsch = new JSch[App.numServers];
    }

    private static void initJschSession() {

        App.jschSession = new Session[App.numServers];
    }

    private static void initHosts() {
        App.hosts = new String[App.numServers];
        for (int i = 0; i < 6; i++) {
            hosts[i] = app.labels[i].getText();
        }
    }

    private static void initThreads() {
        threads = new MyThread1[App.numServers];
        App.exec = Executors.newFixedThreadPool(App.numServers);
    }



};

