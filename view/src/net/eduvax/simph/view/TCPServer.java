/*
 * @file TCPServer.java
 *
 * Copyright 2022 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
package net.eduvax.simph.view;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.StringTokenizer;
import java.util.Vector;

/**
 *
 */
public class TCPServer implements IAvatarsHandler, Runnable {
    public TCPServer(int port) {
        try {
            _server=new ServerSocket(port);
            start();
        }
        catch (IOException ex) {
System.err.println("Can't start server on port "+port+": "+ex);
ex.printStackTrace();
        }
    }

    public void start() {
        Thread th=new Thread(this);
        th.start();
    }

    @Override public void run() {
        _run=true;
        while(_run) {
            try {
                Socket socket=_server.accept();
                Thread th=new Thread(new SocketHandler(socket));
                th.start();
            }
            catch (IOException ex) {
System.err.println("Incoming connection error: "+ex);
ex.printStackTrace();
            }
        }
    }

    public void stop() {
        _run=false;
    }

    @Override public void setAvatars(Vector<IAvatar> avatars) {
        _avatars=avatars;
    }

    
    private boolean _run=false;
    private Vector<IAvatar> _avatars=new Vector<IAvatar>();
    ServerSocket _server;
    
    class SocketHandler implements Runnable {
        public SocketHandler(Socket socket) {
            _socket=socket;
            _avatar=_avatars.elementAt(0);
        }

        @Override public void run() {
            try {
                BufferedReader in=new BufferedReader(new InputStreamReader(_socket.getInputStream()));
                String line=in.readLine();
                while (line!=null) {
                    StringTokenizer st=new StringTokenizer(line," ");
                    String cmd=st.nextToken();
                    if ("avatar".equals(cmd)) {
                        _avatar=_avatars.elementAt(Integer.parseInt(st.nextToken()));
                    }
                    if ("location".equals(cmd)) {
                        double x=Double.parseDouble(st.nextToken());
                        double y=Double.parseDouble(st.nextToken());
                        double z=Double.parseDouble(st.nextToken());
                        _avatar.setLocation(x,y,z); 
                    }
                    if ("attitude".equals(cmd)) {
                        double w=Double.parseDouble(st.nextToken());
                        double x=Double.parseDouble(st.nextToken());
                        double y=Double.parseDouble(st.nextToken());
                        double z=Double.parseDouble(st.nextToken());
                        _avatar.setAttitude(w,x,y,z); 
                    }
                    if ("attribute".equals(cmd)) {
                        String name=st.nextToken();
                        double value=0;
                        if (st.hasMoreTokens()) {
                            value=Double.parseDouble(st.nextToken());
                        }
                        _avatar.setAttribute(name,value);
                    }
                    line=in.readLine();
                }
            }
            catch (java.io.IOException ex) {
System.err.println("Read error on socket read: "+ex);
ex.printStackTrace();
            }
        }

        private IAvatar _avatar;
        private Socket _socket;
    }
}
