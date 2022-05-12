##
# crazyzlj/ccgl:alpine
#
# This script is written based on PROJ and GDAL. 
#   https://github.com/OSGeo/PROJ/blob/master/Dockerfile
#   https://github.com/OSGeo/gdal/tree/master/docker
#
# Usage: 
#   > cd CCGL
#   > docker build -t <tag> -f docker/apline/Dockerfile
#
# Copyright 2022 Liang-Jun Zhu <zlj@lreis.ac.cn>

# Use alpine as the build and test container
ARG ALPINE_VERSION=3.15
FROM alpine:${ALPINE_VERSION} as builder

LABEL maintainer="Liang-Jun Zhu <zlj@lreis.ac.cn>"

# Replace alpine repository source cdn
RUN sed -i 's/dl-cdn.alpinelinux.org/mirrors.tuna.tsinghua.edu.cn/g' /etc/apk/repositories

# Setup build env
RUN apk add --no-cache git cmake make g++

# Copy source directory
WORKDIR /CCGL
COPY CMakeLists.txt .
COPY cmake cmake
COPY src src
COPY apps apps
COPY test test
COPY data data


# Build with GoogleTest and run unittest
RUN cd /CCGL \
    && mkdir build_with_test \
    && cmake -B build_with_test -DCMAKE_BUILD_TYPE=Debug -DUNITTEST=1 -DFORCEDOWNGTEST=1 \
    && cmake --build build_with_test --config Debug -- -j 2 \
    && cd build_with_test \
    && ctest -C Debug --rerun-failed --output-on-failure \
    && cd ..

# # Build for release
ARG INSTALL_DIR=/CCGL/dist
RUN mkdir build \
    && cd build \
    && cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} \
    && make -j 2 \
    && make install \
    && cd ..

# # Build final image
FROM alpine:${ALPINE_VERSION} as runner

RUN date

# Add gnu standard C++ library and OpenMP library for running apps based on CCGL
RUN apk add --no-cache libstdc++ libgomp

# Order layers starting with less frequently varying ones
ARG INSTALL_DIR=/CCGL/dist
COPY --from=builder ${INSTALL_DIR}/bin/ /usr/bin/
COPY --from=builder ${INSTALL_DIR}/include/ /usr/include/
COPY --from=builder ${INSTALL_DIR}/lib/ /usr/lib/
