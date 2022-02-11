/*
 * @file ISceneComposition.java
 *
 * Copyright 2022 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
package net.eduvax.simph.view;

/**
 *
 */
public interface ISceneComposition {
    void build(com.jme3.scene.Node node,
                com.jme3.asset.AssetManager assetManager,
                com.jme3.renderer.ViewPort viewPort);
}
