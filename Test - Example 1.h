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


  float lines(float x, float y, float z) {
      return  noise1(y*10.0f) * 0.1f;
  }


  void scene() {
      ellipsoid(5,7,5);

      apply_noise_field(bind(&Test::lines, this,  _1, _2, _3));
      apply_noise_field(bind(&Test::lines, this,  _1, _2, _3));
      apply_noise_field(bind(&Test::lines, this,  _1, _2, _3));



/*      push_clean_model_state();
      left_asteroid_with_handle();
      pop_and_merge_model_state();

      push_clean_model_state();
      right_asteroid();
      pop_and_merge_model_state();

      repeat(300,bind(&Test::small_impact, this));
      select_all();
      apply_noise_field(bind(&Test::noisy, this,  _1, _2, _3));
*/
  }

  void left_asteroid_with_handle() {
      push_clean_model_state();
      left_asteroid();
      pop_and_merge_model_state();

      push_clean_model_state();
      handle();
      pop_and_merge_model_state();
  }

  void handle() {
      sphere(5,7); // 7
      select_all();
      scale_selection(random_float()*0.9 + 1.0, random_float()*0.3 + 0.2, random_float()*0.3 + 0.2);
      repeat(300,bind(&Test::impact, this));



  }


  void left_asteroid() {
      sphere(5,7); // 7
      select_all();
      scale_selection(random_float()*0.6 + 0.8, random_float()*0.2 + 0.6, random_float()*0.2 + 0.6);
      rotate_x_selection(random_float()*20);
      select_none();
      repeat(300,bind(&Test::impact, this));



      select_all();
      translate_selection(-5.5,random_float() * 2,0);

  }

  void right_asteroid() {
      sphere(5,7);
      select_all();
      scale_selection(random_float()*0.6 + 0.8, random_float()*0.2 + 0.6, random_float()*0.2 + 0.6);
      rotate_x_selection(random_float()*20);
      select_none();
      repeat(300,bind(&Test::impact, this));



      select_all();
      translate_selection(5.5,random_float() * -2,0);

  }

  float noisy(float x, float y, float z) {
      return noise3(x*39.2f,y*39.2f,z*39.2f)*noise3(x*5.2f,y*5.2f,z*5.2f) * 0.2f * max(0.0f, noise3(x,y,z)-0.5f);
  }

  void impact() {
    auto p1 = random_point();
    select_sphere(p1,random_float() * 4 + 1,0.01);
    normalize(p1);
    translate_selection(-p1 * random_float() * 0.5);
    select_none();
  }

  void small_impact() {
      auto p1 = random_surface_point();
      auto p2 = p1;
      auto radius = random_float() * 0.2 + 0.2;
      select_sphere(p2,radius,0.2);
      normalize(p1);
      auto td = -p1 * (random_float()+0.5) * 1.2;
      translate_selection(td * 0.2);
      select_sphere(p2,radius * 1.25,0.2);
      randomize_selection(0.1f);
      translate_selection(td * -0.2);
      select_sphere(p2,radius,0.2);
      translate_selection(td * 0.1);
  }

};

#endif
