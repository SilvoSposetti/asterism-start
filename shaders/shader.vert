#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject{
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

void main(){
    vec4 modelPos = vec4(inPosition, 1.0);
    gl_Position = ubo.proj * ubo.view * ubo.model * modelPos;
    fragColor = inColor;
}