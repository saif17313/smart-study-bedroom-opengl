#version 330 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
flat out vec3 litColor;

uniform vec3 materialColor;
uniform float materialSpecular;
uniform float shininess;
uniform vec3 cameraPosition;
uniform vec3 ambientColor;
struct Light {
    vec3 position;
    vec3 color;
    vec3 attenuation;
    vec3 direction;
    float spot;
};
uniform Light lights[4];

// Identical equation in Flat, Gouraud and Phong; only evaluation stage differs.
vec3 shade(vec3 position, vec3 normal) {
    vec3 result = materialColor * ambientColor;
    vec3 viewDirection = normalize(cameraPosition - position);
    for (int i = 0; i < 4; ++i) {
        vec3 offset = lights[i].position - position;
        float distanceToLight = max(length(offset), 0.0001);
        vec3 lightDirection = offset / distanceToLight;
        float diffuse = max(dot(normal, lightDirection), 0.0);
        vec3 reflected = reflect(-lightDirection, normal);
        float specular = diffuse > 0.0
            ? pow(max(dot(viewDirection, reflected), 0.0), shininess) * materialSpecular : 0.0;
        vec3 falloff = lights[i].attenuation;
        float attenuation = 1.0 / (falloff.x + falloff.y * distanceToLight
                                  + falloff.z * distanceToLight * distanceToLight);
        float cone = mix(1.0, smoothstep(0.70, 0.92,
                         dot(-lightDirection, normalize(lights[i].direction))), lights[i].spot);
        result += (materialColor * diffuse + vec3(specular)) * lights[i].color * attenuation * cone;
    }
    return result;
}

void main() {
    vec4 world = model * vec4(aPosition, 1.0);
    vec3 normal = normalize(transpose(inverse(mat3(model))) * aNormal);
    litColor = shade(world.xyz, normal);
    gl_Position = projection * view * world;
}
