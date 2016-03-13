/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

const {Cu, Ci, Cc} = require("chrome");
const { ctypes } = Cu.import("resource://gre/modules/ctypes.jsm");
const self = require("sdk/self");
const url = require("sdk/url");

const wm = Cc["@mozilla.org/appshell/window-mediator;1"].getService(Ci.nsIWindowMediator);
const mainWindow = wm.getMostRecentWindow("navigator:browser");
const dylibURL = require("sdk/self").data.url(ctypes.libraryName("ArduinoBridge"));
const dylibPATH = url.toFilename(dylibURL).toString();
const dylib = ctypes.open(dylibPATH);  

var openArduino = dylib.declare("openArduino",       /* function name */  
                          ctypes.default_abi,    /* ABI type */  
                          ctypes.int32_t,       /* return type */  
                          ctypes.char.ptr, /* parameter */
                          new ctypes.PointerType(ctypes.char.ptr)
                          );
                          
var pinMode = dylib.declare("pinMode",
                          ctypes.default_abi,
                          ctypes.int32_t,
                          ctypes.int32_t,
                          ctypes.bool,
                          new ctypes.PointerType(ctypes.char.ptr)
                          );

var digitalWrite = dylib.declare("digitalWrite",
                          ctypes.default_abi,
                          ctypes.int32_t,
                          ctypes.int32_t,
                          ctypes.int32_t,
                          new ctypes.PointerType(ctypes.char.ptr)
                          );

var digitalRead = dylib.declare("digitalRead",
                          ctypes.default_abi,
                          ctypes.int32_t,
                          ctypes.int32_t,
                          new ctypes.PointerType(ctypes.int32_t),
                          new ctypes.PointerType(ctypes.char.ptr)
                          );
                          
var analogWrite = dylib.declare("analogWrite",
                          ctypes.default_abi,
                          ctypes.int32_t,
                          ctypes.int32_t,
                          ctypes.int32_t,
                          new ctypes.PointerType(ctypes.char.ptr)
                          );

var analogRead = dylib.declare("analogRead",
                          ctypes.default_abi,
                          ctypes.int32_t,
                          ctypes.int32_t,
                          new ctypes.PointerType(ctypes.int32_t),
                          new ctypes.PointerType(ctypes.char.ptr)
                          );

var pulse = dylib.declare("pulse",
                          ctypes.default_abi,
                          ctypes.int32_t,
                          ctypes.int32_t,
                          ctypes.int32_t,
                          ctypes.int32_t,
                          new ctypes.PointerType(ctypes.char.ptr)
                          );

var delayMicroseconds = dylib.declare("delayMicroseconds",
                          ctypes.default_abi,
                          ctypes.int32_t,
                          ctypes.int32_t,
                          new ctypes.PointerType(ctypes.char.ptr)
                          );

var closeArduino = dylib.declare("closeArduino",
                          ctypes.default_abi,
                          ctypes.int32_t
                          );
                          

var arduino = {};
arduino.open = function(portname) {
    var err = ctypes.char.ptr();
    if ( -1 == openArduino(portname, err.address()) ) {
        throw err.readString(); 
    }
}

arduino.pinMode = function(pin, isOutputMode) {
    var err = ctypes.char.ptr();
    if ( -1 == pinMode(pin, isOutputMode, err.address()) ) {
        throw err.readString(); 
    }
}

arduino.digitalWrite = function(pin, value) {
    var err = ctypes.char.ptr();
    if ( -1 == digitalWrite(pin, value, err.address()) ) {
        throw err.readString(); 
    }
}

arduino.digitalRead = function(pin) {
    var value = ctypes.int32_t(-1);
    var err = ctypes.char.ptr();
    if ( -1 == digitalRead(pin, value.address(), err.address()) ) {
       throw err.readString(); 
    }
    return value.address().contents;
}

arduino.analogWrite = function(pin, value) {
    var err = ctypes.char.ptr();
    if ( -1 == analogWrite(pin, value, err.address()) ) {
       throw err.readString(); 
    }
}

arduino.analogRead = function(pin) {
    var value = ctypes.int32_t(-1);
    var err = ctypes.char.ptr();
    if ( -1 == analogRead(pin, value.address(), err.address()) ) {
       throw err.readString(); 
    }
    return value.address().contents;
}

arduino.pulse = function(pin, ontime, offtime) {
    var err = ctypes.char.ptr();
    if ( -1 == pulse(pin, ontime, offtime, err.address()) ) {
       throw err.readString(); 
    }
}

arduino.delayMicroseconds = function(value) {
    var err = ctypes.char.ptr();
    if ( -1 == delayMicroseconds(value, err.address()) ) {
       throw err.readString(); 
    }
}

    arduino.close = function() {
    closeArduino();
//  dylib.close();
}

mainWindow.gBrowser.addEventListener("readystatechange", function(e) {
  let doc = e.originalTarget;
  if (doc.defaultView.frameElement) {
    return;
  }
  if (doc.readyState != "interactive") {
    return;
  }
  if(doc.hasOwnProperty("wrappedJSObject")){
    doc.wrappedJSObject.arduino = Cu.cloneInto(
      arduino, doc, {cloneFunctions: true});
  }
},true);
