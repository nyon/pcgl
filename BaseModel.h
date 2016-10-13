#pragma once

#include <QString>
#include "Geometry/Polygon.h"
#include "Geometry/AABB.h"
#include "Geometry/Triangle.h"
#include "Geometry/Plane.h"
#include "Geometry/Sphere.h"
#include "Algorithm/Random/LCG.h"


#include <stack>
#include <limits>
#include <functional>
#include <vector>
#include <unordered_map>
#include <QDebug>
#include <QQueue>
#include <QtOpenGL>


#include "ModelBSPNode.h"
#include "ModelStates.h"
#include "ValueNoise.h"
#include "PerlinNoise.h"
#include "SurfacePoint.h"

class Model;

using namespace std;

class BaseModel
{
public:
    BaseModel();
    virtual ~BaseModel();

    Model *generate(const AABB& aabb);

    virtual void __init()=0;
    void __exit();



    void combine(function<void()> left, function<void()> right);
/*    void differ(function<void()> left, function<void()> right);
    void intersect(function<void()> left, function<void()> right);
*/



    void divide_x(function<void()> left, function<void()> right, float dividor = 0.5f);
    void divide_y(function<void()> left, function<void()> right, float dividor = 0.5f);
    void divide_z(function<void()> left, function<void()> right, float dividor = 0.5f);

    void evaluate(function<float (float, float, float)> implicit_func, float grid = 0.5f);

    void repeat(int times, function<void()> func);

    void sphere(float radius = 1.0f, int divisions = -1);
    void ellipsoid(float rx = 1.0f, float ry = 1.0f, float rz = 1.0f, int divisions = -1);

    vec random_vector();

    void scale_selection(const vec& vector);
    void scale_selection(float x, float y, float z) { scale_selection(vec(x,y,z)); }
    void scale_selection(float v) { scale_selection(vec(v,v,v)); }

    void translate_selection(const vec& vector);
    void translate_selection(float x, float y, float z) { translate_selection(vec(x,y,z)); }
    void translate_selection_along_normals(float amount);

    void recenter_selection();

    void rotate_x_selection(float rot);
    void rotate_y_selection(float rot);
    void rotate_z_selection(float rot);

    void randomize_selection(float amount = 0.1f);
    void invert_selection();

    void normalize(vec& vector);

    void apply_noise_field(function<float (float, float, float)> func, float displace = 1.0f);

    void select_all();
    void select_none();
    void select_sphere(const vec& vector, float radius, float randomness = 0.0f, float detail = 10.0f);
    void select_x_gradient(const vec& center, float w);
    void select_y_gradient(const vec& center, float w);
    void select_z_gradient(const vec& center, float w);

    void filter_selection_by_normal_angle(const vec& base_normal, float angle);

    void push_model_state();
    void push_clean_model_state();
    void pop_model_state();
    void pop_and_merge_model_state();


    SurfacePoint random_surface_point();
    SurfacePoint random_vertex();
    float random_float();
    vec random_point();

private:
    Model *model;

};

