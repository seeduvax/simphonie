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

import com.jme3.app.SimpleApplication;
import com.jme3.system.AppSettings;

import java.util.Vector;

/**
 *
 */
public class Main extends SimpleApplication {

    public Main() {
        _avatars.add(new DummyAvatar());
    }

    public static void main(String[] args) {
        Main app = new Main();
        app.setShowSettings(false);
        AppSettings settings = new AppSettings(true);
        settings.setTitle("Simphonie Viewer");
        settings.setResizable(true);
        app.setSettings(settings);
        app.start();
    }
    @Override public void simpleInitApp() {
        _globalScene.build(rootNode,assetManager,viewPort);
        for (IAvatar avatar: _avatars) {
            avatar.build(rootNode,assetManager,viewPort);
        }
    }
    @Override public void simpleUpdate(float tpf) {
        for (IAvatar avatar: _avatars) {
            avatar.update();
        }
    }

    private ISceneComposition _globalScene=new DefaultScene();
    private Vector<IAvatar> _avatars=new Vector<IAvatar>();
}
