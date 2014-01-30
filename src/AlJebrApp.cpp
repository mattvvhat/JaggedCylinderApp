#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/Camera.h"
#include "cinder/gl/Light.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Fbo.h"

#include "aljebr.h"
#include "JaggedCylinder.h"

#include "cinder/qtime/QuickTime.h"
#include "AsynchMovieWriter.h"

#include "SvvimPost.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class AlJebrApp : public AppNative {
public:
	void setup();
	void update();
	void draw();
  
private:
  void prepareSettings(Settings *settings);
  void setupGeometry();
  void updateGeometry();
  
  gl::Texture render();
  
  gl::VboMeshRef  mMesh;
  CameraPersp     mCam;
  
  shared_ptr <gl::Light> mLight;
  gl::GlslProg mPhongShader;
  gl::Fbo mFbo;
  
  shared_ptr <AsynchMovieWriter> mWriter;
  
  SvvimPost mPost;
};

void AlJebrApp::prepareSettings(Settings *settings) {
  settings->setWindowSize(500, 500);
}

void AlJebrApp::setup() {
  mPost.setup();
  
  gl::Fbo::Format fboFormat;
  fboFormat.setSamples(16);
  mFbo = gl::Fbo(500, 500, fboFormat);

  setupGeometry();
  mLight        = make_shared <gl::Light> (gl::Light::DIRECTIONAL, 0);
  try {
    mPhongShader = gl::GlslProg(loadResource("project.vert"), loadResource("project.frag"));
  }
  catch (const std::exception &e) {
    app::console() << e.what() << "\n";
  }
  
  fs::path path = fs::path("./what.mp4");
	if (path.empty())
    return;
  
  qtime::MovieWriter::Format format;
  format.setCodec(qtime::MovieWriter::CODEC_RAW);
  mWriter = make_shared <AsynchMovieWriter> (path, 500, 500, format);
}

void AlJebrApp::update() {
  Vec3f eye     = Vec3f(0, 4, 0);
  Vec3f target  = Vec3f(0, 0, 0);
  Vec3f up      = Vec3f(0, 0, 1);
  mCam.lookAt(eye, target, up);
  mLight->setAmbient(ci::Color(.8, .8, .8));
  mLight->setDiffuse(ci::Color(.9, .9, .9));
  mLight->setSpecular(ci::Color(1, 1, 1));
  mLight->lookAt(Vec3f(5, 0, 0), target);
}

void AlJebrApp::draw() {
  gl::setMatricesWindow(getWindowWidth(), getWindowHeight());
  
  gl::Texture orig = render();
  gl::Texture blur = mPost.applyBlur(orig, 1.5f + 1.5f * cos(getElapsedSeconds()));
  
  // Disable lighting so that we can use our own
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  glDisable(GL_LIGHTING);
  glDisable(GL_DEPTH_TEST);
  gl::disableDepthRead();
  gl::disableDepthWrite();
  
  // Draw FBO
  ci::gl::clear(Color(0, 0, 0));

  gl::color(1, 1, 1, 0.7f);
  gl::draw(orig, getWindowBounds());
  gl::color(1, 1, 1, 0.3f);
  gl::draw(blur, getWindowBounds());
  
  if (mWriter)
    mWriter->addFrame(copyWindowSurface());
}

void AlJebrApp::setupGeometry() {
  aljebr::Geometry geom = create_jagged(-3.5f, 3.5f, 7);
  mMesh = geom.generate();
}

void AlJebrApp::updateGeometry() {
}

gl::Texture AlJebrApp::render () {
  mFbo.bindFramebuffer();
  gl::pushMatrices();
  
  glEnable(GL_LIGHTING);
  glEnable(GL_DEPTH_TEST);
  gl::enableDepthWrite();
  gl::enableDepthRead();
  gl::disableAlphaBlending();
	ci::gl::clear(Color(0, 0, 0));
  
  gl::setMatrices(mCam);
  
  float t = getElapsedSeconds();
  gl::rotate(45 * Vec3f(0, 0, t));
  
  // Bind Phong Shader
  if (mPhongShader) {
    mPhongShader.bind();
  }
  
  // Draw mesh or cube
  if (mMesh) {
    gl::color(1, 1, 1);
    gl::draw(mMesh);
  }
  else {
    gl::color(1, 0, 1);
    gl::drawCube(Vec3f(0, 0, 0), Vec3f(1, 1, 1));
  }
  
  // Unbind Phong Shader
  if (mPhongShader) {
    mPhongShader.unbind();
  }
  
  gl::popMatrices();
  mFbo.unbindFramebuffer();
  return mFbo.getTexture();
}

CINDER_APP_NATIVE( AlJebrApp, RendererGl )
