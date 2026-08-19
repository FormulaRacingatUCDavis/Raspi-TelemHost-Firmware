FROM ubuntu:24.04 AS builder

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        build-essential \
        cmake \
        git \
        pkg-config \
        libmosquitto-dev \
        ca-certificates \
        curl \
        zip \
        unzip \
        tar \
        && rm -rf /var/lib/apt/lists/*

WORKDIR /app

RUN git clone https://github.com/microsoft/vcpkg.git /opt/vcpkg && \
    /opt/vcpkg/bootstrap-vcpkg.sh -disableMetrics

ENV VCPKG_ROOT=/opt/vcpkg

COPY . .

RUN cmake -S . -B build \
    -DCMAKE_TOOLCHAIN_FILE=/opt/vcpkg/scripts/buildsystems/vcpkg.cmake \
    -DCMAKE_BUILD_TYPE=Release

RUN cmake --build build -j$(nproc)


FROM ubuntu:24.04

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        libmosquitto1 \
        && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY --from=builder /app/build/DAQ ./DAQ

CMD ["./DAQ"]