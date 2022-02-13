/*
 * @file Avatar.java
 *
 * Copyright 2022 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
package net.eduvax.simph.view;

import com.jme3.input.ChaseCamera;
import com.jme3.scene.Spatial;
import com.jme3.scene.control.Control;

/**
 *
 */
public abstract class Avatar implements IAvatar {
    private Spatial _spatial=null;
    private Control _control=null;
    protected void setSpatial(Spatial spatial) {
        _spatial=spatial;
    }
    protected Spatial getSpatial() {
        return _spatial;
    }
    @Override public void setChaseCamera(ChaseCamera cam) {
        if (_control!=null) {
            _spatial.removeControl(_control);
        }
        _spatial.addControl(cam);
        _control=cam;
    }
}
