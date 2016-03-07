SRC=ArduinoBridge.c
dummy: so
	echo end

so: $(SRC)
	gcc --share -fPIC $(SRC) -o libArduinoBridge.so

