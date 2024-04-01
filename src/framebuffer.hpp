#pragma once

#include <memory>

#include <glm/fwd.hpp>

class texture;

class framebuffer
{
private:
    struct private_data;

public:
    framebuffer();
    ~framebuffer();

    void initialize();
    void destroy();

    void set_samples(unsigned sample_count);

    std::shared_ptr<const texture> color_texture() const;
    std::shared_ptr<const texture> depth_texture() const;

    void copy_texture(texture* txt) const;

    void resize(glm::uvec2 size);

    void bind();
    static void unbind();

private:
    std::unique_ptr<private_data> _p;
};
