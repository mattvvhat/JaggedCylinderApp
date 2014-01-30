//
//  JaggedCylinder.h
//  AlJebr
//
//  Created by Matthew Owen on 1/29/14.
//
//

#pragma once

#include "aljebr.h"
#include "cinder/gl/Vbo.h"
#include <random>

class Ring {
public:
  Ring(int points = 5, float skew = 0.0f, float y = 0.0f);
  ci::gl::VboMeshRef generate() const;
  
private:
};

#define RAND_FLOAT (float) (rand() / ((float) RAND_MAX))


std::vector <ci::Vec3f> create_ring (int points, float skew, float y) {
  std::vector <ci::Vec3f> ring;

  int seed_val;
  std::mt19937 rng;
  std::uniform_int_distribution<uint32_t> dist(-.2, .2);
  rng.seed(seed_val);
  
  for (int k=0; k < points; k++) {
    float theta = 2.0f * M_PI * (k+0.0f) / points + skew;
    float r = 1.0f;
    ci::Vec3f p = ci::Vec3f((r + RAND_FLOAT/3.f)*cos(theta) + dist(rng), r*sin(theta) + dist(rng), y + RAND_FLOAT/1.5f);
    ring.push_back(p);
  }
  
  return ring;
}

al::Geometry create_jagged (float ymin, float ymax, float steps) {
  aljebr::Geometry geom;
  
  std::vector <ci::Vec3f> prev = create_ring(9, 0, ymin);
  std::vector <ci::Vec3f> next;
  
  float skew = 0.0f;
  
  for (int k=0; k < steps; k++) {
    skew += 1.25f;
    // skew = 0.0f;
    
    float y = ymin + (k+1)/steps * (ymax - ymin);
    
    next = create_ring(9, skew, y);
    
    for (int k=0; k < next.size()-1; k++) {
      ci::Vec3f p = prev[k];
      ci::Vec3f q = next[k];
      ci::Vec3f r = prev[(k+1) % prev.size()];
      ci::Vec3f s = next[(k+1) % next.size()];
      al::Face bl, ur;
      bl.a = p;
      bl.b = q;
      bl.c = r;
      ur.a = s;
      ur.b = r;
      ur.c = q;
      geom.addFace(bl);
      geom.addFace(ur);
    }
    ci::Vec3f p = prev[prev.size()-1];
    ci::Vec3f q = next[next.size()-1];
    ci::Vec3f r = prev[0];
    ci::Vec3f s = next[0];
    al::Face bl, ur;
    bl.a = p; bl.b = q; bl.c = r;
    ur.a = s; ur.b = r; ur.c = q;
    geom.addFace(bl);
    geom.addFace(ur);
    
    prev = next;
  }
  
  return geom;
}