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
    asteroid();
  }

  void asteroid() {
      ellipsoid(random_float()*2 + 6.2, random_float()*1.2 + 2.9, random_float()*1.2 + 2.9,9); // 9

      repeat(60,bind(&Test::impact, this));
      compress();
      repeat(40, bind(&Test::impact, this));
      repeat(10, bind(&Test::big_boulder, this));
      repeat(20, bind(&Test::small_impact, this));
      repeat(3000, bind(&Test::small_boulder, this));

      select_all();
      apply_noise_field(bind(&Test::noisy, this,  _1, _2, _3), 0.25f);
  }

  void compress()
  {
      select_x_gradient(vec(2,0,0),8);
      scale_selection(0.9f, 0.8f, 0.8f);
      select_x_gradient(vec(2,0,0),4);
      scale_selection(0.9f, 0.7f, 0.7f);
  }

  void deform()
  {
      select_sphere(vec(0, 0, 0), 6);
      translate_selection(random_vector());
  }

  void big_boulder() {
      auto a = random_vertex();
      auto b = random_float() * 0.1f + 0.05f;
      select_sphere(a,b,0.6f, 100.0f);
      translate_selection(a.normal*b);
      filter_selection_by_normal_angle(a.normal, M_PI/2.0f);
      scale_selection(2.0f);
  }

  void small_boulder() {
      auto a = random_vertex();

      push_clean_model_state();
      ellipsoid((random_float() + 0.5f)*5.0f,(random_float() + 0.5f)*5.0f,(random_float() + 0.5f)*5.0f,2);
      select_all();
      apply_noise_field(bind(&Test::small_boulder_noise, this,  _1, _2, _3), 20.0f);
      select_all();
      rotate_x_selection(random_float() * 4.0f);
      rotate_y_selection(random_float() * 4.0f);
      rotate_z_selection(random_float() * 4.0f);
      scale_selection(0.003f * random_float());
      recenter_selection();
      translate_selection(a);
      pop_and_merge_model_state();
  }

  float small_boulder_noise(float x, float y, float z) {
      return pnoise3(x*302.2f,y*302.2f,z*302.2f);
  }

  float noisy(float x, float y, float z) {
      return (0.25f * pnoise3(x*32.2f,y*32.2f,z*32.2f)+0.5f * pnoise3(x*12.2f,y*12.2f,z*12.2f)+pnoise3(x*5.2f,y*5.2f,z*5.2f)) * max(0.0f, pnoise3(x,y,z)-0.5f);
  }

  void impact() {
    auto p1 = random_vertex();
    select_sphere(p1,random_float() * 3 + 1,0.02f);
    translate_selection(-p1.normal * 0.25);
  }

  void small_impact() {
      auto p1 = random_vertex();
      auto radius = random_float() * 0.8 + 0.2;
      select_sphere(p1,radius,0.02f);
      auto td = -p1.normal * (random_float()+0.5) * 0.7;
      translate_selection(td * 0.2);
      select_sphere(p1,radius * 1.25,0.02f);
      translate_selection(-td * 0.1);
  }

};

#endif
