#version 330 core
in vec3 worldPosition;
in vec3 worldNormal;
out vec4 fragmentColor;

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
    vec3 color = shade(worldPosition, normalize(worldNormal));
    fragmentColor = vec4(pow(max(color, vec3(0.0)), vec3(1.0 / 2.2)), 1.0);
}
