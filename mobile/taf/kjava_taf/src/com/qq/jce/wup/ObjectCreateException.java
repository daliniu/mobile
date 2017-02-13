package com.qq.jce.wup;

public class ObjectCreateException extends Exception {

    ObjectCreateException(Exception ex) {
        super(ex.toString());
    }


}
