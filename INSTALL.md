# Install TuxGuitar

You can find ready to use installation packages for Linux, Windows, MacOS, FreeBSD and Android on

[https://github.com/helge17/tuxguitar/releases/](https://github.com/helge17/tuxguitar/releases/)

# TuxGuitar Build Instructions

## Warning

The following instructions may not be complete. For hints and workarounds needed to build TuxGuitar, see the script

```sh
misc/build_tuxguitar_from_source.sh
```

## Prerequisites

- JDK 7 or higher
- Maven 3.3 or higher
- Fluidsynth (optional)
- JACK (optional)
- Eclipse SWT 4.13

## Build on Debian/Ubuntu Linux

### Install Prerequisites

```sh
$ sudo apt install wget unzip git build-essential default-jdk maven libwebkit2gtk-4.0-37 libfluidsynth-dev libjack-jackd2-dev libasound2-dev libgtk-3-dev liblilv-dev libsuil-dev qtbase5-dev
```

### Download and install SWT for Linux

```sh
$ wget https://archive.eclipse.org/eclipse/downloads/drops4/R-4.13-201909161045/swt-4.13-gtk-linux-x86_64.zip
$ mkdir swt-4.13-gtk-linux-x86_64
$ cd swt-4.13-gtk-linux-x86_64
$ unzip ../swt-4.13-gtk-linux-x86_64.zip
$ mvn install:install-file -Dfile=swt.jar -DgroupId=org.eclipse.swt -DartifactId=org.eclipse.swt.gtk.linux.x86_64 -Dpackaging=jar -Dversion=4.13
$ cd ..
```

### Get the TuxGuitar sources

```sh
$ git clone https://github.com/helge17/tuxguitar.git
$ cd tuxguitar
```

### Hacks

The Debian version must start with a number:

```sh
$ sed -i "s/SNAPSHOT/9.99-snapshot/" desktop/build-scripts/tuxguitar-linux-swt-x86_64-deb/src/resources/DEBIAN/control
```

For the (outdated) VST plugin you need some additional header files:

```sh
$ mkdir desktop/build-scripts/native-modules/tuxguitar-synth-vst-linux-x86_64/include
$ cd desktop/build-scripts/native-modules/tuxguitar-synth-vst-linux-x86_64/include
$ for hfile in aeffect.h aeffectx.h vstfxstore.h; do
    wget https://raw.githubusercontent.com/R-Tur/VST_SDK_2.4/master/pluginterfaces/vst2.x/$hfile
  done
$ cd -
```

### Build and install

```sh
$ cd desktop/build-scripts/tuxguitar-linux-swt-x86_64-deb
$ mvn -e clean verify -P native-modules
$ sudo dpkg -i target/tuxguitar-*.deb
```

### Start TuxGuitar

Now you can start TuxGuitar from your Desktop menu or on the command line with

```sh
$ tuxguitar
```

## Generic GNU/Linux

On Non-Debian-based systems install the prerequisites using your package manager. Then download and install SWT, download the TuxGuitar sources and the VST header files as described for Debian above.

### Build and Start TuxGuitar

```sh
$ cd desktop/build-scripts/tuxguitar-linux-swt-x86_64
$ mvn -e clean verify -P native-modules
```

```sh
$ cd target/tuxguitar-*
$ ./tuxguitar.sh
```

## Build for Windows on Linux

The Windows version is cross compiled on Ubuntu/Debian with [Mingw-w64](https://mingw-w64.org/).

### Install Prerequisites

```sh
$ sudo apt install default-jdk maven gcc-mingw-w64-x86-64 g++-mingw-w64-i686-win32
```

### Download and install SWT for Windows

```sh
$ wget https://archive.eclipse.org/eclipse/downloads/drops4/R-4.13-201909161045/swt-4.13-win32-win32-x86_64.zip
$ mkdir swt-4.13-win32-win32-x86_64
$ cd swt-4.13-win32-win32-x86_64
$ unzip ../swt-4.13-win32-win32-x86_64.zip
$ mvn install:install-file -Dfile=swt.jar -DgroupId=org.eclipse.swt -DartifactId=org.eclipse.swt.win32.win32.x86_64 -Dpackaging=jar -Dversion=4.13
$ cd ..
```

### Get the TuxGuitar sources

Same as for Debian (see above).

### Hacks

Download the VST header files:

```sh
$ mkdir desktop/build-scripts/native-modules/tuxguitar-synth-vst-windows-x86/include
$ cd desktop/build-scripts/native-modules/tuxguitar-synth-vst-windows-x86/include
$ for hfile in aeffect.h aeffectx.h vstfxstore.h; do
    wget https://raw.githubusercontent.com/R-Tur/VST_SDK_2.4/master/pluginterfaces/vst2.x/$hfile
  done
$ cd -
```

### Build and Start TuxGuitar

As we are building the Windows version on Linux, we explicitly deactivate the Linux profile and select the Windows profile manually to avoid confusion.

```sh
$ cd desktop/build-scripts/tuxguitar-windows-swt-x86_64
$ mvn -e clean verify -P native-modules -P -platform-linux-x86_64 -P platform-windows-all
```

The Windows application is now located in the `desktop/build-scripts/tuxguitar-windows-swt-x86_64/target/tuxguitar-SNAPSHOT-windows-swt-x86_64` folder. Copy it to your Windows machine.

To start TuxGuitar you need a Java Runtime Environment. You can get the one from [portableapps.com](https://portableapps.com/apps/utilities/OpenJDK64) and extract it to a subfolder named `jre`. Then you should be able to start TuxGuitar by double-clicking on `tuxguitar.exe` or `tuxguitar.bat`.

## Build on MacOS

On MacOS you need to download and install [Homebrew](https://brew.sh) to build TuxGuitar.

### Install Prerequisites

```sh
$ brew install openjdk maven wget
```

### Download and install SWT for MacOS

```sh
$ wget https://archive.eclipse.org/eclipse/downloads/drops4/R-4.13-201909161045/swt-4.13-cocoa-macosx-x86_64.zip
$ mkdir swt-4.13-cocoa-macosx-x86_64
$ cd swt-4.13-cocoa-macosx-x86_64
$ unzip ../swt-4.13-cocoa-macosx-x86_64.zip
$ mvn install:install-file -Dfile=swt.jar -DgroupId=org.eclipse.swt -DartifactId=org.eclipse.swt.cocoa.macosx.x86_64 -Dpackaging=jar -Dversion=4.13
$ cd ..
```

### Get the TuxGuitar sources

Same as for Debian (see above).

### Build and Start TuxGuitar

```sh
$ cd desktop/build-scripts/tuxguitar-macosx-swt-cocoa-x86_64
$ mvn -e clean verify
```

The application is now located in the `desktop/build-scripts/tuxguitar-macosx-swt-cocoa-x86_64/target/tuxguitar-SNAPSHOT-macosx-swt-cocoa-x86_64.app` folder. Start TuxGuitar by double-clicking on the folder.

## Build on FreeBSD

### Install Prerequisites

```sh
$ sudo pkg install openjdk11 alsa-plugins maven swt gcc gmake fluidsynth wget
```

### Install SWT for FreeBSD

On FreeBSD we use SWT from the OS to build and run TuxGuitar. FreeBSD 13.2 comes with SWT version 4.21.

```sh
mvn install:install-file -Dfile=/usr/local/share/java/classes/swt.jar -DgroupId=org.eclipse.swt -DartifactId=org.eclipse.swt.gtk.freebsd.x86_64 -Dpackaging=jar -Dversion=4.21
```

### Get the TuxGuitar sources

Same as for Debian (see above).

### Build and Start TuxGuitar

```sh
$ cd desktop/build-scripts/tuxguitar-freebsd-swt-x86_64
$ mvn -e clean verify -P native-modules
```

```sh
$ cd target/tuxguitar-*
$ ./tuxguitar.sh
```
