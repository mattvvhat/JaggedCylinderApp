//
//  AsynchMovieWriter.h
//  Projected
//
//  Created by Matthew Owen on 1/6/14.
//
//

#pragma once

#include "cinder/qtime/MovieWriter.h"
#include "cinder/ImageIo.h"
#include "cinder/Thread.h"
#include "cinder/gl/Texture.h"

#include <queue>
#include <mutex>

using namespace std;
using namespace ci;
using namespace ci::app;

using namespace cinder;
using namespace cinder::qtime;

class AsynchMovieWriter {
public:
  
  // Useless constructor
  AsynchMovieWriter() : mClosing(true) {
    app::console() << "WARNING: Using bad default constructor\n";
  }
  
  // Useful constructor
  AsynchMovieWriter (const fs::path &path, int32_t width, int32_t height, const qtime::MovieWriter::Format & format = qtime::MovieWriter::Format()) {
    mClosing  = false;
    mPath     = path;
    mFormat   = format;
    // MovieWriter
    mMovieWriter = qtime::MovieWriter::create(mPath, width, height, mFormat);
    // Thread
    mThread = make_shared<std::thread>( bind(&AsynchMovieWriter::processFrames, this) );
  }

  // Destructor
  ~AsynchMovieWriter () {
    mClosing = true;
    mThread->join();
  }
  
  // Addframe
  void addFrame (const ImageSourceRef &imageSource, float duration = -1.0f) {
    Frame frame;
    frame.image     = imageSource;
    frame.duration  = duration;
    // Accessing mFrames requires a lock
    mLatch.lock();
    mFrames.push(frame);
    mLatch.unlock();
  }

  
private:
  
  // Ongoing loop that only dies at end
  void processFrames () {
    while (!mClosing) {
      while (mFrames.size() > 0) {
        Frame frame = mFrames.front();
        // Ensure that multiple frames aren't being written and that
        // mFrames access doesn't happen simultaneously
        mLatch.lock();
        mMovieWriter->addFrame(frame.image, frame.duration);
        mFrames.pop();
        mLatch.unlock();
      }
    }
  }
  
  /**
   * FRAME STRUCT
   * image    : an image reference to be used with
   * duration : duration of the frame
   */
  struct Frame {
    ImageSourceRef  image;
    float           duration;
  };
  
  // Queue of frame objects
  std::queue <Frame> mFrames;
  // Thread stuff
  bool mClosing;
  std::mutex mLatch;
  std::shared_ptr<std::thread>  mThread;
  // Recorded variables
  fs::path mPath;
  qtime::MovieWriter::Format  mFormat;
  qtime::MovieWriterRef       mMovieWriter;
};