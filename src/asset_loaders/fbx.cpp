#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <glm/vec3.hpp>

#include "asset_loaders/fbx.hpp"

#include "mesh.hpp"

void asset_loader_FBX::load(std::string_view path)
{
    Assimp::Importer importer;
    const aiScene* scene =
        importer.ReadFile(path.data(),
                          aiProcess_CalcTangentSpace | aiProcess_Triangulate |
                              aiProcess_JoinIdenticalVertices |
                              aiProcess_SortByPType | aiProcess_EmbedTextures);

    // TODO: check if the scene was loaded

    std::vector<vertex3d> vertices;
    std::vector<int> indices;
    std::vector<mesh::submesh_info> submeshes;

    for (int i = 0; i < scene->mNumMeshes; ++i)
    {
        mesh::submesh_info info;
        const aiMesh* assimp_mesh = scene->mMeshes[ i ];
        info.material_index = assimp_mesh->mMaterialIndex;
        info.vertex_index_offset = indices.size();

        for (int vertex_index = 0; vertex_index < assimp_mesh->mNumVertices;
             ++vertex_index)
        {
            vertices.push_back({});
            constexpr char position_name[] = "position";
            vertices.back().position() = {
                assimp_mesh->mVertices[ vertex_index ].x,
                assimp_mesh->mVertices[ vertex_index ].y,
                assimp_mesh->mVertices[ vertex_index ].z
            };
            vertices.back().normal() = {
                assimp_mesh->mNormals[ vertex_index ].x,
                assimp_mesh->mNormals[ vertex_index ].y,
                assimp_mesh->mNormals[ vertex_index ].z
            };
            vertices.back().uv() = {
                assimp_mesh->mTextureCoords[ 0 ][ vertex_index ].x,
                assimp_mesh->mTextureCoords[ 0 ][ vertex_index ].y,
            };
        }

        for (int face_index = 0; face_index < assimp_mesh->mNumFaces;
             ++face_index)
        {
            const aiFace& assimp_face = assimp_mesh->mFaces[ face_index ];
            for (int j = 0; j < assimp_face.mNumIndices; ++j)
                indices.push_back(assimp_face.mIndices[ j ]);
        }

        submeshes.push_back(std::move(info));
    }

    _mesh = new mesh;
    _mesh->set_vertices(std::move(vertices));
    _mesh->set_indices(std::move(indices));
    _mesh->set_submeshes(std::move(submeshes));
    _mesh->init();
}

mesh* asset_loader_FBX::get_mesh()
{
    return _mesh;
}
