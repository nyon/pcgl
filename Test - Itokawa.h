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
      repeat(40, bind(&Test::impact, this));
      //repeat(200, bind(&Test::small_impact, this));

      select_all();
      apply_noise_field(bind(&Test::noisy, this,  _1, _2, _3), 0.3f);
  }

  void compress()
  {
      auto v2 = random_surface_point();
      select_x_gradient(vec(2,0,0),8,100,100);
      scale_selection(0.9f, 0.8f, 0.8f);
      select_x_gradient(vec(2,0,0),4,100,100);
      scale_selection(0.9f, 0.7f, 0.7f);
      //invert_selection();
      //scale_selection(0.8f, 1.0f, 1.0f);
      //rotate_z_selection(0.3f);
      //select_x_gradient(vec(1,0,0),10,100,100);
      //invert_selection();
      //scale_selection(1.2f, 1.0f, 1.0f);

  }

  void deform()
  {
      auto v = random_surface_point();
      select_sphere(vec(0, 0, 0), 6);
      translate_selection(random_vector());
  }

  void asteroid() {
      ellipsoid(random_float()*2 + 6.2, random_float()*1.2 + 2.9, random_float()*1.2 + 2.9,8); // 9

      repeat(60,bind(&Test::impact, this));
  }

  float noisy(float x, float y, float z) {
      return (0.25f * noise3(x*32.2f,y*32.2f,z*32.2f)+0.5f * noise3(x*12.2f,y*12.2f,z*12.2f)+noise3(x*5.2f,y*5.2f,z*5.2f)) * max(0.0f, noise3(x,y,z)-0.5f);
  }

  void impact() {
    auto p1 = random_surface_point();
    select_sphere(p1,random_float() * 3 + 1,0.03);
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
