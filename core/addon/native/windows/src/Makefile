DLL_DIST1 = ../../../../../samples/addons/observer/data
DLL_DIST2 = ../../../../../samples/webpages/addons/data
DLL_DIST3 = ../../../../../core/addons/arduino.js/data
CFLAGS  := -O2 -I../../macosx/src $(CFLAGS)

all: ArduinoBridge.dll

ArduinoBridge.dll: ArduinoBridge.obj
	link -dll -out:ArduinoBridge.dll -def:ArduinoBridge.def ArduinoBridge.obj
	cp -p ArduinoBridge.dll $(DLL_DIST1)
	cp -p ArduinoBridge.dll $(DLL_DIST2)
	cp -p ArduinoBridge.dll $(DLL_DIST3)

ArduinoBridge.obj: ArduinoBridge.cpp
	cl $(CFLAGS) -c ArduinoBridge.cpp -FoArduinoBridge.obj

clean:
	rm -f ArduinoBridge.dll ArduinoBridge.exp ArduinoBridge.lib ArduinoBridge.obj
