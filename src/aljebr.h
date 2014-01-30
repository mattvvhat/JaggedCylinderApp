//
//  aljebr.h
//  AlJebr
//
//  Created by Matthew Owen on 1/29/14.
//
//

#pragma once

#include "cinder/gl/Vbo.h"
#include "cinder/app/App.h"

namespace aljebr {
  
  /**
   * Triange Face
   * Structure to store 3 points - a, b, c - that define a plane
   * and their normal. In principle, the normal can be computed
   * from a, b, c and a boolean flag.
   */
  struct Face {
    ci::Vec3f a, b, c;
    ci::Vec3f n;
  };
  
  /**
   * Square Face
   * Strictire to store 4 points - a, b. c. d = tjat define a square tile
   * and its normal.
   */
  struct SquareFace {
    ci::Vec3f a, b, c, d;
    ci::Vec3f n;
  };
  
  /**
   * Geometry
   */
  class Geometry {
  public:
    Geometry();
    void addFace(const Face &f);
    void addFace(const Face &f, const ci::Vec3f &n);
    ci::gl::VboMeshRef generate() const;
  private:
    ci::gl::VboMesh::Layout mLayout;
    ci::gl::VboMeshRef      mMeshRef;
    
    std::vector <ci::Vec3f>   mPoints;
    std::vector <ci::Vec3f>   mNormals;
    std::vector <uint32_t>    mIndices;
    std::vector <ci::Vec2f>   mTexCoords;
  };
}

namespace al = aljebr;

al::Geometry::Geometry () {
	mLayout.setStaticIndices();
	mLayout.setStaticPositions();
  mLayout.setStaticNormals();
}

void al::Geometry::addFace (const Face &f) {
  mPoints.push_back(f.a);
  mPoints.push_back(f.b);
  mPoints.push_back(f.c);
  
  unsigned int k = mIndices.size();
  mIndices.push_back(k+0);
  mIndices.push_back(k+1);
  mIndices.push_back(k+2);
  
  ci::Vec3f p = f.a - f.b;
  ci::Vec3f q = f.a - f.c;
  p.normalize();
  q.normalize();
  ci::Vec3f n = p.cross(q);
  n.normalize();
  mNormals.push_back(n);
  mNormals.push_back(n);
  mNormals.push_back(n);
}

void al::Geometry::addFace (const Face &f, const ci::Vec3f &n) {
  mPoints.push_back(f.a);
  mPoints.push_back(f.b);
  mPoints.push_back(f.c);
  unsigned int k = mIndices.size();
  mIndices.push_back(k+0);
  mIndices.push_back(k+1);
  mIndices.push_back(k+2);
}

ci::gl::VboMeshRef al::Geometry::generate () const {
  ci::gl::VboMeshRef mesh;
  
  if (mPoints.size() != mIndices.size()) {
    ci::app::console() << mPoints.size() << " != " << mIndices.size() << "\n";
    return mesh;
  }
  
  std::vector <ci::Vec3f> points;
  std::vector <uint32_t>  indices;
  
  mesh = ci::gl::VboMesh::create(mPoints.size(), mIndices.size(), mLayout, GL_TRIANGLES);
  mesh->bufferPositions(mPoints);
  mesh->bufferIndices(mIndices);
  mesh->bufferNormals(mNormals);

  return mesh;
}





























