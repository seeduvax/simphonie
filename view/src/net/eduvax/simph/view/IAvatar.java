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
public interface IAvatar {
    void setTime(double time);
    void setLocation(double x, double y, double z);
    void setAttitude(double w,double x,double y, double z);
    void setAttribute(String attrName,double value);
}
