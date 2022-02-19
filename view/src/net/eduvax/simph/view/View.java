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
import com.jme3.input.KeyInput;
import com.jme3.input.controls.ActionListener;
import com.jme3.input.controls.KeyTrigger;
import com.jme3.renderer.Renderer;
import com.jme3.renderer.ViewPort;
import com.jme3.scene.Node;
import com.jme3.scene.Spatial;
import com.jme3.system.AppSettings;
import java.util.Vector;

/**
 *
 */
public class View extends SimpleApplication {
    public View() {
        setShowSettings(false);
        setDisplayFps(false);
        setDisplayStatView(false);
        setPauseOnLostFocus(false);
        setSettings(_settings);
        _settings.setTitle("Simphonie viewer");
        _settings.setVSync(true);
        _settings.setResolution(1280,768);
    }

    public AppSettings getSettings() {
        return _settings;
    }

    public void add(ISceneComposition comp) {
        _sceneElements.add(comp);
        if (comp instanceof IAvatar) {
            _avatars.add((IAvatar)comp);
        }
    }

    public void add(IAvatarsHandler handler) {
        _avatarsHandlers.add(handler);
    }
    
    @Override public void simpleInitApp() {
        for (ISceneComposition obj: _sceneElements) {
            obj.build(this);
        }
        for (IAvatarsHandler handler: _avatarsHandlers) {
            handler.setAvatars(_avatars);
        }
        setChase(_selAvatar);
        inputManager.addMapping("nextCam",new KeyTrigger(KeyInput.KEY_PGUP));
        inputManager.addListener(new ActionListener() {
            @Override 
            public void onAction(String name, boolean pressed, float tpf) {
                if (pressed) nextCam();
            }
        },"nextCam");
        inputManager.addMapping("prevCam",new KeyTrigger(KeyInput.KEY_PGDN));
        inputManager.addListener(new ActionListener() {
            @Override 
            public void onAction(String name, boolean pressed, float tpf) {
                if (pressed) prevCam();
            }
        },"prevCam");
        
    }

    public void nextCam() {
        _selAvatar++;
        if (_selAvatar>_sceneElements.size()) {
            _selAvatar=0;
        }
        setChase(_selAvatar);
    }

    public void prevCam() {
        _selAvatar--;
        if (_selAvatar<0) {
            _selAvatar=_sceneElements.size();
        }
        setChase(_selAvatar);
    }

    

    public void setChase(Spatial s) {
        if (s!=null) {
            flyCam.setEnabled(false);
            ChaseCamera chaseCam=new ChaseCamera(cam,inputManager);
            chaseCam.setMinVerticalRotation((float)-Math.PI);
            s.removeControl(ChaseCamera.class);
            s.addControl(chaseCam);
        }
        else {
            flyCam.setEnabled(true);
        }
    }
    public void setChase(int i) {
        Spatial s=null;
        try {
            s=rootNode.getChildren().get(i);
        }
        catch (IndexOutOfBoundsException ex) {
            // don't care out of bounds, 
            // it's a kind of reset...
        }
        setChase(s);
    }
    public void setChase(String name) {
        setChase(rootNode.getChild(name));
    }

    public Node getRootNode() {
        return rootNode;
    }
    @Override public void simpleUpdate(float tpf) {
        for (ISceneComposition c: _sceneElements) {
            c.update(tpf);
        }
    }
    
    private Vector<ISceneComposition> _sceneElements=
                                           new Vector<ISceneComposition>();
    private Vector<IAvatar> _avatars=new Vector<IAvatar>();
    private Vector<IAvatarsHandler> _avatarsHandlers=new Vector<IAvatarsHandler>();
    private AppSettings _settings=new AppSettings(true);
    private int _selAvatar=0;
}
