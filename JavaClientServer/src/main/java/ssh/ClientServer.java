package ssh;
/* -*-mode:java; c-basic-offset:2; indent-tabs-mode:nil -*- */

import com.jcraft.jsch.*;

import java.awt.*;
import javax.swing.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.*;
import java.util.Date;
import java.io.File;
import java.util.Enumeration;
import java.util.Vector;
import javax.swing.event.TreeSelectionEvent;
import javax.swing.event.TreeSelectionListener;
import javax.swing.tree.DefaultMutableTreeNode;


public class ClientServer extends JFrame {


    static private ClientServer clientServer;

    static private int numServers = 9;

    static String hosts[] = {
            "des01.eti.pg.gda.pl",
            "des02.eti.pg.gda.pl",
            "des03.eti.pg.gda.pl",
            "des04.eti.pg.gda.pl",
            "des05.eti.pg.gda.pl",
            "des06.eti.pg.gda.pl",
            "des07.eti.pg.gda.pl",
            "des08.eti.pg.gda.pl",
            "des09.eti.pg.gda.pl"};

    private String passwordRoot = "1283";
    static String user = "student";
    static String password = "student";
    static String commCheckConn = "set|grep SSH";
    static String SFTPWORKINGDIR = "file/to/transfer";

    public Color hoverColor = new Color(200, 50, 50);
    public Color activeColor = new Color(0, 255, 128);
    public Color transparent = new Color(0, 0, 0, 0);
    public Color statusColor[] = {Color.WHITE, Color.BLACK};

    static private boolean killChilds = false;
    static private boolean terminateCons = false;
    static private boolean doConnect[] = new boolean[numServers]; //true;
    static private boolean isConnected[] = new boolean[numServers];//false;
    static private boolean checkConnection[] = new boolean[numServers];// true;
    private boolean barRunning[] = new boolean[numServers];// false;
    Session session[] = new Session[numServers];

    private int status = 0;
    private String statusText = "";

    private static final long serialVersionUID = 1L;

    private JPanel panelMain;
    private JButton DCButton;
    private JLabel JLabel1;
    private JLabel JLabel2;
    private JLabel JLabel3;
    private JLabel JLabel4;
    private JLabel JLabel5;
    private JLabel JLabel6;
    private JLabel JLabel7;
    private JLabel JLabel8;
    private JLabel JLabel9;

    private JProgressBar progressBar1;
    private JProgressBar progressBar2;
    private JProgressBar progressBar3;
    private JProgressBar progressBar4;
    private JProgressBar progressBar5;
    private JProgressBar progressBar6;
    private JProgressBar progressBar7;
    private JProgressBar progressBar8;
    private JProgressBar progressBar9;
    private JTabbedPane tabbedPane1;
    private JPanel tabJpanel1;
    private JTextField textField1;
    private JPanel tabJpanel2;
    private JTextField textField2;
    private JTree rtree;


    String path;

    File fileRoot;

    FileTree filetree;


    private JLabel jlabels[] = {
            JLabel1,
            JLabel2,
            JLabel3,
            JLabel4,
            JLabel5,
            JLabel6,
            JLabel7,
            JLabel8,
            JLabel9};

    private JProgressBar progressBars[] = {
            progressBar1,
            progressBar2,
            progressBar3,
            progressBar4,
            progressBar5,
            progressBar6,
            progressBar7,
            progressBar8,
            progressBar9};

    private int barVal = 0;

    class ProgressBarPainter implements Runnable {

        int id = -1;

        ProgressBarPainter(int i) {
            id = i;
        }

        public void run() {
            while (true) {
                try {
                    Thread.sleep(10);
                    progressBars[id].repaint();
                } catch (InterruptedException ex) {
                    break;
                }
            }
        }
    }

    class LabelPainter implements Runnable {
        public Color statusColor[] = {Color.WHITE, Color.BLACK};
        int id = -1;

        LabelPainter(int i) {
            id = i;
        }

        public void run() {
            while (true) {
                try {

                    if (isConnected[id]) {
                        statusText = "connected\n";
                        statusColor[0] = activeColor;
                        statusColor[1] = Color.BLACK;
                    } else {
                        statusText = "disconnected\n";
                        statusColor[0] = hoverColor;
                        statusColor[1] = Color.WHITE;
                    }

                    Thread.sleep(50);
                    jlabels[id].setBackground(statusColor[0]);
                    jlabels[id].setForeground(statusColor[1]);

                } catch (InterruptedException ex) {
                    System.out.printf("LabelPainter, %s", ex);
                }
            }
        }
    }

    class ConnsChecker implements Runnable {
        MyUserInfo ui;
        JSch jsch;
        Channel channel;
        InputStream inputStream;

        int id = -1;
        boolean verbose = false;

        ConnsChecker(int i, boolean verb) {
            id = i;
            verbose = verb;
        }

        private void set_channel_command(String command) {
            if (verbose) System.out.println("set_channel_command");

            try {
                channel = session[id].openChannel("exec");
            } catch (Exception err) {
                if (verbose) System.out.printf("%s, %s\n", "openChannel", err);
            }

            try {
                inputStream = channel.getInputStream();
            } catch (Exception err) {
                if (verbose) System.out.printf("%s, %s\n", "getInputStream", err);
            }

            channel.setInputStream(null);
            ((ChannelExec) channel).setCommand(command);
            ((ChannelExec) channel).setErrStream(System.err);

            try {
                channel.connect();
            } catch (Exception err) {
                if (verbose) System.out.printf("%s, %s\n", "connect", err);
                isConnected[id] = false;
                checkConnection[id] = false;
            }


        }

        private void get_input_stream() {
            if (verbose) System.out.println("get_input_stream");
            byte[] tmp = new byte[1024];
            int i = 0;
            boolean isAvaiable = true;
            while (true) {
                if (verbose) System.out.print("...while\n");

                try {
                    isAvaiable = inputStream.available() > 0;
                } catch (Exception err) {
                    if (verbose) System.out.printf("%s, %s\n", "available", err);
                    return;
                }

                while (isAvaiable) {
                    try {
                        i = inputStream.read(tmp, 0, 1024);
                    } catch (Exception err) {
                        if (verbose) System.out.printf("%s, %s\n", "read", err);
                        return;
                    }

                    if (i < 0) break;
                    System.out.printf("des0%d.eti.pg.gda.pl : ", id);
                    System.out.print(new String(tmp, 0, i));

                    try {
                        isAvaiable = inputStream.available() > 0;
                    } catch (Exception err) {
                        if (verbose) System.out.printf("%s, %s\n", "available", err);
                        return;
                    }

                }

                if (verbose) System.out.print("...if\n");

                if (channel.isClosed()) {
                    try {
                        isAvaiable = inputStream.available() > 0;
                    } catch (Exception err) {
                        if (verbose) System.out.printf("%s, %s\n", "available", err);
                        return;
                    }

                    if (isAvaiable) continue;
                    if (verbose) System.out.println("exit-status: " + channel.getExitStatus());
                    return;
                }

                System.out.println("...sleep");

                try {
                    Thread.sleep(100);
                } catch (Exception err) {
                    if (verbose) System.out.printf("%s, %s\n", "sleep", err);
                    return;
                }
            }
        }

        private void update_bar(int v) {
            if (verbose) System.out.println("...update_bar");
            progressBars[id].setValue(v);
        }

        private void update_bar() {
            if (verbose) System.out.println("...update_bar");
            progressBars[id].setValue(barVal++);
        }


        public void run() {
            if (verbose) System.out.println("...run");
            JSch.setLogger(new JSCHLogger()); // More on this below
            if (verbose) System.out.printf("System Date :: %s\n", new Date());
            while (!terminateCons) {
                if (verbose) System.out.println("...while (doConnect[id])");
                if (verbose) System.out.printf("System Date :: %s\n", new Date());
                try {
                    if (verbose) System.out.print("......new\n");
                    if (verbose) System.out.printf("System Date :: %s\n", new Date());

                    while (doConnect[id]) {
                        update_bar(0);

                        if (!isConnected[id]) {
                            if (verbose) System.out.print("...... if (!isConnected[id]) \n");
                            if (verbose) System.out.printf("System Date :: %s\n", new Date());
                            update_bar(1);
                            jsch = new JSch();
                            update_bar(2);
                            ui = new MyUserInfo();
                            update_bar(3);

                            ui.setPassword(password);
                            update_bar(5);

                            session[id] = jsch.getSession(ClientServer.user, hosts[id], 22);
                            update_bar(10);
                            session[id].setPassword(password);
                            update_bar(15);
                            session[id].setConfig("PreferredAuthentications", "publickey,keyboard-interactive,password");
                            update_bar(20);

                            session[id].setUserInfo(ui);
                            update_bar(30);
                            if (verbose) System.out.println("session.connect");
                            if (verbose) System.out.printf("System Date :: %s\n", new Date());
                            session[id].connect();
                            update_bar(40);

                            isConnected[id] = true;
                            if (verbose) System.out.println("isConnected");
                            if (verbose) System.out.printf("System Date :: %s\n", new Date());
                        }

                        update_bar(50);
                        Thread.sleep(500);

                        while (checkConnection[id]) {
                            if (verbose) System.out.print("...... while (checkConnection[id])\n");
                            if (verbose) System.out.printf("System Date :: %s\n", new Date());

                            set_channel_command(commCheckConn);

                            if (verbose) System.out.print("...... update_bar(50) \n");
                            if (verbose) System.out.printf("System Date :: %s\n", new Date());
                            get_input_stream();
                            if (verbose) System.out.print("...... get_input_stream() \n");
                            if (verbose) System.out.printf("System Date :: %s\n", new Date());
                            update_bar(100);
                            if (verbose) System.out.print("...... update_bar(100) \n");
                            if (verbose) System.out.printf("System Date :: %s\n", new Date());
                            Thread.sleep(1000);
                            channel.disconnect();
                            if (verbose) System.out.print("...... channel.disconnect() \n");
                            if (verbose) System.out.printf("System Date :: %s\n", new Date());


                        }

                        if (verbose) System.out.println("session.disconnect");
                        if (verbose) System.out.printf("System Date :: %s\n", new Date());

                        //session.disconnect();

                        checkConnection[id] = true;
                        //update_bar(0);
                        Thread.sleep(1000);

                        if (!session[id].isConnected())
                            isConnected[id] = false;
                    }

                } catch (Exception err) {
                    if (verbose) System.out.printf("%s, %s\n", "main", err);
                    if (verbose) System.out.printf("System Date :: %s\n", new Date());

                    isConnected[id] = false;
                    try {
                        Thread.sleep(1000);

                    } catch (Exception err1) {
                        if (verbose) System.out.printf("%s, %s\n", "main.sleep", err1);
                        if (verbose) System.out.printf("System Date :: %s\n", new Date());

                    }
                }
            }
            update_bar(0);

        }
    }

    class cargarRTree implements Runnable {
        Channel channel;
        InputStream inputStream;
        int id = -1;
        String remotePath;
        DefaultMutableTreeNode parent;

        cargarRTree(String path, DefaultMutableTreeNode par, int i) {
            parent = par;
            remotePath = path;
            id = i;
        }

        private void set_channel_command(String command) {


            try {
                channel = session[id].openChannel(command);
            } catch (Exception err) {
                System.out.printf("%s, %s\n", "openChannel", err);
            }

            try {
                inputStream = channel.getInputStream();
            } catch (Exception err) {
                System.out.printf("%s, %s\n", "getInputStream", err);
            }

            try {
                channel.connect();
            } catch (Exception err) {
                System.out.printf("%s, %s\n", "connect", err);
            }


        }

        private void get_input_stream() {
            ChannelSftp sftpChannel = (ChannelSftp) channel;
            Vector<ChannelSftp.LsEntry> list = null; // List source directory structure.
            try {
                list = sftpChannel.ls(remotePath);
            } catch (SftpException e) {
                e.printStackTrace();
            }


            for (ChannelSftp.LsEntry oListItem : list) { // Iterate objects in the list to get file/folder names.
                DefaultMutableTreeNode node = new DefaultMutableTreeNode(oListItem.getFilename());
                if (!oListItem.getAttrs().isDir()) { // If it is a file (not a directory).
                    parent.add(node); // add as a child node
                } else {
                    if (!".".equals(oListItem.getFilename()) && !"..".equals(oListItem.getFilename())) {
                        parent.add(node); // add as a child node
                        new cargarRTree(remotePath + "/" + oListItem.getFilename(), node, id); // call again for the subdirectory
                    }
                }
            }
        }


        public void run() {
            JSch.setLogger(new JSCHLogger()); // More on this below

            while (!killChilds) {
                try {

                    while (isConnected[id]) {

                        set_channel_command("sftp");

                        get_input_stream();
                        channel.disconnect();


                        Thread.sleep(1000);
                    }

                } catch (Exception err) {
                    System.out.printf("%s, %s\n", "catch", err);

                    try {
                        Thread.sleep(1000);

                    } catch (Exception err1) {
                        System.out.printf("%s, %s\n", "main.sleep", err1);

                    }
                }

                try {
                    Thread.sleep(500);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }

        }
    }

    private void openvpn_connect_delli20() {


        try {

            String command = "echo '1283' | sudo -S openvpn delli20vpn.ovpn";
            System.out.println(command);
            String ss = null;
            Runtime obj = null;
            Runtime commandPrompt = Runtime.getRuntime();
            Process p = commandPrompt.exec(
                    command, null, new File(
                            getClass().getClassLoader().getResource("").getPath() + "openvpn_config")
            );

            try {
                p.waitFor();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }

            BufferedReader stdInput = new BufferedReader(new InputStreamReader(p.getInputStream()));
            BufferedReader stdError = new BufferedReader(new InputStreamReader(p.getErrorStream()));
            System.out.println("Here is the standard output of the command:\n");
            int n = 0;

            n = 0;
            while (n < 1e6) {
                ss = stdInput.readLine();
                if (ss != null)
                    System.out.println(ss);
                n++;
            }
            System.out.println("Here is the standard error of the command (if any):\n");
            while ((ss = stdError.readLine()) != null) {
                System.out.println(ss);
            }

        } catch (IOException e) {
            System.out.println("FROM CATCH" + e.toString());
        }

    }


    public void get_file_names() {

        DefaultMutableTreeNode root = (DefaultMutableTreeNode) filetree.tree.getModel().getRoot();
        Enumeration e = root.preorderEnumeration();
        if (e.hasMoreElements()) {
            Object rootElem = e.nextElement();
            File rootFile = new File(rootElem.toString());
            if (rootFile.isDirectory()) {
                Object prevElem = rootElem;
                Object currElem;
                File prevFile = rootFile;
                File currFile, currDir;
                File[] files_, files;
                while (e.hasMoreElements()) {
                    currElem = e.nextElement();
                    if (new File(currElem.toString()).isDirectory()) {
                        currDir = new File(rootElem.toString() + "//" + currElem.toString());
                        System.out.printf("dir: %s\n", currDir);
                        files_ = currDir.listFiles();
                        files = new File[files_.length];
                        for (int i = 0; i < files_.length; i++) {
                            if (!files_[i].isDirectory()) {
                                files[i] = files_[i];
                            }
                        }

                        //System.out.println(Arrays.toString(files));
                    }

                }
            }
        }
    }

    private ClientServer() {
        System.out.println("...ClientServer");

        //openvpn_connect_delli20();

        path = ".";

        fileRoot = new File(path);

        filetree = new FileTree(fileRoot);


        SwingUtilities.invokeLater(new Runnable() {
            public void run() {
                get_file_names();
            }
        });

        tabbedPane1.add("L-DIR", filetree);


        setContentPane(panelMain);

        for (int i = 0; i < numServers; i++) {
            barVal = 0;
            isConnected[i] = false;
            checkConnection[i] = true;
            doConnect[i] = true;
            barRunning[i] = false;
        }

        DCButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                SwingUtilities.invokeLater(new Runnable() {
                    public void run() {
                        Thread t1[] = new Thread[numServers];
                        Thread t2[] = new Thread[numServers];
                        Thread t3[] = new Thread[numServers];
                        for (int i = 0; i < numServers; i++) {
                            if (barRunning[i]) {
                                barVal = 0;
                                checkConnection[i] = false;
                                doConnect[i] = false;
                                barRunning[i] = false;
                                terminateCons = true;
                            } else {
                                terminateCons = false;
                                checkConnection[i] = true;
                                doConnect[i] = true;
                                barRunning[i] = true;

                                t1[i] = new Thread(new ConnsChecker(i, true));
                                t2[i] = new Thread(new ProgressBarPainter(i));

                            }

                            t3[i] = new Thread(new LabelPainter(i));

                        }

                        for (int i = 0; i < numServers; i++) {
                            if (barRunning[i]) {
                                t1[i].start();
                                t2[i].start();
                            }

                            t3[i].start();
                        }
                    }
                });


            }

        });

        SwingUtilities.invokeLater(new Runnable() {
            public void run() {
                DefaultMutableTreeNode nroot = new DefaultMutableTreeNode("/home/student");
                Thread t1 = new Thread(new cargarRTree("/home/student", nroot, 1));
                t1.start();
                JPanel jpanel1 = new JPanel();

                BorderLayout layout = new BorderLayout();
                jpanel1.setLayout(layout);

                // Make a tree list with all the nodes, and make it a JTree
                rtree = new JTree(nroot);

                // Add a listener
                rtree.addTreeSelectionListener(new TreeSelectionListener() {
                    public void valueChanged(TreeSelectionEvent e) {
                        DefaultMutableTreeNode node = (DefaultMutableTreeNode) e
                                .getPath().getLastPathComponent();
                        System.out.println("You selected remote " + node);
                    }
                });

                // Lastly, put the JTree into a JScrollPane.
                JScrollPane scrollpane = new JScrollPane();
                scrollpane.getViewport().add("FilesTree", rtree);

                jpanel1.add(BorderLayout.CENTER, scrollpane);
                tabbedPane1.add("R-DIR", jpanel1);

            }
        });

        Runtime.getRuntime().addShutdownHook(new Thread() {
            @Override
            public void run() {
                killChilds = true;
            }
        });

        pack();


        setDefaultCloseOperation(WindowConstants.EXIT_ON_CLOSE);

        setVisible(true);
    }


    public class CreateChildNodes implements Runnable {

        private DefaultMutableTreeNode root;

        private File fileRoot;

        public CreateChildNodes(File fileRoot,
                                DefaultMutableTreeNode root) {
            this.fileRoot = fileRoot;
            this.root = root;
        }

        public void run() {
            createChildren(fileRoot, root);
        }

        private void createChildren(File fileRoot,
                                    DefaultMutableTreeNode node) {
            File[] files = fileRoot.listFiles();
            if (files == null) return;

            for (File file : files) {
                DefaultMutableTreeNode childNode =
                        new DefaultMutableTreeNode(new FileNode(file));
                node.add(childNode);

                if (file.isDirectory()) {
                    createChildren(file, childNode);
                }

            }
        }

    }

    public class FileNode {

        private File file;

        public FileNode(File file) {
            this.file = file;
        }

        @Override
        public String toString() {
            String name = file.getName();
            if (name.equals("")) {
                return file.getAbsolutePath();
            } else {
                return name;
            }
        }
    }

    public static class JSCHLogger implements com.jcraft.jsch.Logger {

        static java.util.Hashtable<Integer, String> name = new java.util.Hashtable<Integer, String>();

        static {
            name.put(DEBUG, "DEBUG: ");
            name.put(INFO, "INFO: ");
            name.put(WARN, "WARN: ");
            name.put(ERROR, "ERROR: ");
            name.put(FATAL, "FATAL: ");
        }

        public boolean isEnabled(int level) {
            return true;
        }

        public void log(int level, String message) {
            System.err.print(name.get(level));
            System.err.println(message);
        }
    }


    public static void main(String[] arg) {
        System.out.print("main...\n");

        clientServer = new ClientServer();

    }

    public static class MyUserInfo implements UserInfo, UIKeyboardInteractive {

        String password = null;
//
//        private JPanel panelMain;
//        private JLabel JLabel1;
//        private JLabel JLabel2;
//        private JLabel JLabel3;
//        private JLabel JLabel4;
//        private JLabel JLabel5;
//        private JLabel JLabel6;
//        private JButton DCButton;


        public void setPassword(String passwd) {
            password = passwd;
        }

        public String getPassword() {
            System.out.print("...getPassword\n");
            return password;
        }

        // @Override
        public String getPassphrase() {
            System.out.print("...getPassphrase\n");
            return null;
        }

        public boolean promptPassphrase(String message) {
            System.out.print("...promptPassphrase\n");
            return true;
        }

        public boolean promptPassword(String message) {
            System.out.print("...promptPassword\n");
            return true;
        }

        /*
        public boolean promptYesNo(String str) {
            Object[] options = {"yes", "no"};
            int foo = JOptionPane.showOptionDialog(null,
                    str,
                    "Warning",
                    JOptionPane.DEFAULT_OPTION,
                    JOptionPane.WARNING_MESSAGE,
                    null, options, options[0]);
            return foo == 0;
        }
        */

        //this method responsible for that message, so just make it return true
        public boolean promptYesNo(String arg0) {
            System.out.print("...promptYesNo\n");
            return true;
        }

        public void showMessage(String message) {
            System.out.print("...showMessage\n");

            JOptionPane.showMessageDialog(null, message);
        }

        public String[] promptKeyboardInteractive(String destination,
                                                  String name,
                                                  String instruction,
                                                  String[] prompt,
                                                  boolean[] echo) {


            String[] response = new String[prompt.length];

            return response;
        }

    }
}




