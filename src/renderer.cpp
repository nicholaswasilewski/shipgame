#ifndef _RENDERER_CPP__
#define _RENDERER_CPP__

#include "numerical_types.h"
#include "hash.cpp"
#include "matrixMath.cpp"
#include "entity.cpp"
#include "glHelper.cpp"

typedef GLuint vertex_array_handle;
typedef GLuint shader_handle;
typedef GLuint texture_handle;

#define MAX_TEXTURE_UNITS 4

// Maybe this should be generalized to a graphics resource handle?
struct mesh
{
		vertex_array_handle Id;
		GLenum Primitive;
		GLuint Count;
};

struct renderer_texture
{
		texture_handle Id;
		GLenum Unit;
		GLenum Target;
};

struct renderer_shader
{
    shader_handle Id;
};

struct vertices
{
//    VertexFormat; // this is going to have to be its own list of vertex properties I think.
    int vertex_data_count;
    void* vertex_data;
};

struct render_component
{
    v3 Scale;
    v3 Position;
    v3 Axis;
    float Angle;

    renderer_shader Shader;
    mesh Mesh;
		renderer_texture Textures[MAX_TEXTURE_UNITS]; // Null terminated texture thingies.
    // uniforms should be put into the render data area.
};

struct renderer
{
    hashtable IndexToComponent;
    uint32 ClearFlags;
		
    int Capacity;
    int Count;
    render_component* Components;
		size_t LightGroupOffset;
};

render_component* AddEntity(renderer* Renderer, memory_arena* Arena, entity Entity)
{
    if (Renderer->Count < Renderer->Capacity)
    {
				int Index = Renderer->Count++;
				Renderer->Components[Index] = {0};
				Insert(Renderer->IndexToComponent, Entity.Index, Index);
				return &Renderer->Components[Index];
    }
		else
		{
				printf("Renderer capacity reached.");
				return 0;
		}
}

void ApplyDefaultRendererState()
{
    GL(glFrontFace(GL_CCW));
    GL(glEnable(GL_CULL_FACE));
    GL(glEnable(GL_DEPTH_TEST));
    GL(glDepthFunc(GL_LESS));
}

void SetClearColor(color4 color)
{
    GL(glClearColor(color.r, color.g, color.b, color.a));
}

void ClearScreen(renderer* Renderer)
{
    GL(glClear(Renderer->ClearFlags));
}

void Initialize(renderer* Renderer, memory_arena* Arena)
{
		const uint32 COLOR_BUFFER_BIT = GL_COLOR_BUFFER_BIT;
    const uint32 DEPTH_BUFFER_BIT = GL_DEPTH_BUFFER_BIT;
    Renderer->ClearFlags = COLOR_BUFFER_BIT | DEPTH_BUFFER_BIT;
    Renderer->Capacity = 10;
		Renderer->IndexToComponent = CreateHashTable(Renderer->Capacity, Arena);
    Renderer->Count = 0;
		Renderer->Components = PushArray(Arena, 10, render_component);
}

// Need a way to do lights, need a way to store uniforms without having to do lookups every frame.
void Draw(renderer* Renderer, memory_arena* Arena, render_component Component, mat4 Projection, mat4 View)
{
		GLuint ShaderId = Component.Shader.Id;
		GL(glUseProgram(ShaderId));
		for (int i = 0; i < MAX_TEXTURE_UNITS; i++) {
				if (Component.Textures[i].Id == 0) {
						break;
				}
				GL(glActiveTexture(Component.Textures[i].Unit));
				GL(glBindTexture(Component.Textures[i].Target, Component.Textures[i].Id));
		}
		
		Assert(ShaderId);

		GLint LightPower = GL(glGetUniformLocation(ShaderId, "Light.Power"));
		GLint LightPosition = GL(glGetUniformLocation(ShaderId, "Light.Position"));
    GLint LightAmbient = GL(glGetUniformLocation(ShaderId, "Light.Ambient"));
    GLint LightDiffuse = GL(glGetUniformLocation(ShaderId, "Light.Diffuse"));
    GLint LightSpecular = GL(glGetUniformLocation(ShaderId, "Light.Specular"));

		GLint M = GL(glGetUniformLocation(ShaderId, "M"));
		GLint V = GL(glGetUniformLocation(ShaderId, "V"));
		GLint MVP = GL(glGetUniformLocation(ShaderId, "MVP"));
		
		GLint Diffuse = GL(glGetUniformLocation(ShaderId, "Material.Diffuse"));
		GLint Specular = GL(glGetUniformLocation(ShaderId, "Material.Specular"));
		GLint Emissive = GL(glGetUniformLocation(ShaderId, "Material.Emissive"));
		GLint Shine = GL(glGetUniformLocation(ShaderId, "Material.Shine"));
		
		mat4 Rotation = MakeRotation(Component.Axis, Component.Angle);
		mat4 Scale = MakeScale(Component.Scale);
		mat4 Translation = MakeTranslation(Component.Position);
		mat4 ModelTransform = Translation * Rotation * Scale;
		mat4 MvpValue = Projection * View * ModelTransform;
		
		GL(glUniformMatrix4fv(M, 1, GL_FALSE, &ModelTransform.E[0][0]));
		GL(glUniformMatrix4fv(V, 1, GL_FALSE, &View.E[0][0]));
		GL(glUniformMatrix4fv(MVP, 1, GL_FALSE, &MvpValue.E[0][0]));
		GL(glBindVertexArray(Component.Mesh.Id));
		GL(glDrawElements(Component.Mesh.Primitive, Component.Mesh.Count, GL_UNSIGNED_SHORT, (void*)0));
		
}

void Draw(renderer* Renderer, memory_arena* Arena, mat4 Projection, mat4 View)
{
		for(int i = 0; i < Renderer->Count; i++) {
				Draw(Renderer, Arena, Renderer->Components[i], Projection, View);
		}
}

#endif
