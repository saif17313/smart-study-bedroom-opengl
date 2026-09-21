#version 330 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
out vec3 litColor;

uniform vec3 materialColor;
uniform float materialSpecular;
uniform float shininess;
uniform vec3 cameraPosition;
uniform vec3 ambientColor;
uniform vec3 lightPosition;
uniform vec3 lightColor;

// One fixed light: ambient + diffuse + specular, with no spotlight or toggles.
vec3 shade(vec3 position, vec3 normal) {
    vec3 lightDirection = normalize(lightPosition - position);
    float diffuse = max(dot(normal, lightDirection), 0.0);
    vec3 viewDirection = normalize(cameraPosition - position);
    vec3 reflected = reflect(-lightDirection, normal);
    float specular = diffuse > 0.0
        ? pow(max(dot(viewDirection, reflected), 0.0), shininess) * materialSpecular
        : 0.0;
    return materialColor * ambientColor
        + (materialColor * diffuse + vec3(specular)) * lightColor;
}

void main() {
    vec4 world = model * vec4(aPosition, 1.0);
    vec3 normal = normalize(transpose(inverse(mat3(model))) * aNormal);
    litColor = shade(world.xyz, normal);
    gl_Position = projection * view * world;
}
