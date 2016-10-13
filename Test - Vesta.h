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
      repeat(200, bind(&Test::small_impact, this));

      select_all();
      apply_noise_field(bind(&Test::noisy, this,  _1, _2, _3), 0.2f);
  }

  void asteroid() {
      ellipsoid(random_float()*2 + 5.2,random_float()*2 + 5.2,random_float()*2 + 5.2,8); // 9

      repeat(50,bind(&Test::impact, this));
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
      select_sphere(p1,radius,0.04);
      auto td = -p1.normal * (random_float()+0.5) * 0.7;
      translate_selection(td * 0.2);
      select_sphere(p1,radius * 1.25,0.04);
      translate_selection(-td * 0.1);
  }

};

#endif
