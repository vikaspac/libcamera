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
 * 5) Configure camera: role->Viewfinder, and validate
 * 6) Allocate buffers
 * 7) stop camera
 *
 *
 * Filename: capture5.cpp
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
unique_ptr<CameraConfiguration> config_;
FrameBufferAllocator *allocator_;

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


	/* Create configuration like StillCapture, StillCaptureRaw, VideoRecording, Viewfinder  */
	config_ = camera_->generateConfiguration( { StreamRole::Viewfinder } );

	if (!config_) {
		cout << "Warn: Failed to generate configuration from roles" << endl;
		return -EINVAL;
	}

	//Stream configuration
	StreamConfiguration &streamConfig = config_->at(0);
	cout << "Vikas: Default Stream configuration is: " << streamConfig.toString() << std::endl;


	/* setting WxH */
	streamConfig.size.width = 1280;
	streamConfig.size.height = 720;

	ret = camera_->configure(config_.get());
	if (ret) {
		cout << "Err: CONFIGURATION FAILED!" << endl;
		return -1;
	}

	/* Configuration Validation */
	CameraConfiguration::Status validation = config_->validate();
	if (validation == CameraConfiguration::Invalid) {
		cout << "Failed to create valid camera configuration" << endl;
		return -EINVAL;
	}

	cout << "Stream configuration adjusted to " << streamConfig.toString().c_str() << endl;

	/* Apply the configurations settings after validation is done */
	ret = camera_->configure(config_.get());
	if (ret < 0) {
		cout << "Failed to configure camera" << endl;
		return ret;
	}
	cout << "Vikas: Configuration set" << endl;


	/* Allocate Buffers */
	Stream *stream = streamConfig.stream();
	allocator_ = new FrameBufferAllocator(camera_);
	ret = allocator_->allocate(stream);
		if (ret < 0) {
			cout << "Err: Failed to allocate buffers" << endl;
			return ret;
		}
	cout << "Vikas: Buffer allocation done" << endl;

#if 1

	camera_->stop();
//	camera_->freeBuffers();
	camera_->release();
	camera_.reset();
	/* Stop Camera */
	cout << "Vikas: CameraManager stop +" << endl;
	cm->stop();
	cout << "Vikas: CameraManager stop -" << endl;
	delete cm;
#endif

	return 0;
}
