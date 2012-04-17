const xulApp = require("api-utils/xul-app");
const HAS_DOCUMENT_ELEMENT_INSERTED =
        xulApp.versionInRange(xulApp.platformVersion, "2.0b6", "*");
const ON_CONTENT = HAS_DOCUMENT_ELEMENT_INSERTED ? 'document-element-inserted' :
                   'content-document-global-created';
const observers = require("api-utils/observer-service");
const arduino = require("arduino.js");

observers.add(ON_CONTENT, function(dom) {
	let window = HAS_DOCUMENT_ELEMENT_INSERTED ? dom.defaultView : dom;
	if (null == window) {
		return;
	}
	dom.wrappedJSObject.arduino = arduino;
});