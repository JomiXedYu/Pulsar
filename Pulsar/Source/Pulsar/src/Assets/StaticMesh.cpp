﻿#include "Assets/StaticMesh.h"

#include "Application.h"

#include "EngineMath.h"
#include <Pulsar/Assets/Shader.h>
#include <Pulsar/Assets/Texture2D.h>

namespace pulsar
{
    void StaticMesh::OnInstantiateAsset(AssetObject* obj)
    {
        base::OnInstantiateAsset(obj);
        auto mesh = static_cast<ThisClass*>(obj);
        mesh->m_sections = m_sections;
    }
    void StaticMesh::CreateGPUResource()
    {
        if(m_isCreatedResource)
        {
            return;
        }
        m_isCreatedResource = true;
        for(auto& section : m_sections)
        {
            auto vertSize = section.Vertex.size() * kSizeofStaticMeshVertex;
            auto vert = Application::GetGfxApp()->CreateBuffer(gfx::GFXBufferUsage::Vertex, vertSize);
            m_vertexBuffers.push_back(vert);

            auto indicesSize = section.Indices.size() * sizeof(decltype(section.Indices)::value_type);
            auto indices = Application::GetGfxApp()->CreateBuffer(gfx::GFXBufferUsage::Index, vertSize);
            m_indicesBuffers.push_back(indices);
        }
    }
    void StaticMesh::DestroyGPUResource()
    {
        if(!m_isCreatedResource)
            return;

        m_vertexBuffers.clear();
        m_indicesBuffers.clear();
        m_isCreatedResource = false;
    }
    bool StaticMesh::IsCreatedGPUResource() const
    {
        return m_isCreatedResource;
    }

    StaticMesh::~StaticMesh()
    = default;

    void StaticMesh::Serialize(AssetSerializer* s)
    {
        base::Serialize(s);
        sser::ReadWriteStream(s->Stream, s->IsWrite, m_sections);
        sser::ReadWriteStream(s->Stream, s->IsWrite, m_materialNames);
    }


    ObjectPtr<StaticMesh> StaticMesh::StaticCreate(
        string_view name,
        array_list<StaticMeshSection>&& vertData,
        array_list<string>&& materialNames)
    {
        StaticMesh_sp self = mksptr(new StaticMesh);
        self->Construct();
        self->m_name = name;
        self->m_sections = std::move(vertData);
        self->m_materialNames = std::move(materialNames);

        return self;
    }


    std::iostream& ReadWriteStream(std::iostream& stream, bool isWrite, StaticMeshVertex& data)
    {
        ReadWriteStream(stream, isWrite, data.Position);
        ReadWriteStream(stream, isWrite, data.Normal);
        ReadWriteStream(stream, isWrite, data.Tangent);
        ReadWriteStream(stream, isWrite, data.Bitangent);
        ReadWriteStream(stream, isWrite, data.Color);
        sser::ReadWriteStream(stream, isWrite, data.TexCoords);
        return stream;
    }

    std::iostream& ReadWriteStream(std::iostream& stream, bool isWrite, StaticMeshSection& data)
    {
        sser::ReadWriteStream(stream, isWrite, data.Vertex);
        sser::ReadWriteStream(stream, isWrite, data.Indices);
        sser::ReadWriteStream(stream, isWrite, data.MaterialIndex);
        return stream;
    }

}