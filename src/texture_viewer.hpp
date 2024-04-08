#pragma once

class image;
class texture;

class texture_viewer
{
public:
    static void show_preview(image* img);
    static void show_preview(texture* txt);
};
