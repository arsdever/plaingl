#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <glm/vec3.hpp>

#include "asset_loaders/fbx.hpp"

#include "assimp/quaternion.h"
#include "camera.hpp"
#include "light.hpp"
#include "mesh.hpp"

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

    // TODO: there can be multiple separate meshes

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

    for (int i = 0; i < scene->mNumCameras; ++i)
    {
        auto cam = new camera;
        const auto& aiCam = *scene->mCameras[ i ];
        const auto& aiCamNode = *scene->mRootNode->FindNode(aiCam.mName);
        aiMatrix4x4 camMat;
        aiCam.GetCameraMatrix(camMat);
        aiMatrix4x4 final = aiCamNode.mTransformation;
        final *= camMat;
        aiVector3D pos;
        aiVector3D scale;
        aiQuaternion rot;
        final.Decompose(scale, rot, pos);
        cam->set_fov(aiCam.mHorizontalFOV * 2.0f);
        cam->set_ortho(false);
        auto& t = cam->get_transform();
        t.set_position(convert(pos) / convert(scale));
        t.set_rotation(convert(rot));
    }

    for (int i = 0; i < scene->mNumLights; ++i)
    {
        auto l = new light;
        const auto& al = *scene->mLights[ i ];
        const auto& aln = *scene->mRootNode->FindNode(al.mName);
        aiMatrix4x4 mat = aln.mTransformation;
        aiVector3D pos;
        aiVector3D scale;
        aiQuaternion rot;
        mat.Decompose(scale, rot, pos);
        l->get_transform().set_position(convert(al.mPosition + pos) / 100.0f);
        glm::vec3 combined_intensity_color = convert(al.mColorDiffuse);
        auto intensity = std::max(
            std::max(combined_intensity_color.x, combined_intensity_color.y),
            combined_intensity_color.z);
        l->set_color(combined_intensity_color / intensity);
        l->set_intensity(intensity / 100);
    }
}

mesh* asset_loader_FBX::get_mesh() { return _mesh; }
