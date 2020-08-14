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
 * 3) get camera ID
 * 4) Acquire the camera
 * //5) stop camera
 *
 *
 * Filename: capture3.cpp
 */


#include <string.h>

#include <iostream>
#include <memory>


#include <libcamera/libcamera.h>
#include <libcamera/camera_manager.h>
#include <libcamera/version.h>


using namespace libcamera;
using namespace std;

shared_ptr<Camera> camera_;

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


	/* get camera */
	/* If only one camera is available, use it automatically. */
	if (cm->cameras().size() == 1)
		cameraName = cm->cameras()[0]->id();

	if (cameraName == "")
		return -EINVAL;

	camera_ = cm->get(cameraName);
	if (!camera_) {
		cout << "Err: Camera" << cameraName.c_str() << "not found" << endl;
		return -ENODEV;
	}
	cout << "Vikas: Found camera: " << cameraName.c_str() << endl;

	/* acquire camera */
	if (camera_->acquire()) {
		cout << "Err: Failed to acquire camera" << endl;
		camera_.reset();
		return -EBUSY;
	}
	cout << "Vikas: Acquired camera !!!  " << cameraName.c_str() << endl;






#if 0
	/* Stop Camera */
	cout << "Vikas: CameraManager stop +" << endl;
	cm->stop();
	cout << "Vikas: CameraManager stop -" << endl;
	delete cm;
#endif

	return 0;
}
