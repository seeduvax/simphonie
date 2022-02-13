/*
 * @file DummyAvatar.java
 *
 * Copyright 2022 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
package net.eduvax.simph.view;

import com.jme3.asset.AssetManager;
import com.jme3.material.Material;
import com.jme3.math.ColorRGBA;
import com.jme3.scene.Geometry;
import com.jme3.scene.Node;
import com.jme3.scene.shape.Box;
import com.jme3.math.Transform;
import com.jme3.math.Vector3f;
import com.jme3.renderer.queue.RenderQueue;
import com.jme3.renderer.ViewPort; 

/**
 *
 */
public class DummyAvatar extends Avatar {
    private float _y=0;
    @Override public void build(Node parent, AssetManager assetManager, ViewPort viewPort) {
        Box b=new Box(1,1,1);
        Geometry geom = new Geometry("DummyAvatar", b);
        Material mat = new Material(assetManager, "Common/MatDefs/Light/Lighting.j3md");
        mat.setBoolean("UseMaterialColors",true);
        mat.setColor("Ambient",ColorRGBA.Gray);
        mat.setColor("Diffuse",ColorRGBA.Gray);
        mat.setColor("Specular",ColorRGBA.White);
        mat.setFloat("Shininess", 32f);
        geom.setMaterial(mat);
        geom.setShadowMode(RenderQueue.ShadowMode.CastAndReceive);
        parent.attachChild(geom);
        setSpatial(geom);
    }
    @Override public void update() {
        _y+=0.01;
        getSpatial().setLocalTranslation(0,_y,0);
    }
}
