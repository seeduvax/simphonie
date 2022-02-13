/*
 * @file IAvatar.java
 *
 * Copyright 2022 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
package net.eduvax.simph.view;
import com.jme3.input.ChaseCamera;

/**
 *
 */
public interface IAvatar extends ISceneComposition {
    public void update();
    public void setChaseCamera(ChaseCamera cam);
}
