/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

const {Cc,Ci,Cr} = require("chrome");
var arduino = require("arduino.js");
var timers = require("timers");

var pins = [
    {number:13, timerid: -1},
    {number:12, timerid: -1},
    {number:11, timerid: -1},
    {number:10, timerid: -1},
    {number:9, timerid: -1},
    {number:8, timerid: -1},
    {number:7, timerid: -1},
    {number:6, timerid: -1},
    {number:5, timerid: -1},
    {number:4, timerid: -1},
    {number:3, timerid: -1},
    {number:2, timerid: -1}
];

arduino.open("/dev/cu.usbmodemfd121");
for (var i = 0, n = pins.length; i < n; i++) {
    var pin = pins[i];
    arduino.pinMode(pin.number, true);
}

function light(index) {
    for (var i = index, n = index+4; i < n; i++) {
        var pin = pins[i];
        if (pin.timerid == -1) {
            lightOn(pin);
            break;
        }
    }
}

function lightOn(pin) {
    arduino.analogWrite(pin.number, 255);
    pin.timerid = timers.setTimeout(lightOff, 0, pin);
}

function lightOff(pin) {
    arduino.analogWrite(pin.number, 0);
    pin.timerid = -1;
}

var Observer = {
    init: function() {
        var observerService = Cc["@mozilla.org/observer-service;1"].getService(Ci.nsIObserverService);
//      observerService.addObserver(this, "quit-application-requested", false);  
        observerService.addObserver(this, "*", false);  
    },
    observe : function(aSubject, aTopic, aData) {
        if (aTopic == "visited-status-resolution") {
        return;
        }
        if (aTopic.indexOf("http") == 0) {
            light(0);
        } else if (aTopic.indexOf("cookie") == 0) {
            light(4);
        } else {
            light(8);
        }
        console.log("---------------------------");
        console.log(aTopic);
        if (null != aSubject) {
            console.log(aSubject.QueryInterface(Ci.nsISupportsString));
        }
        if (null != aData) {
            console.log(aData);
        }
    }
}
Observer.init();