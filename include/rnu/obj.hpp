#pragma once

#include <array>
#include <string>
#include <optional>
#include <filesystem>
#include <vector>
#include <memory>
#include <rnu/math/math.hpp>

namespace rnu
{
  using obj_vec3 = vec3;
  using obj_vec2 = vec2;
  using obj_face = vec3ui32;

  struct face_t
  {
    std::vector<obj_face> vertices;
  };


  struct material_t
  {
    std::string name;
    float specularity;
    obj_vec3 ambient;
    obj_vec3 diffuse;
    obj_vec3 specular;
    obj_vec3 emissive;
    float ior;
    float dissolve;
    int illumination_model;

    std::string map_diffuse;
  };

  struct vertex_group_t
  {
    std::string name;
    std::vector<face_t> faces;
    std::shared_ptr<material_t> material;
  };

  struct object_t
  {
    std::string name;

    std::vector<vertex_group_t> groups;

    std::vector<obj_vec3> positions;
    std::vector<obj_vec3> normals;
    std::vector<obj_vec2> texcoords;
  };
  std::vector<object_t> load_obj(std::filesystem::path const& obj_file);

  struct triangulated_object_t
  {
    std::string name;
    std::vector<std::uint32_t> indices;
    std::vector<obj_vec3> positions;
    std::vector<obj_vec3> normals;
    std::vector<obj_vec2> texcoords;
    std::shared_ptr<material_t> material;
  };
  std::vector<triangulated_object_t> triangulate(object_t const& object);

  void join_into(triangulated_object_t& dst, triangulated_object_t const& src);
}