#version 450

layout(binding = 1) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
    mat4 viewProj;
} cam;

layout( push_constant ) uniform constants {
	mat4 model;
} PushConstants;

layout (location = 0) in vec2 inPosition;
layout (location = 1) in vec2 inUV;
layout (location = 2) in vec3 inInstancePos;
layout (location = 3) in vec2 inInstanceScale;
layout (location = 4) in vec3 inInstanceRotation;
layout (location = 5) in vec4 inInstanceColor;

layout(location = 0) out vec2 fragUV;
layout(location = 1) out vec4 fragColor;

mat4 calcModel(vec3 pos, vec2 scale, vec3 ang) {
    float cosX = cos(ang.x);
    float sinX = sin(ang.x);
    float cosY = cos(ang.y);
    float sinY = sin(ang.y);
    float cosZ = cos(ang.z);
    float sinZ = sin(ang.z);

    float m00 = cosY * cosZ + sinX * sinY * sinZ; 
    float m01 = cosY * sinZ - sinX * sinY * cosZ; 
    float m02 = cosX * sinY;
    float m03 = 0.0;

    float m04 = -cosX * sinZ; 
    float m05 = cosX * cosZ; 
    float m06 = sinX;
    float m07 = 0.0;

    float m08 = sinX * cosY * sinZ - sinY * cosZ;
    float m09 = -sinY * sinZ - sinX * cosY * cosZ;
    float m10 = cosX * cosY;
    float m11 = 0.0;

    float m12 = pos.x;
    float m13 = pos.y;
    float m14 = pos.z;// + float(gl_InstanceIndex) / 13000.0;
    float m15 = 1.0;

    //return mat4(
    //    m00, m01, m02, m03,
    //    m04, m05, m06, m07,
    //    m08, m09, m10, m11,
    //    m12, m13, m14, m15
    //);

    mat4 m;

    m[0] = vec4(m00, m01, m02, m03);
    m[1] = vec4(m04, m05, m06, m07);
    m[2] = vec4(m08, m09, m10, m11);
    m[3] = vec4(m12, m13, m14, m15);

    return m;

//    float sr = sin(rotation);
//    float cr = cos(rotation);
//
//    float m00 = scale.x *  cr;
//    float m10 = scale.x *  sr;
//    float m01 = scale.y * -sr;
//    float m11 = scale.y *  cr;
//    float m03 = pos.x;
//    float m13 = pos.y;
//    float m23 = pos.z;
//
//    return mat4(
//        m00, m10, 0.0, 0.0,
//        m01, m11, 0.0, 0.0,
//        0.0, 0.0, 1.0, 0.0,
//        m03, m13, m23, 1.0
//    );
}

void main() {
    mat4 transform = calcModel(inInstancePos, inInstanceScale, inInstanceRotation);

    gl_Position = cam.viewProj * PushConstants.model * transform * vec4(inPosition, 0.0, 1.0);
    fragUV = inUV;
    fragColor = inInstanceColor;
}