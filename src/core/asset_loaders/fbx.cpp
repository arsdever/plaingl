#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <glm/vec3.hpp>

#include "core/asset_loaders/fbx.hpp"

#include "assimp/quaternion.h"
#include "common/file.hpp"
#include "common/logging.hpp"
#include "core/asset_manager.hpp"
#include "graphics/material.hpp"
#include "graphics/mesh.hpp"

namespace
{
logger log() { return get_logger("fbx_loader"); }
} // namespace

glm::vec3 convert(aiVector3D ai_vec3)
{
    return { ai_vec3.x, ai_vec3.y, ai_vec3.z };
}

glm::quat convert(aiQuaternion ai_quat)
{
    return { ai_quat.w, ai_quat.x, ai_quat.y, ai_quat.z };
}

glm::vec3 convert(aiColor3D ai_quat)
{
    return { ai_quat.r, ai_quat.g, ai_quat.b };
}

void asset_loader_FBX::load(std::string_view path)
{
    if (!common::file::exists(path))
    {
        log()->warn("File does not exist: {}", path);
    }

    Assimp::Importer importer;
    const aiScene* ai_scene =
        importer.ReadFile(path.data(),
                          aiProcess_CalcTangentSpace | aiProcess_Triangulate |
                              aiProcess_JoinIdenticalVertices |
                              aiProcess_SortByPType | aiProcess_EmbedTextures);

    if (!ai_scene)
    {
        log()->error("Failed to load: {}", path);
        return;
    }

    std::queue<aiNode*> dfs_queue;
    dfs_queue.push(ai_scene->mRootNode);
    while (!dfs_queue.empty())
    {
        aiNode* node = dfs_queue.front();
        dfs_queue.pop();
        for (int i = 0; i < node->mNumChildren; ++i)
        {
            dfs_queue.push(node->mChildren[ i ]);
        }

        log()->debug(
            "Node: {} meshes: {}", node->mName.C_Str(), node->mNumMeshes);
        if (node->mNumMeshes > 0)
        {
            mesh* m = nullptr;
            {
                std::vector<const aiMesh*> ai_submeshes;
                for (int i = 0; i < node->mNumMeshes; ++i)
                {
                    ai_submeshes.push_back(
                        ai_scene->mMeshes[ node->mMeshes[ i ] ]);
                }

                m = load_mesh(std::move(ai_submeshes));
                asset_manager::default_asset_manager()->register_asset(
                    std::format("{}_mesh", node->mName.C_Str()), m);
            }
            material* mat = new material;
            mat->set_shader_program(
                asset_manager::default_asset_manager()->get_shader("standard"));
            mat->declare_property("u_albedo_texture_strength",
                                  material_property::data_type::type_float);
            mat->declare_property(
                "u_albedo_color",
                material_property::data_type::type_float_vector_4);
            mat->declare_property("u_normal_texture_strength",
                                  material_property::data_type::type_float);
            mat->declare_property("u_model_matrix",
                                  material_property::data_type::unknown);
            mat->declare_property("u_vp_matrix",
                                  material_property::data_type::unknown);
            mat->set_property_value("u_albedo_texture_strength", 0.0f);
            mat->set_property_value(
                "u_albedo_color", 0.8f, 0.353f, 0.088f, 1.0f);
            mat->set_property_value("u_normal_texture_strength", 0.0f);
        }
    }
}

mesh* asset_loader_FBX::load_mesh(std::vector<const aiMesh*> ai_submeshes)
{
    mesh* result;
    std::vector<vertex3d> vertices;
    std::vector<int> indices;
    std::vector<mesh::submesh_info> submeshes;

    for (auto ai_mesh : ai_submeshes)
    {
        mesh::submesh_info info;
        info.material_index = ai_mesh->mMaterialIndex;
        info.vertex_index_offset = indices.size();

        for (int vertex_index = 0; vertex_index < ai_mesh->mNumVertices;
             ++vertex_index)
        {
            vertices.push_back({});
            constexpr char position_name[] = "position";
            vertices.back().position() = {
                ai_mesh->mVertices[ vertex_index ].x,
                ai_mesh->mVertices[ vertex_index ].y,
                ai_mesh->mVertices[ vertex_index ].z
            };
            vertices.back().normal() = { ai_mesh->mNormals[ vertex_index ].x,
                                         ai_mesh->mNormals[ vertex_index ].y,
                                         ai_mesh->mNormals[ vertex_index ].z };
            vertices.back().uv() = {
                ai_mesh->mTextureCoords[ 0 ][ vertex_index ].x,
                ai_mesh->mTextureCoords[ 0 ][ vertex_index ].y,
            };

            vertices.back().tangent() = {
                ai_mesh->mTangents[ vertex_index ].x,
                ai_mesh->mTangents[ vertex_index ].y,
                ai_mesh->mTangents[ vertex_index ].z
            };

            vertices.back().bitangent() = {
                ai_mesh->mBitangents[ vertex_index ].x,
                ai_mesh->mBitangents[ vertex_index ].y,
                ai_mesh->mBitangents[ vertex_index ].z
            };
        }

        for (int face_index = 0; face_index < ai_mesh->mNumFaces; ++face_index)
        {
            const aiFace& assimp_face = ai_mesh->mFaces[ face_index ];
            for (int j = 0; j < assimp_face.mNumIndices; ++j)
                indices.push_back(assimp_face.mIndices[ j ] +
                                  info.vertex_index_offset);
        }

        submeshes.push_back(std::move(info));
    }

    result = new mesh;
    result->set_vertices(std::move(vertices));
    result->set_indices(std::move(indices));
    result->set_submeshes(std::move(submeshes));
    result->init();

    return result;
}
