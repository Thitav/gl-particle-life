#version 450 core

struct Particle {
    vec2 position;
    vec2 velocity;
    uint type;
};
layout (std430, binding = 0) buffer particlesSSBO {
    Particle particles[];
};

struct ParticleGroupRule {
    float attraction;
    float min_radius;
    float max_radius;
};
layout (std140, binding = 1) uniform simulationUBO {
    ParticleGroupRule groupsRules[32 * 32];
    vec4 groupsColors[32];
    uint particlesCount;
    uint groupsCount;
    float viscosity;
};

out vec4 particleColor;
out gl_PerVertex {
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
    gl_PointSize = 5.0f;
    gl_Position = vec4(self.position, 1.0, 1.0);
}
