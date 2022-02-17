/*
 * @file View.java
 *
 * Copyright 2022 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
package net.eduvax.simph.view;
import com.jme3.app.SimpleApplication;
import com.jme3.asset.AssetManager;
import com.jme3.input.ChaseCamera;
import com.jme3.renderer.Renderer;
import com.jme3.renderer.ViewPort;
import com.jme3.scene.Node;
import com.jme3.scene.Spatial;
import java.util.Vector;

/**
 *
 */
public class View extends SimpleApplication {
    public View() {
    }

    public void add(ISceneComposition comp) {
        _sceneElements.add(comp);
    }
    
    @Override public void simpleInitApp() {
        for (ISceneComposition obj: _sceneElements) {
            obj.build(this);
        }
setChase("dummy");
    }

    public void setChase(String name) {
        Spatial s=rootNode.getChild(name);
        if (s!=null) {
            flyCam.setEnabled(false);
            ChaseCamera chaseCam=new ChaseCamera(cam,inputManager);
            s.removeControl(ChaseCamera.class);
            s.addControl(chaseCam);
        }
        else {
            flyCam.setEnabled(true);
        }
    }

    public Node getRootNode() {
        return rootNode;
    }
    @Override public void simpleUpdate(float tpf) {
        for (ISceneComposition c: _sceneElements) {
            c.update();
        }
    }
    
    private Vector<ISceneComposition> _sceneElements=
                                           new Vector<ISceneComposition>();
}