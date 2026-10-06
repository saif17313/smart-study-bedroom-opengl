#version 330 core
in vec3 worldPosition;
in vec3 worldNormal;
out vec4 fragmentColor;

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
    vec3 color = shade(worldPosition, normalize(worldNormal));
    fragmentColor = vec4(pow(max(color, vec3(0.0)), vec3(1.0 / 2.2)), 1.0);
}
