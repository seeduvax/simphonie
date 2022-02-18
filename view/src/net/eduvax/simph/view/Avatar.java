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

import com.jme3.math.Quaternion;
import com.jme3.math.Vector3f;
import java.util.LinkedList;

/**
 *
 */
public abstract class Avatar implements IAvatar {
    class AttrChange {
        public String _name;
        public double _value;
        public AttrChange(String name, double value) {
            _name=name;
            _value=value;
        }
    }
    @Override public void setTime(double time) { 
        _time=time;
    }
    
    @Override public void setLocation(double x, double y, double z) {
        _location=new Vector3f((float)x,(float)y,(float)z);
    }

    @Override public void setAttitude(double w,double x, double y, double z) {
        _attitude=new Quaternion((float)w,(float)x,(float)y,(float)z);
    }

    @Override public synchronized void setAttribute(String attrName,double value) {
        _attrChangesQueue.add(new AttrChange(attrName,value));
    }
  
     
    protected synchronized Vector3f getLocation() {
        return _location;
    } 
    protected synchronized Quaternion getAttitude() {
        return _attitude;
    }

    protected synchronized AttrChange pollAttrChange() {
        return _attrChangesQueue.poll();
    }

    private double _time;
    private Vector3f _location=new Vector3f();
    private Quaternion _attitude=new Quaternion();
    private LinkedList<AttrChange> _attrChangesQueue=new LinkedList<AttrChange>();
}
