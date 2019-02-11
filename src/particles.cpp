#include "matrixMath.cpp"
#include "platform.h"
#include "graphics.cpp"
#include "memory.h"

// TODO: Organize this stuff so there can be multiple emitters

struct cool_thing_uniform_list {
    uniform MVP;
    uniform ModelMatrix;
    uniform ViewMatrix;
    uniform ProjectionMatrix;
    uniform Time;
    uniform Period;
};
cool_thing_uniform_list CoolThingUniforms;
cool_thing_uniform_list SetupCoolThingUniforms(game_data *Game, GLuint coolProgram) {
    
    cool_thing_uniform_list UniformList = {};
    UniformList.MVP = CreateUniform(coolProgram, UniformMatrix4fv, "MVP");
    UniformList.ModelMatrix = CreateUniform(coolProgram, UniformMatrix4fv, "uModelMatrix");
    UniformList.ViewMatrix = CreateUniform(coolProgram, UniformMatrix4fv, "uViewMatrix");
    UniformList.ProjectionMatrix = CreateUniform(coolProgram, UniformMatrix4fv, "uProjectionMatrix");
    UniformList.Time = CreateUniform(coolProgram, Uniform1fv, "uTime");
    UniformList.Period = CreateUniform(coolProgram, Uniform1fv, "uTimePeriod");
    
    return UniformList;
}

GLuint CoolVAO;
GLuint CoolProgram;
int NumParticles = 10000;
int VerticesPerParticle = 4;
int NumVertices = NumParticles*VerticesPerParticle;
int TrianglesPerParticle = 2;
int IndicesPerParticle = TrianglesPerParticle*3;
int NumIndices = IndicesPerParticle*NumParticles;

float TimePeriod = 10.0f;
v3 CoolPosition = V3(0.0, 0.0, 0.0);

uint32* CoolIndices;
GLuint CoolVBO;
GLuint CoolIndexVBO;
index_buffer_info CoolIndexBuffer;

struct CoolVertex
{
    v3 Position;
    v3 ParticlePosition;
    v4 Color;
    v2 Uv;
    float TimeOffset;
    v4 Velocity;
};
CoolVertex* CoolVertex_ = (CoolVertex*)0;
CoolVertex* CoolVertices;

void InitializeCoolParticles(int ParticleCount, CoolVertex* Vertices) {
    float particleRadius = .1f;
    float halfRadius = particleRadius/2.0f;
    float spreadRadius = .5f;
    
    for(int particleIndex = 0, i = 0; particleIndex < ParticleCount; particleIndex++) {
        float radius = Random(0.0f, spreadRadius);
        float theta = Random(0.0f, 2*PI);
        v3 ParticlePosition = V3(sin(theta)*radius, Random(-spreadRadius, 0.0f), cos(theta)*radius);
        v4 ParticleColor = V4(Random(.25, 1.0f), Random(.25, 1.0f), Random(.25, 1.0f), 1.0f);
        float TimeOffset = Random(0.0, TimePeriod);
        float AngularVelocity = 2.0f;
        AngularVelocity *= Random(2)==0?-1:1;
        
        Vertices[i].Color = ParticleColor;
        Vertices[i].Uv = V2(0.0f, 0.0f);
        Vertices[i].TimeOffset = TimeOffset;
        Vertices[i].Velocity = V4(0.0, 1.0, 0.0, AngularVelocity);
        Vertices[i].ParticlePosition = ParticlePosition;
        Vertices[i++].Position = V3(-halfRadius, halfRadius, 0.0f) + ParticlePosition;
        Vertices[i].Color = ParticleColor;
        Vertices[i].Uv = V2(1.0f, 0.0f);
        Vertices[i].TimeOffset = TimeOffset;
        Vertices[i].Velocity = V4(0.0, 1.0, 0.0, AngularVelocity);
        Vertices[i].ParticlePosition = ParticlePosition;
        Vertices[i++].Position = V3(halfRadius, halfRadius, 0.0f) + ParticlePosition;
        Vertices[i].Color = ParticleColor;
        Vertices[i].Uv = V2(0.0f, 1.0f);
        Vertices[i].TimeOffset = TimeOffset;
        Vertices[i].Velocity = V4(0.0, 1.0, 0.0, AngularVelocity);
        Vertices[i].ParticlePosition = ParticlePosition;
        Vertices[i++].Position = V3(-halfRadius, -halfRadius, 0.0f) + ParticlePosition;
        Vertices[i].Color = ParticleColor;
        Vertices[i].Uv = V2(1.0f, 1.0f);
        Vertices[i].TimeOffset = TimeOffset;
        Vertices[i].Velocity = V4(0.0, 1.0, 0.0, AngularVelocity);
        Vertices[i].ParticlePosition = ParticlePosition;
        Vertices[i++].Position = V3(halfRadius, -halfRadius, 0.0f) + ParticlePosition;
    }
}

void InitializeCoolThing(game_data *Game)
{
    glGenVertexArrays(1, &CoolVAO);
    glBindVertexArray(CoolVAO);
    
    glGenBuffers(1, &CoolIndexVBO);
    GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, CoolIndexVBO));
    CoolIndices = PushArray(&Game->MainArena, NumIndices, uint32);
    for(uint32 Index = 0, ParticleIndex = 0; ParticleIndex < NumParticles; ParticleIndex++) {
        int VertexIndex = ParticleIndex*VerticesPerParticle;
        CoolIndices[Index++] = VertexIndex + 0;
        CoolIndices[Index++] = VertexIndex + 2;
        CoolIndices[Index++] = VertexIndex + 1;
        CoolIndices[Index++] = VertexIndex + 2;
        CoolIndices[Index++] = VertexIndex + 3;
        CoolIndices[Index++] = VertexIndex + 1;
    }    
    GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(CoolIndices[0])*NumIndices, CoolIndices, GL_STATIC_DRAW));
    
    GL(glGenBuffers(1, &CoolVBO));
    GL(glBindBuffer(GL_ARRAY_BUFFER, CoolVBO));
    CoolVertices = PushArray(&Game->MainArena, NumVertices, CoolVertex);
    InitializeCoolParticles(NumParticles, CoolVertices);
    GL(glBufferData(GL_ARRAY_BUFFER, sizeof(CoolVertex)*NumVertices, CoolVertices, GL_DYNAMIC_DRAW));
    
    GL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(CoolVertex), (GLvoid*)(&CoolVertex_->Position)));
    GL(glEnableVertexAttribArray(0));
    GL(glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(CoolVertex), (GLvoid*)(&CoolVertex_->Color)));
    GL(glEnableVertexAttribArray(1));
    GL(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(CoolVertex), (GLvoid*)(&CoolVertex_->Uv)));
    GL(glEnableVertexAttribArray(2));
    GL(glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(CoolVertex), (GLvoid*)(&CoolVertex_->TimeOffset)));
    GL(glEnableVertexAttribArray(3));
    GL(glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(CoolVertex), (GLvoid*)(&CoolVertex_->Velocity)));
    GL(glEnableVertexAttribArray(4));
    GL(glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(CoolVertex), (GLvoid*)(&CoolVertex_->ParticlePosition)));
    GL(glEnableVertexAttribArray(5));
    
    GL(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GL(glBindVertexArray(0));
    
    CoolProgram = LoadShaders(&Game->TempArena, "../res/Shaders/coolShader.vert", "../res/Shaders/coolShader.frag");
    CoolThingUniforms = SetupCoolThingUniforms(Game, CoolProgram);
}
float CoolT = 0.0f;
void UpdateCoolThing(float dT, game_data *Game)
{
    CoolT += dT;
    while(CoolT >= TimePeriod)
    {
        CoolT -= TimePeriod;
    }
    CoolThingUniforms.Time.Value = &CoolT;
}

void DrawCoolThing(game_data* Game, mat4 Projection, mat4 View)
{
    GL(glUseProgram(CoolProgram));
    glDepthMask(false);
    //Set uniforms
    mat4 Rotation = Identity4x4();
    mat4 Scale = MakeScale(V3(1.0f, 1.0f, 1.0f));
    mat4 Translation = MakeTranslation(CoolPosition);
    mat4 ModelTransform = Translation * Rotation * Scale;
    mat4 MVP = Projection * View * ModelTransform;
    CoolThingUniforms.MVP.Value = (void*)(&MVP.E[0][0]);
    CoolThingUniforms.ModelMatrix.Value = (void*)(&ModelTransform.E[0][0]);
    CoolThingUniforms.ProjectionMatrix.Value = (void*)(&Projection.E[0][0]);
    CoolThingUniforms.ViewMatrix.Value = (void*)(&View.E[0][0]);
    
    GL(SetUniform(CoolThingUniforms.MVP));
    GL(SetUniform(CoolThingUniforms.ModelMatrix));
    GL(SetUniform(CoolThingUniforms.ProjectionMatrix));
    GL(SetUniform(CoolThingUniforms.ViewMatrix));
    GL(glUniform1f(CoolThingUniforms.Time.Location, CoolT));
    GL(glUniform1f(CoolThingUniforms.Period.Location, TimePeriod));
    glEnable (GL_BLEND);
    glBlendFunc (GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    GL(glBindVertexArray(CoolVAO));
    
    GL(glDrawElements(GL_TRIANGLES,
                      NumIndices,
                      GL_UNSIGNED_INT,
                      (void*)0));                  
    
    GL(glBindVertexArray(0));
    glDisable(GL_BLEND);
    glDepthMask(true);
}