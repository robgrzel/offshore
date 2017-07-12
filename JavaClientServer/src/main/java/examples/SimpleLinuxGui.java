package examples;

import com.jcraft.jsch.*;

import javax.swing.*;
import javax.swing.tree.DefaultMutableTreeNode;
import java.util.Vector;

public class SimpleLinuxGui {
    static private SimpleLinuxGui slg;
    String sshremotedir = "/home/student";
    String username = "student";
    String hostname = "des02.eti.pg.gda.pl";
    String password = "password";


    public static void cargarRTree(String remotePath, DefaultMutableTreeNode parent) {
//todo: change "/" por remote file.separator
        JSch jsch = new JSch();
        MyUserInfo ui = new MyUserInfo();
        ui.setPassword(slg.password);
        Session session = null;
        boolean isSession = false;
        while (!isSession)
            try {
                session = jsch.getSession(slg.username, slg.hostname, 22);
                isSession = true;
            } catch (JSchException e) {
                e.printStackTrace();
            }

        session.setConfig("PreferredAuthentications", "publickey,keyboard-interactive,password");
        session.setPassword(slg.password);
        session.setUserInfo(ui);
        while (!session.isConnected()) {
            try {
                session.connect();
            } catch (JSchException e) {
                e.printStackTrace();
            }
        }
        Channel channel = null;
        if (session.isConnected()) {
            try {
                channel = session.openChannel("sftp");
            } catch (JSchException e) {
                e.printStackTrace();
            }
            try {
                channel.connect();
            } catch (JSchException e) {
                e.printStackTrace();
            }
        }
        ChannelSftp sftpChannel = (ChannelSftp) channel;
        Vector<ChannelSftp.LsEntry> list = null; // List source directory structure.
        try {
            list = sftpChannel.ls(remotePath);
        } catch (SftpException e) {
            e.printStackTrace();
        }

        System.out.println(list);
//Vector<ChannelSftp.LsEntry> list = sftpChannel
        for (ChannelSftp.LsEntry oListItem : list) { // Iterate objects in the list to get file/folder names.
            DefaultMutableTreeNode node = new DefaultMutableTreeNode(oListItem.getFilename());
            if (!oListItem.getAttrs().isDir()) { // If it is a file (not a directory).
                parent.add(node); // add as a child node
                System.out.print("");
            } else {
                if (!".".equals(oListItem.getFilename()) && !"..".equals(oListItem.getFilename())) {
                    parent.add(node); // add as a child node
                    cargarRTree(remotePath + "/" + oListItem.getFilename(), node); // call again for the subdirectory
                }
            }
        }
    }


    public static void main(String[] args) {
        slg = new SimpleLinuxGui();
        JFrame jf = new JFrame();


        DefaultMutableTreeNode nroot = new DefaultMutableTreeNode(slg.sshremotedir);
        cargarRTree(slg.sshremotedir, nroot);
// TODO Auto-generated catch block
        JTree yourJTree = new JTree(nroot);
        jf.add(yourJTree);
        jf.setSize(640, 480);
        jf.setVisible(true);
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