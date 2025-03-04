#version 450 core

struct Particle {
  vec2 position;
  vec2 velocity;
  uint type;
};
layout (std430, binding = 0) buffer particlesSSBO {
  Particle particles[];
};

layout (std140, binding = 1) uniform simulationUBO {
  uint particlesCount;
  uint groupsCount;
  vec4 groupsColors[8];
  vec4 groupsRules[64];
};

out vec4 particleColor;
out gl_PerVertex
{
  vec4 gl_Position;
  float gl_PointSize;
  float gl_ClipDistance[];
};

void main() {
  Particle self = particles[gl_InstanceID];

  if (self.position.x >= 1 || self.position.x <= -1) {
    self.velocity *= -1.0;
  }
  if (self.position.y >= 1 || self.position.y <= -1) {
    self.velocity *= -1.0;
  }
  self.position += self.velocity;

  particles[gl_InstanceID] = self;
  particleColor = groupsColors[self.type];
  gl_Position = vec4(self.position, 1.0, 1.0);
}
