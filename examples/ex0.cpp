#include <rnu/math/math.hpp>
#include <iostream>
#include <chrono>
#include <functional>

int main(int argc, char** argv)
{
    using XX = rnu::reference_type<const rnu::vec3>;

    rnu::vec3 vector;

    rnu::mat2 mat;
    rnu::mat2 mat2;
    rnu::mat2i mat32;
    mat32 << 1;
    
    rnu::mat2 matrix(1, 2, 20.f, 234);

    mat = rnu::element_wise([](float f, float b) {
        return f + b;
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
    {
        std::vector<rnu::mat2> vectors(100000);
        std::generate(vectors.begin(), vectors.end(), [] { return rnu::mat2{ float(rand()), float(rand()), float(rand()), float(rand()) }; });
        auto start = std::chrono::steady_clock::now();
        for (size_t i = 0; i < 100000; ++i)
        {
            float faf[4];
            faf[0] = vectors[i].at(0, 0) + vectors[i].at(0, 0);
            faf[1] = vectors[i].at(0, 1) + vectors[i].at(0, 1);
            faf[2] = vectors[i].at(1, 0) + vectors[i].at(1, 0);
            faf[3] = vectors[i].at(1, 1) + vectors[i].at(1, 1);
            vectors[i].at(0, 0) = faf[0];
            vectors[i].at(0, 1) = faf[1];
            vectors[i].at(1, 0) = faf[2];
            vectors[i].at(1, 1) = faf[3];
        }
        std::cout << "Time: " << (std::chrono::steady_clock::now() - start).count() << " nanos\n";
    }

    {
        std::vector<rnu::mat2> vectors(100000);
        std::generate(vectors.begin(), vectors.end(), [] { return rnu::mat2{ float(rand()), float(rand()), float(rand()), float(rand()) }; });
        auto start = std::chrono::steady_clock::now();
        for (size_t i = 0; i < 100000; ++i)
        {
            vectors[i] = rnu::element_wise([](float a) { return a + a; }, vectors[i]);
        }
        std::cout << "Time: " << (std::chrono::steady_clock::now() - start).count() << " nanos\n";
    }

    //rnu::vec4i vack;
    //vack % 9;
    //--vack;

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