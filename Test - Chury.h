#ifndef __TEST_H
#define __TEST_H

#include "BaseModel.h"
#include <functional>

using namespace std;
using namespace std::placeholders;

class Test : public BaseModel
{
public:
  void __init() {
    scene();
  }
  void scene() {
      asteroid();
      compress();
      repeat(100, bind(&Test::impact, this));
      repeat(200, bind(&Test::small_impact, this));

      select_all();
      apply_noise_field(bind(&Test::noisy, this,  _1, _2, _3), 0.2f);
  }

  void compress()
  {
      auto v2 = random_surface_point();
      select_x_gradient(vec(0,0,0),9,100,100);
      scale_selection(1.0f, 0.4f, 0.4f);
      invert_selection();
      scale_selection(0.5f, 1.0f, 1.0f);
      rotate_z_selection(0.3f);
      select_x_gradient(vec(0,0,0),12,100,100);
      invert_selection();
      scale_selection(1.1f, 1.0f, 1.0f);

  }

  void deform()
  {
      auto v = random_surface_point();
      select_sphere(vec(0, 0, 0), 6);
      translate_selection(random_vector());
  }

  void asteroid() {
      ellipsoid(random_float()*2 + 8.2, random_float()*1.2 + 2.9, random_float()*1.2 + 2.9,8); // 9

      repeat(60,bind(&Test::impact, this));
  }

  float noisy(float x, float y, float z) {
      return noise3(x*39.2f,y*39.2f,z*39.2f)*noise3(x*5.2f,y*5.2f,z*5.2f) * max(0.0f, noise3(x,y,z)-0.5f);
  }

  void impact() {
    auto p1 = random_surface_point();
    select_sphere(p1,random_float() * 3 + 1,0.04);
    translate_selection(-p1.normal * 0.25);
  }

  void small_impact() {
      auto p1 = random_surface_point();
      auto radius = random_float() * 0.8 + 0.2;
      select_sphere(p1,radius,0.02);
      auto td = -p1.normal * (random_float()+0.5) * 0.7;
      translate_selection(td * 0.2);
      select_sphere(p1,radius * 1.25,0.02);
      translate_selection(-td * 0.1);
  }

};

#endif
