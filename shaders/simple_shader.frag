#version 450

layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec3 fragPosWorld;
layout (location = 2) in vec3 fragNormalWorld;
layout (location = 3) in vec2 fragUV;

layout (location = 0) out vec4 outColor;

struct PointLight {
  vec4 position; // ignore w
  vec4 color; // w = intensity
};

layout (set = 0, binding = 0) uniform GlobalUbo {
  mat4 projection;
  mat4 view;
  mat4 inverseView;
  vec4 ambientLightColor;
  PointLight pointLights[10];
  int numLights;
} ubo;

layout (set = 0, binding = 1) uniform sampler2D image;

layout(push_constant) uniform Push {
  mat4 modelMatrix;
  mat4 normalMatrix;
} push;

void main() {
  vec3 ambientLight = ubo.ambientLightColor.xyz * ubo.ambientLightColor.w;
  vec3 diffuseLight = ambientLight;

  vec3 specularLight = vec3(0.0);

  vec3 surfaceNormal = normalize(fragNormalWorld);

  vec3 cameraPosWorld = ubo.inverseView[3].xyz;
  vec3 viewDirection = normalize(cameraPosWorld - fragPosWorld);



  for(int i = 0; i < ubo.numLights; i++) {
    PointLight light = ubo.pointLights[i];
    vec3 directionToLight = light.position.xyz - fragPosWorld;
    float attenuation = 1.0 / dot(directionToLight, directionToLight);
    directionToLight = normalize(directionToLight);

    float cosAngleIncidence = max(dot(normalize(fragNormalWorld), directionToLight), 0);
    vec3 intensity = light.color.xyz * light.color.w * attenuation;

    diffuseLight += intensity * cosAngleIncidence;

    vec3 halfAngle = normalize(directionToLight + viewDirection);
    float blinnTerm = dot(surfaceNormal, halfAngle);
    blinnTerm = clamp(blinnTerm, 0, 1);
    blinnTerm = pow(blinnTerm, 202.0);
    specularLight += intensity * blinnTerm;
  }
  vec3 imageColor = texture(image, fragUV * vec2(1.0, -1.0)).xyz;
  outColor = vec4((diffuseLight * fragColor + specularLight * fragColor) * imageColor, 1.0);
}