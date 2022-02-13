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
import com.jme3.scene.Geometry;
import com.jme3.scene.Node;
import com.jme3.scene.shape.Box;
import com.jme3.util.SkyFactory;
import com.jme3.light.DirectionalLight;
import com.jme3.math.Transform;
import com.jme3.math.Vector3f;
import com.jme3.renderer.queue.RenderQueue;
import com.jme3.renderer.ViewPort; 
import com.jme3.post.FilterPostProcessor;
import com.jme3.shadow.DirectionalLightShadowFilter;
import com.jme3.shadow.DirectionalLightShadowRenderer;

/**
 *
 */
public class DefaultScene implements ISceneComposition {
    @Override public void build(Node parent, AssetManager assetManager,
                                ViewPort viewPort) {
        DirectionalLight sun = new DirectionalLight();
        sun.setColor(ColorRGBA.White);
        sun.setDirection(new Vector3f(-.5f,-.5f,-.5f).normalizeLocal());
        parent.addLight(sun);

        Box b = new Box(10000, 0.5f, 10000);
        Geometry geom = new Geometry("Ground", b);
        Transform t=new Transform(new Vector3f(0,-0.5f,0));
        geom.setLocalTransform(t);
        Material mat = new Material(assetManager, "Common/MatDefs/Light/Lighting.j3md");
        mat.setBoolean("UseMaterialColors",true);
        mat.setColor("Ambient",ColorRGBA.Green);
        mat.setColor("Diffuse",ColorRGBA.Green);
        mat.setColor("Specular",ColorRGBA.Yellow);
        mat.setFloat("Shininess", 64f);
        geom.setShadowMode(RenderQueue.ShadowMode.CastAndReceive);
        geom.setMaterial(mat);
        parent.attachChild(geom);
        parent.attachChild(SkyFactory.createSky(assetManager,
                          "res/textures/sky/skysphere1.jpg", 
                           SkyFactory.EnvMapType.EquirectMap));

        final int SHADOWMAP_SIZE=1024;
        DirectionalLightShadowRenderer dlsr = new DirectionalLightShadowRenderer(assetManager, SHADOWMAP_SIZE, 3);
        dlsr.setLight(sun);
        viewPort.addProcessor(dlsr);

        DirectionalLightShadowFilter dlsf = new DirectionalLightShadowFilter(assetManager, SHADOWMAP_SIZE, 3);
        dlsf.setLight(sun);
        dlsf.setEnabled(true);
        FilterPostProcessor fpp = new FilterPostProcessor(assetManager);
        fpp.addFilter(dlsf);
        viewPort.addProcessor(fpp);
    }
}
