/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright (C) 2019, Google Inc.
 *
 * libcamera Camera API tests
 */

/* Device: Raspberry Pi 3B+
 * Sensor: Camera module V1 ov5647
 *
 * 1)  create cameramanager
 * 2)  start camera
 * 3)  get camera ID
 * 4)  Acquire the camera
 * 5)  Configure camera: role->Viewfinder, and validate
 * 6)  Allocate buffers
 * 7)  Frame capture request creation
 * 8)  Enable signal and slot mechanism
 * 9)  Start Capturing .i.e., prepare buffers and start queuing them
 * 10) Start Event displatcher for listening events and set timer signals
 * 11) stop camera
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


static void requestComplete(Request *request)
{

	if (request->status() == Request::RequestCancelled)
		return;
}



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


	cout << "Vikas: Configuration set +" << endl;
	/* Create configuration like StillCapture, StillCaptureRaw, VideoRecording, Viewfinder  */
	config_ = camera_->generateConfiguration( { StreamRole::Viewfinder } );
//	config_ = camera_->generateConfiguration( { StreamRole::StillCapture } );
//	config_ = camera_->generateConfiguration( { StreamRole::StillCaptureRaw } );
//	config_ = camera_->generateConfiguration( { StreamRole:: VideoRecording } );

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
	cout << "Vikas: Configuration set -" << endl;


	/* Allocate Buffers */
	cout << "Vikas: Buffer allocation +" << endl;
	Stream *stream = streamConfig.stream();
	allocator_ = new FrameBufferAllocator(camera_);
	ret = allocator_->allocate(stream);
		if (ret < 0) {
			cout << "Err: Failed to allocate buffers" << endl;
			return ret;
		}
	cout << "Vikas: Buffer allocation -" << endl;


	cout << "Vikas: Frame capture request creation" << endl;
	/* Frame Capture request creation */
	std::vector<Request *> requests;
	for (const std::unique_ptr<FrameBuffer> &buffer : allocator_->buffers(stream)) {
		Request *request = camera_->createRequest();
		if (!request) {
			cout << "Failed to create request" << endl;
			return -1;
		}

		if (request->addBuffer(stream, buffer.get())) {
			cout << "Failed to associating buffer with request" << endl;
			return -1;
		}

		requests.push_back(request);
	}

	/* signal and slot communication mechanism connect with callback function association */
	camera_->requestCompleted.connect(requestComplete);


	cout << "Vikas: Start camera capture" << endl;
	/* Start capturing*/
	if (camera_->start()) {
		cout << "Failed to start camera" << endl;
		return -1;
	}

	for (Request *request : requests) {
		if (camera_->queueRequest(request)) {
			cout << "Failed to queue request" << endl;
			return -1;
		}
	}

	/* Run the Event loop for dispatching the events received from device
	 * Poll based dispatcher for 1 sec used */
	cout << "Vikas: Run the event Dispatcher" << endl;
	EventDispatcher *dispatcher = cm->eventDispatcher();

	Timer timer;
	timer.start(1000);
	while (timer.isRunning())
		dispatcher->processEvents();



	cout << "Vikas: event Dispatcher POLL  done" << endl;

	/* Clean up camera */
#if 1
	cout << "Vikas: CameraManager close +" << endl;

	delete allocator_;

	ret = camera_->stop();
	if (ret)
		cout << "Err: Failed to stop capture" << endl;

	camera_->requestCompleted.disconnect(requestComplete);
	config_.reset();
	camera_->release();
	camera_.reset();
	cm->stop();
	cout << "Vikas: CameraManager close -" << endl;
	delete cm;
#endif

	return 0;
}
