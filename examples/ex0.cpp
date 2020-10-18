#include <rnu/math/vec.hpp>
#include <rnu/math/quat.hpp>
#include <rnu/math/mat.hpp>
#include <rnu/camera.hpp>
#include <iostream>

namespace rnu
{
}

int main(int argc, char** argv)
{
    rnu::camera<float> cam;
    cam.axis(0, 0, 0, 0, 0, 0, 0);
    cam.matrix(true);
    cam.mouse(0, 0, 0);
    cam.projection(0, 0, 0, 0, 0);

    rnu::mat4 o;
    auto val = any_of(o != o);

    //   rnu::quat q = static_cast<rnu::quat>(rnu::quatd());
    //   q *= q;

    //   rnu::mat<float, 3, 3> matrix{
    //       2.f, 3.f, 4.f,
    //       1.f, 4.f, 4.f,
    //       9.f, 1.f, 3.f
    //   };
    //   rnu::vec3 a, b;
    //   auto x = matrix * (inverse(q) * ((a * 5) + (b * 2) + cross(a, b)));

    //   rnu::mat4i mm;
    //   mm *= (-!+~(mm));
    //   mm++;
    //   --mm;

       //rnu::mat<float, 3, 3> ttt{
       //	2, 0, 0,
       //	0, 2, 0,
       //	0, 0, 2
       //};
       //auto xax = det(inverse(ttt));

    //   rnu::vec3 aaaa = rnu::mat<float, 1, 3>();
    //   rnu::mat<float, 1, 3> aosp = aaaa;

    return 0;
}