# Ignition Gazebo : A Robotic Simulator

[![Bitbucket open issues](https://img.shields.io/bitbucket/issues-raw/ignitionrobotics/ign-gazebo.svg)](https://bitbucket.org/ignitionrobotics/ign-gazebo/issues)
[![Bitbucket open pull requests](https://img.shields.io/bitbucket/pr-raw/ignitionrobotics/ign-gazebo.svg)](https://bitbucket.org/ignitionrobotics/ign-gazebo/pull-requests)
[![Discourse topics](https://img.shields.io/discourse/https/community.gazebosim.org/topics.svg)](https://community.gazebosim.org)
[![Hex.pm](https://img.shields.io/hexpm/l/plug.svg)](https://www.apache.org/licenses/LICENSE-2.0)

Build | Status
-- | --
Test coverage | [![codecov](https://codecov.io/bb/ignitionrobotics/ign-gazebo/branch/default/graph/badge.svg)](https://codecov.io/bb/ignitionrobotics/ign-gazebo)  
Ubuntu Bionic | [![Build Status](https://build.osrfoundation.org/buildStatus/icon?job=ignition_gazebo-ci-default-bionic-amd64)](https://build.osrfoundation.org/job/ignition_gazebo-ci-default-bionic-amd64)  
Homebrew      | [![Build Status](https://build.osrfoundation.org/buildStatus/icon?job=ignition_gazebo-ci-default-homebrew-amd64)](https://build.osrfoundation.org/job/ignition_gazebo-ci-default-homebrew-amd64)  
Windows       | [![Build Status](https://build.osrfoundation.org/buildStatus/icon?job=ignition_gazebo-ci-default-windows7-amd64)](https://build.osrfoundation.org/job/ignition_gazebo-ci-default-windows7-amd64)

Ignition Gazebo is the leading open source robotics simulator. Through Ignition Gazebo users have access to high fidelity physics, rendering, and sensor models. Additionally, users and developers have multiple points of entry to simulation including a graphical user interface, plugins, and asynchronous message passing and services.

Ignition Gazebo is derived from [Gazebo](http://gazebosim.org), and represents over 16 years of development and experience in robotics and simulation. This library is part of the [Ignition Robotics](https://ignitionrobotics.org) project.

# Table of Contents

[Features](#markdown-header-features)

[Install](#markdown-header-install)

* [Binary Install](#markdown-header-binary-install)

* [Source Install](#markdown-header-source-install)

    * [Prerequisites](#markdown-header-prerequisites)
  
    * [Building from Source](#markdown-header-building-from-source)

[Usage](#markdown-header-usage)

[Documentation](#markdown-header-documentation)

[Testing](#markdown-header-testing)

[Folder Structure](#markdown-header-folder-structure)

[Code of Conduct](#markdown-header-code-of-conduct)

[Contributing](#markdown-header-code-of-contributing)

[Versioning](#markdown-header-versioning)

[License](#markdown-header-license)

# Features

**Dynamics simulation**: Access multiple high-performance physics engines
through [Ignition
Physics](https://bitbucket.org/ignitionrobotics/ign-physics).

**Advanced 3D graphics**: Utilizing OGRE v2, Ignition Gazebo provides realistic
rendering of environments including high-quality lighting, shadows, and
textures.

**Sensors and noise models**: Generate sensor data, optionally with noise, from laser range finders, 2D/3D cameras, Kinect style sensors, contact sensors, force-torque, IMU, and more.

**Plugins**: Develop custom plugins for robot, sensor, and
environmental control.

**Simulation models**: Access numerous robots including PR2, Pioneer2 DX, iRobot Create, and TurtleBot. Construct environments using other physically accurate models. Or build a new model using [SDF](https://sdformat.org).

**TCP/IP Transport**: Run simulation on remote servers, and interface to Ignition Gazebo through socket-based message passing using [Ignition Transport](https:/bitbucket.org/ignitionrobotics/ign-transport).

**Command line tools**: Extensive command line tools facilitate simulation introspection and control.

# Install

We recommend following the [Binary Install](#markdown-header-binary-install) instructions to get Ignition Gazebo installed as quickly and painlessly as possible.

The [Source Install](#markdown-header-source-install) instructions should be used if you need the very latest software improvements, you need to modify the code, or you plan to make a contribution.

## Binary Install

The binary install method will use pre-built packages which are typically available through a package management utility such as [Apt](https://wiki.debian.org/Apt). This approach eliminates the need to download and compile source code, and dependencies are handled for you. The downside of a binary install is that you won't be able to modify the code. See [Source Install](#markdown-header-source-install) for information on installing Ignition Gazebo from source.

**Ubuntu Bionic**

1. Configure package repositories.

    ```
    sudo sh -c 'echo "deb http://packages.osrfoundation.org/gazebo/ubuntu-stable `lsb_release -cs` main" > /etc/apt/sources.list.d/gazebo-stable.list'
    ```
    
    ```
    sudo sh -c 'echo "deb http://packages.osrfoundation.org/gazebo/ubuntu-prerelease `lsb_release -cs` main" > /etc/apt/sources.list.d/gazebo-prerelease.list'
    ```
    
    ```
    wget http://packages.osrfoundation.org/gazebo.key -O - | sudo apt-key add -
    ```
    
    ```
    sudo apt-get update
    ```
    
2. Install Ignition Gazebo

    ```
    sudo apt-get install libignition-gazebo-dev
    ```

## Source Install

A source install 
### Prerequisites

Ignition Gazebo has a fairly large set of dependencies. Refer to the following sections for dependency installation instructions for each supported operating system.

**[Ubuntu Bionic](http://releases.ubuntu.com/18.04/)**

1. Base requirements:

    ```
    sudo apt-get -y install cmake build-essential curl cppcheck g++-8 libgflags-dev doxygen ruby-ronn libtinyxml2-dev libtinyxml-dev software-properties-common libeigen3-dev
    ```

2. Required Ignition libraries:

    ```
    sudo sh -c 'echo "deb http://packages.osrfoundation.org/gazebo/ubuntu-stable `lsb_release -cs` main" > /etc/apt/sources.list.d/gazebo-stable.list'
    sudo sh -c 'echo "deb http://packages.osrfoundation.org/gazebo/ubuntu-prerelease `lsb_release -cs` main" > /etc/apt/sources.list.d/gazebo-prerelease.list'
    wget http://packages.osrfoundation.org/gazebo.key -O - | sudo apt-key add -
    sudo apt-get update
    sudo apt-get -y install libignition-cmake2-dev libignition-common3-dev libignition-math6-eigen3-dev libignition-plugin-dev libignition-physics-dev libignition-rendering-dev libignition-tools-dev libignition-transport6-dev libignition-gui-dev libignition-msgs3-dev libsdformat8-dev
    ```

### Building from source

1. Install [prerequisites](#markdown-header-prerequisites)

2. Configure gcc8

    * Ubuntu
    
        ```
        update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-8 800 --slave /usr/bin/g++ g++ /usr/bin/g++-8 --slave /usr/bin/gcov gcov /usr/bin/gcov-8
        ```
        
1. Clone the repository.

    ```
    hg clone https://bitbucket.org/ignitionrobotics/ign-gazebo
    ```

2. Configure and  build.

    ```
    cd ign-gazebo
    mkdir build
    cd build
    cmake ../
    make
    ```

# Usage

Gazebo can be run from the command line, once [installed](#markdown-header-install), using:

```
ign-gazebo
```

For help, and command line options use:

```
ign-gazebo -h
```

# Documentation

The following links contain documentation for the latest release.

* [API Documentation](https://ignitionrobotics.org/libs/gazebo/latest)
* [Tutorials](https://ignitionrobotics.org/libs/gazebo/latest/tutorials)

Documentation for past releases can be accessed at [https://ignitionrobotics.org/libs/gazebo](https://ignitionrobotics.org/libs/gazebo)

You can also generate documentation from a clone of this repository by following these steps.

1. You will need [Doxygen](http://www.doxygen.org/). On Ubuntu Doxygen can be installed using

    ```
    sudo apt-get install doxygen
    ```

2. Clone the repository

    ```
    hg clone https://bitbucket.org/ignitionrobotics/ign-gazebo
    ```

3. Configure and build the documentation.

    ```
    cd ign-gazebo
    mkdir build
    cd build
    cmake ../
    make doc
    ```

4. View the documentation by running the following command from the `build` directory.

    ```
    firefox doxygen/html/index.html
    ```
    
# Testing

Follow these steps to run tests and static code analysis in your clone of this repository.

1. Follow the [building from source instructions](#markdown-header-building from source).

2. Run tests.

    ```
    make test
    ```

3. Static code checker.

    ```
    make codecheck
    ```

See the [Writing Tests section of the contributor guide](https://bitbucket.org/ignitionrobotics/ign-gazebo/src/406665896aa40bb42f14cf61d48b3d94f2fc5dd8/CONTRIBUTING.md#markdown-header-writing-tests) for help creating or modifying tests.

# Folder Structure

Refer to the following table for information about important directories and files in this repository.

```
+-- examples                 Example programs.  
+-- include/ignition/gazebo  Header files.  
+-- src                      Source files and unit tests.  
|    +-- components          Component source code.  
|    +-- gui                 Graphical interface source code.  
|    +-- system              System source code.
+-- test
|    +-- integration         Integration tests.
|    +-- performance         Performance tests.
|    +-- plugins             Plugin tests.
|    +-- regression          Regression tests.
|    +-- worlds              SDF world files used in tests.
+-- tutorials                Tutorials, written in markdown. 
+-- Changelog.md             Changelog.
+-- CMakeLists.txt           CMake build script.  
+-- Migration.md             Migration guide.  
+-- README.md                This readme.  
```

# Contributing

Please see
[CONTRIBUTING.md](https://bitbucket.org/ignitionrobotics/ign-gazebo/src/406665896aa40bb42f14cf61d48b3d94f2fc5dd8/CONTRIBUTING.md?at=default&fileviewer=file-view-default).

# Code of Conduct

Please see
[CODE_OF_CONDUCT.md](https://bitbucket.org/ignitionrobotics/ign-gazebo/src/406665896aa40bb42f14cf61d48b3d94f2fc5dd8/CODE_OF_CONDUCT.md?at=default&fileviewer=file-view-default).

# Versioning

This library uses [Semantic Versioning](https://semver.org/). Additionally, this library is part of the [Ignition Robotics project](https://ignitionrobotics.org) which periodically releases a versioned set of compatible and complimentary libraries. See the [Ignition Robotics website](https://ignitionrobotics.org) for version and release information.

# License

This library is licensed under [Apache 2.0](https://www.apache.org/licenses/LICENSE-2.0). See also the [LICENSE](https://bitbucket.org/ignitionrobotics/ign-gazebo/src/406665896aa40bb42f14cf61d48b3d94f2fc5dd8/LICENSE?at=default&fileviewer=file-view-default) file.



