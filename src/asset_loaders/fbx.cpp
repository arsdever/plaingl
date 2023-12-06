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

    for (int i = 0; i < scene->mNumMeshes; ++i)
    {
        const aiMesh* assimp_mesh = scene->mMeshes[ i ];

        mesh* mesh_ = new mesh;
        std::vector<vertex> vertices;
        std::vector<int> indices;

        for (int vertex_index = 0; vertex_index < assimp_mesh->mNumVertices;
             ++vertex_index)
        {
            vertices.push_back({});
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

        mesh_->set_vertices(std::move(vertices));
        mesh_->set_indices(std::move(indices));
        mesh_->init();
        _meshes.push_back(mesh_);
    }
}

const std::vector<mesh*>& asset_loader_FBX::get_meshes() const {
    return _meshes;
}

std::vector<mesh*>&& asset_loader_FBX::extract_meshes() {
    return std::move(_meshes);
}
