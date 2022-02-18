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

/**
 *
 */
public class Main {
    public static void main(String[] args) {
        View app = new View();
        app.setShowSettings(false);
        app.setDisplayFps(false);
        app.setDisplayStatView(false);
        app.setPauseOnLostFocus(false);
        AppSettings settings = new AppSettings(true);

        settings.setTitle("Simphonie Viewer");
        settings.setVSync(true);
        app.setSettings(settings);

        app.add(new DummyAvatar());
        app.add(new DummyScene());
        app.add(new DefaultLight());
        app.add(new ToonStyle());
        app.add(new TCPServer(17001));

        app.start();
    }
}
