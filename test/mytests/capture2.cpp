/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright (C) 2019, Google Inc.
 *
 * libcamera Camera API tests
 */

/* Device: Raspberry Pi 3B+
 * Sensor: Camera module V1 ov5647
 *
 * 1) create cameramanager
 * 2) start camera
 * 3) stop camera
 *
 *
 * Filename: capture2.cpp
 */


#include <string.h>

#include <iostream>
#include <memory>

#include <libcamera/libcamera.h>

using namespace libcamera;
using namespace std;


int main()
{
	int ret=0;
	std::string cameraName;

	/* create camera manager */
	CameraManager *cm = new CameraManager();

	/* Start Camera */
	cout << "Vikas: CameraManager start +" << endl;
	ret = cm->start();
	if (ret) {
		cout << "Failed to start camera manager: " << strerror(-ret) << endl;
		cout << "Vikas: CameraManager start -" << endl;
		return ret;
	}
	cout << "Vikas: CameraManager start -" << endl;





	/* Stop Camera */
	cout << "Vikas: CameraManager stop +" << endl;
	cm->stop();
	cout << "Vikas: CameraManager stop -" << endl;
	delete cm;

	return 0;
}

