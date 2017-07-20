My desktop OS is Ubuntu14 LTS 64bit version, and linux and android were 
compiled and built on it. The windows version was built on Windows7. Therefore,
there are 3 prebuilt versions now. I do not have any Mac machine, so I
could not build Mac or IOS versions.

1. Directory structure

bin_prebuilt_apk 	<= android version
bin_prebuilt_linux64	<= 64bit linux version
bin_prebuilt_windows	<= windows7 version

caller			<= The program that is calling the native function.
natvie_extension	<= Native code

[caller main source]

	caller/Source/Main.hx

[native_extension main source]

	native_extension/project/common/ExternalInterface.cpp

2. How to make ndll

1) Go to native_extension/project directory
2) make linux 	# for linux64 version
3) make android # for android version
4) make windows # for windows version but should be executed on Windows OS
5) The ndll files will be located in native_extension/ndll directory

* If you do not have make, please see the Makefile, and you can know the 
  commands.

3. How to make caller

1) Go to caller directory
2) make linux	# for linux64 version
3) make android	# for android version
4) make windows	# for windows version but should be executed on Windows OS
5) Copy ndll files into where they are supposed to be.

	[android]

	from: native_extension/ndll/Android/native_extension.so
	to: caller/Export/android/release/bin/app/src/main/jniLibs/armeabi

	[linux64]

	from: native_extension/ndll/Linux64/native_extension.ndll
	to: caller/Export/linux64/cpp/release/bin

	[windows]

	from: native_extension/ndll/Windows/native_extension-19.ndll
	to: caller/Export/windows/cpp/release/bin

Thank you very much for your time!

