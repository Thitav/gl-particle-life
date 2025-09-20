FROM debian:13.1

RUN set -ex; \ 
  apt-get update; \
  apt-get install -qqy --no-install-recommends \
  wget ca-certificates curl git \
  gcc g++ clang cmake ninja-build \
  mesa-utils libglu1-mesa-dev freeglut3-dev mesa-common-dev \
  libglew-dev libglfw3-dev libglm-dev libx11-dev libxrandr-dev \
  libxinerama-dev libxcursor-dev libxi-dev;

CMD ["/bin/bash", "-i"]
