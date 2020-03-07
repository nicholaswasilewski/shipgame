#ifndef _GAME_H__
#define _GAME_H__

#include "graphics.cpp"
#include "renderer.cpp"
#include "unit.h"

struct game_object2
{
    model2 *Model;
    v3 Scale;
    v3 Position;
    v3 Axis;
    float Angle;
};

struct color_game_object
{
    color_model *ColorModel;
    v3 Scale;
    v3 Position;
    v3 Axis;
    float Angle;
};

struct game_data
{
    bool Initialized;
    memory_arena MainArena;
    memory_arena TempArena;
    
    void* GameScene;
    
    light_texture_shader LightTextureShader;
    color_shader ColorShader;
    water_shader WaterShader;
    skybox_shader SkyBoxShader;
    circle_shader CircleShader;
    camera Camera;
    
    texture BoxDiffuseMap;
    texture BoxSpecularMap;
    texture BoxEmissiveMap;
    texture_color_material BoxMaterial;
    texture_color_material ColorMaterial;
    model2 BoxModel;
    model2 BoxModel2;
    model2 MonkeyModel;
    
    //Scene
    light Light;
    game_object2 Box2;
    game_object2 LightBox;
    
    game_object2 Box3;
    
    GLuint CircleDataBufferBinding;
    model2 QuadModel;
    game_object2 Quad;
    
    //New Scene
    game_object2 Player;
    color_game_object Water;
    color_model WaterColorModel;
    texture_color_material WaterColorMaterial;
    texture WaterNormalMap;
    texture WaterDuDvMap;
    skybox SkyBox;
    GLuint ReflectionFBO;
    
    postprocessor Postprocessor;

    renderer Renderer;
	units Units;
};

#endif
