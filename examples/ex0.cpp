#include <rnu/math/math.hpp>
#include <iostream>

#include <functional>


int main(int argc, char** argv)
{
    using XX = rnu::reference_type<const rnu::vec3>;

    rnu::vec3 vector;

    rnu::mat4 mat;
    rnu::mat4 mat2;
    
    rnu::element_wise([](float& f, float b) {
        f += b;
        }, mat, mat2);
    auto x = rnu::element_wise([](float& f, float& fa) { return f += fa; }, vector, vector);

    rnu::quat quatasd;
    rnu::quatd d = rnu::element_wise([](float& r, float q) { 
        r += q;
        return double(r); }, quatasd, quatasd);

    (vector + vector * vector - vector / vector) + 10;
    vector + 5;
    5 + vector;
    vector += 210;

    auto vav = vector == vector;

    rnu::vec4i vack;
    vack % 9;
    --vack;

    /*rnu::camera<float> cam;
    cam.axis(0, 0, 0, 0, 0, 0, 0);
    cam.matrix(true);
    cam.mouse(0, 0, 0);
    cam.projection(0, 0, 0, 0, 0);

    rnu::mat4 o;
    auto val = any_of(o != o);*/

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