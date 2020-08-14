.. SPDX-License-Identifier: CC-BY-SA-4.0

.. section-begin-libcamera

===========
 libcamera
===========

**A complex camera support library for Linux, Android, and ChromeOS**

Cameras are complex devices that need heavy hardware image processing
operations. Control of the processing is based on advanced algorithms that must
run on a programmable processor. This has traditionally been implemented in a
dedicated MCU in the camera, but in embedded devices algorithms have been moved
to the main CPU to save cost. Blurring the boundary between camera devices and
Linux often left the user with no other option than a vendor-specific
closed-source solution.

To address this problem the Linux media community has very recently started
collaboration with the industry to develop a camera stack that will be
open-source-friendly while still protecting vendor core IP. libcamera was born
out of that collaboration and will offer modern camera support to Linux-based
systems, including traditional Linux distributions, ChromeOS and Android.

.. section-end-libcamera
.. section-begin-getting-started

Getting Started
---------------

To fetch the sources, build and install:

::

  git clone git://linuxtv.org/libcamera.git
  cd libcamera
  meson build
  ninja -C build install


Project
~~~~~~~

This is a personal fork.
Try preparing more test app examples for Raspberry Pi device.

::

  git clone git://xyx/libcamera.git
  cd libcamera
  meson build
  cd build
  meson configure -Dpipelines=raspberrypi -Dtest=false
  cd ..
  sudo ninja -C build install


Export settings:
	export LD_LIBRARY_PATH=/usr/local/lib/arm-linux-gnueabihf:$LD_LIBRARY_PATH

Log Levels:
	export LIBCAMERA_LOG_LEVELS=0

Run test app
::

  cd libcamera/build/test/mytests
  ./capture



.. code::

  gst-launch-1.0 libcamerasrc camera-name="Camera 1" ! videoconvert ! autovideosink

.. section-end-getting-started
