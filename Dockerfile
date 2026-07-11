FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    pkg-config \
    libgtk-3-dev \
    libsoup2.4-dev \
    libglib2.0-dev \
    libxml2-dev \
    zlib1g-dev \
    gettext \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /src
COPY . .

RUN rm -rf build CMakeFiles CMakeCache.txt && \
    mkdir -p build && cd build && cmake .. && make -j$(nproc)

CMD ["/bin/sh", "-c", "cp -r /src/build/bin/. /out"]