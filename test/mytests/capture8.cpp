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
 * 11) Print buffer statistics like FPS, seq num, bytesused and timestamp
 * 12) Add the frame dump logic with macro #define DUMP_BUFFER
 * 13) stop camera
 *
 *
 * Filename: capture8.cpp
 */

#include <string.h>

#include <iostream>
#include <memory>

#include <chrono>
#include <iomanip>
#include <limits.h>
#include <sstream>

#include <libcamera/libcamera.h>
#include <libcamera/camera_manager.h>
#include <libcamera/version.h>



using namespace libcamera;
using namespace std;

//Define the below macro if you like to dump the frames into files
#define DUMP_BUFFER 1

#ifdef DUMP_BUFFER

#include <map>
#include <libcamera/buffer.h>

#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

std::map<int, std::pair<void *, unsigned int>> mappedBuffers_;


void BufferWriter_mapBuffer(libcamera::FrameBuffer *buffer);
int BufferWriter_write(libcamera::FrameBuffer *buffer, const std::string &streamName);

#endif


shared_ptr<Camera> camera_;
unique_ptr<CameraConfiguration> config_;
FrameBufferAllocator *allocator_;
std::chrono::steady_clock::time_point last_;


static void requestComplete(Request *request)
{
	std::map<libcamera::Stream *, std::string> streamName_;

	if (request->status() == Request::RequestCancelled)
		return;

	const std::map<Stream *, FrameBuffer *> &buffers = request->buffers();

	std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
	double fps = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_).count();
	fps = last_ != std::chrono::steady_clock::time_point() && fps
	    ? 1000.0 / fps : 0.0;
	last_ = now;

	std::stringstream info;
	info << "fps: " << std::fixed << std::setprecision(2) << fps;

	for (auto it = buffers.begin(); it != buffers.end(); ++it) {
		Stream *stream = it->first;
		FrameBuffer *buffer = it->second;
		const std::string &name = streamName_[stream];

		const FrameMetadata &metadata = buffer->metadata();

		info << " " << name
		     << " seq: " << std::setw(6) << std::setfill('0') << metadata.sequence
		     << " bytesused: ";

		unsigned int nplane = 0;
		for (const FrameMetadata::Plane &plane : metadata.planes) {
			info << plane.bytesused;
			if (++nplane < metadata.planes.size())
				info << "/";
		}
		info << " timestamp: " << metadata.timestamp;

#ifdef DUMP_BUFFER
		if (BufferWriter_write(buffer, name)) {
			cout << "Dumping frames " << info.str() << endl;
		} else {

			cout << "Failed to dump the frame " << endl;
		}
	}
#else
	}

	cout << info.str() << endl;
#endif


#if 1
	/*
	 * Create a new request and populate it with one buffer for each
	 * stream.
	 */
	request = camera_->createRequest();
	if (!request) {
		cout << "Err: Can't create request" << endl;
		return;
	}

	for (auto it = buffers.begin(); it != buffers.end(); ++it) {
		Stream *stream = it->first;
		FrameBuffer *buffer = it->second;

		request->addBuffer(stream, buffer);
	}

	camera_->queueRequest(request);
#endif
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
//	config_ = camera_->generateConfiguration( { StreamRole::Viewfinder } );
//	config_ = camera_->generateConfiguration( { StreamRole::StillCapture } );
//	config_ = camera_->generateConfiguration( { StreamRole::StillCaptureRaw } );
	config_ = camera_->generateConfiguration( { StreamRole:: VideoRecording } );

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

#ifdef DUMP_BUFFER
		BufferWriter_mapBuffer(buffer.get());
#endif

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
	cout << "Vikas: CameraManager close +" << endl;

	ret = camera_->stop();
	if (ret)
		cout << "Err: Failed to stop capture" << endl;

	camera_->requestCompleted.disconnect(requestComplete);

	cout << "Vikas: CameraManager close -" << endl;

	return 0;
}




#ifdef DUMP_BUFFER


void BufferWriter_mapBuffer(FrameBuffer *buffer)
{
	for (const FrameBuffer::Plane &plane : buffer->planes()) {
		void *memory = mmap(NULL, plane.length, PROT_READ, MAP_SHARED,
				    plane.fd.fd(), 0);

		mappedBuffers_[plane.fd.fd()] =
			std::make_pair(memory, plane.length);
	}
}

int BufferWriter_write(FrameBuffer *buffer, const std::string &streamName)
{
	std::string filename;
	size_t pos;
	int fd, ret = 0;

	filename = "frame#.bin";
	pos = filename.find_first_of('#');
	if (pos != std::string::npos) {
		std::stringstream ss;
		ss << streamName << "-" << std::setw(6)
		   << std::setfill('0') << buffer->metadata().sequence;
		filename.replace(pos, 1, ss.str());
	}

	fd = open(filename.c_str(), O_CREAT | O_WRONLY |
		  (pos == std::string::npos ? O_APPEND : O_TRUNC),
		  S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
	if (fd == -1)
		return -errno;

	for (const FrameBuffer::Plane &plane : buffer->planes()) {
		void *data = mappedBuffers_[plane.fd.fd()].first;
		unsigned int length = plane.length;

		ret = ::write(fd, data, length);
		if (ret < 0) {
			ret = -errno;
			std::cerr << "write error: " << strerror(-ret)
				  << std::endl;
			break;
		} else if (ret != (int)length) {
			std::cerr << "write error: only " << ret
				  << " bytes written instead of "
				  << length << std::endl;
			break;
		}
	}

	close(fd);

	return ret;
}


#endif

