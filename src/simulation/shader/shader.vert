#version 450 core

struct Particle {
    uint type;
    vec2 position;
    vec2 velocity;
};
layout(std430, binding = 0) buffer particlesBuff {
    Particle particles[];
};

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
  gl_Position = vec4(self.position, 1.0, 1.0);
}
