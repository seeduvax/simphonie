/*
 * @file DefaultScene.java
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
import com.jme3.math.Transform;
import com.jme3.math.Vector3f;
import com.jme3.renderer.ViewPort; 
import com.jme3.scene.Geometry;
import com.jme3.scene.Node;
import com.jme3.scene.shape.Box;
import com.jme3.util.SkyFactory;

/**
 *
 */
public class DummyScene implements ISceneComposition {
    @Override public void build(View view) {
        Node parent=view.getRootNode();
        AssetManager assetManager=view.getAssetManager();
        ViewPort viewPort=view.getViewPort();

        Geometry ground = new Geometry("Ground", new Box(10000, 0.5f, 10000));
        Transform t=new Transform(new Vector3f(0,-0.5f,0));
        ground.setLocalTransform(t);
        Material mat = new Material(assetManager, "Common/MatDefs/Light/Lighting.j3md");
        mat.setBoolean("UseMaterialColors",true);
        mat.setColor("Ambient",ColorRGBA.Green);
        mat.setColor("Diffuse",ColorRGBA.Green);
        mat.setColor("Specular",ColorRGBA.Yellow);
        mat.setFloat("Shininess", 64f);
        ground.setMaterial(mat);
        parent.attachChild(ground);
        parent.attachChild(SkyFactory.createSky(assetManager,
                          "res/textures/sky/skysphere1.jpg", 
                           SkyFactory.EnvMapType.EquirectMap));


    }
    @Override public void update() {
    }
}
