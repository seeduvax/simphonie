/*
 * @file Main.java
 *
 * Copyright 2022 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
package net.eduvax.simph.view;

import com.jme3.system.AppSettings;
import java.awt.Dimension;
import java.awt.Toolkit;
import java.util.StringTokenizer;

/**
 *
 */
public class Main {
    public static void main(String[] args) {
        View app = new View();
        AppSettings settings = app.getSettings();
        boolean toon=false;
        int tcpPort=0;
        for(String arg: args) {
            if ("-f".equals(arg)) {
                Toolkit tk=Toolkit.getDefaultToolkit();
                Dimension d=tk.getScreenSize();
                settings.setResolution(d.width,d.height);
                settings.setFullscreen(true);
            }
            else if ("-ts".equals(arg)) {
                toon=true;
            }
            else if (arg.startsWith("-tcp")) {
                tcpPort=10001;
                StringTokenizer st=new StringTokenizer(arg,"=");
                st.nextToken();
                if (st.hasMoreTokens()) {
                    tcpPort=Integer.parseInt(st.nextToken());
                }
            }
        }

        app.add(new DummyAvatar());
        app.add(new DummyAvatar());
        app.add(new DummyScene());
        app.add(new DefaultLight());
        if (toon) {
            app.add(new ToonStyle());
        }
        if (tcpPort>0) {
            app.add(new TCPServer(tcpPort));
        }

        app.start();
    }
}
