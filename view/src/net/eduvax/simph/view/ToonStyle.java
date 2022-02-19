/*
 * @file ToonStyle.java
 *
 * Copyright 2022 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
package net.eduvax.simph.view;

import com.jme3.material.Material;
import com.jme3.math.ColorRGBA;
import com.jme3.post.FilterPostProcessor;
import com.jme3.post.filters.CartoonEdgeFilter;
import com.jme3.renderer.Caps;
import com.jme3.scene.Geometry;
import com.jme3.scene.Node;
import com.jme3.scene.Spatial;
import com.jme3.scene.Spatial.CullHint;
import com.jme3.texture.Texture;
/**
 *
 */
public class ToonStyle implements ISceneComposition {
    private void makeToonish(Spatial spatial, View view){
        if (spatial instanceof Node){
            Node n = (Node) spatial;
            for (Spatial child : n.getChildren())
                makeToonish(child,view);
        }else if (spatial instanceof Geometry){
            Geometry g = (Geometry) spatial;
            Material m = g.getMaterial();
            if (m.getMaterialDef().getMaterialParam("UseMaterialColors") != null) {
                Texture t = view.getAssetManager().loadTexture("res/textures/ramp/toon.png");
                m.setTexture("ColorRamp", t);
                m.setBoolean("UseMaterialColors", true);
                m.setBoolean("VertexLighting", true);
            }
        }
    }

    @Override public void build(View view) {
        for (Spatial child : view.getRootNode().getChildren()) {
            makeToonish(child,view);
        }
        if (view.getRenderer().getCaps().contains(Caps.GLSL100)){
            FilterPostProcessor fpp=new FilterPostProcessor(view.getAssetManager());
            fpp.setNumSamples(4);
            CartoonEdgeFilter toon=new CartoonEdgeFilter();
            toon.setEdgeColor(ColorRGBA.Black);
            fpp.addFilter(toon);
            view.getViewPort().addProcessor(fpp);
        }
    }
    @Override public void update(float tpf) {
    }
}
