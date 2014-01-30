//
//  SvvimPost.h
//  SvvimGram
//
//  Created by Matthew Owen on 1/6/14.
//
//

#ifndef SvvimGram_SvvimPost_h
#define SvvimGram_SvvimPost_h

#include "cinder/app/App.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Fbo.h"

using namespace cinder;
using namespace cinder::gl;
using namespace ci::app;
using namespace std;

#define SVVIM_POST_DEF_FBO_WIDTH  500
#define SVVIM_POST_DEF_FBO_HEIGHT 500

class SvvimPost {
public:
  
  SvvimPost(int width = SVVIM_POST_DEF_FBO_WIDTH, int height = SVVIM_POST_DEF_FBO_HEIGHT) {
    mWidth  = width > 0   ? width   : SVVIM_POST_DEF_FBO_WIDTH;
    mHeight = height > 0  ? height  : SVVIM_POST_DEF_FBO_HEIGHT;
  }
  
  void setup () {
    mFirstPassFbo   = Fbo(500, 500);
    mSecondPassFbo  = Fbo(500, 500);
    HBLUR = GlslProg(loadResource("pass.vert"), loadResource("hblur.frag"));
    VBLUR = GlslProg(loadResource("pass.vert"), loadResource("vblur.frag"));
  }

  gl::Texture applyBlur(  const gl::Texture &texture, const float amount);
  
private:
  int mWidth, mHeight;
  
  GlslProg CRUSH;
  GlslProg HBLUR;
  GlslProg VBLUR;

  Fbo mFirstPassFbo, mSecondPassFbo;
};


// Implementation

gl::Texture SvvimPost::applyBlur(const gl::Texture &texture, const float amount) {
  gl::pushMatrices();
  
  float a = amount/getWindowWidth();
  
  // Apply vertical blur shader
  {
    glClearColor(.2, .2, .2, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    HBLUR.bind();
    HBLUR.uniform("texture", 0);
    HBLUR.uniform("size", (float) a);
    mFirstPassFbo.bindFramebuffer();
    texture.bind(0);
    gl::setMatricesWindow(getWindowWidth(), getWindowHeight());
    gl::clear(ColorA(0, 0, 0, 0));
    gl::color(1, 1, 1);
    gl::drawSolidRect(mFirstPassFbo.getBounds());
    texture.unbind(0);
    mFirstPassFbo.unbindFramebuffer();
    HBLUR.unbind();
  }
  
  // Apply horizontal blur shader
  {
    glClearColor(.2, .2, .2, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    VBLUR.bind();
    VBLUR.uniform("texture", 0);
    VBLUR.uniform("size", (float) a);
    mSecondPassFbo.bindFramebuffer();
    mFirstPassFbo.bindTexture(0);
    gl::setMatricesWindow(getWindowWidth(), getWindowHeight());
    gl::clear(ColorA(0, 0, 0, 0));
    gl::color(1, 1, 1);
    gl::drawSolidRect(mSecondPassFbo.getBounds());
    mFirstPassFbo.unbindTexture();
    mSecondPassFbo.unbindFramebuffer();
    VBLUR.unbind();
  }
  
  gl::popMatrices();
  return mSecondPassFbo.getTexture();
}

#endif
