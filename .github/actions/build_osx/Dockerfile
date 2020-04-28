
FROM debian

RUN apt-get update && \
    apt-get upgrade -yy && \
    apt-get install -yy \
    automake            \
    bison               \
    curl                \
    file                \
    flex                \
    git                 \
    libtool             \
    pkg-config          \
    python              \
    texinfo             \
    vim                 \
    wget                \
    zlib1g-dev          \
    build-essential     \
    cmake               \
    make                \
    tar                 \
    unzip               \
    zip                 \
    libgl1-mesa-dev     \
    libglu1-mesa-dev    \
    jq			\
    rsync

ENV OSXCROSS_SDK_VERSION 10.11
RUN SDK_VERSION=$OSXCROSS_SDK_VERSION                           \
    mkdir /opt/osxcross &&                                      \
    cd /opt &&                                                  \
    git clone https://github.com/tpoechtrager/osxcross.git &&   \
    cd osxcross &&                                              \
    git checkout e0a171828a72a0d7ad4409489033536590008ebf &&    \
    sed -i -e 's|-march=native||g' ./build_clang.sh ./wrapper/build.sh && \
    ./tools/get_dependencies.sh &&                              \
    curl -L -o ./tarballs/MacOSX${OSXCROSS_SDK_VERSION}.sdk.tar.xz \
    https://github.com/apriorit/osxcross-sdks/raw/master/MacOSX${OSXCROSS_SDK_VERSION}.sdk.tar.xz && \
    yes | PORTABLE=true ./build.sh &&                           \
    ./build_compiler_rt.sh

ENV PATH $PATH:/opt/osxcross/target/bin

ADD entrypoint.sh /entrypoint.sh
RUN chmod a+x /entrypoint.sh

ENTRYPOINT ["/entrypoint.sh"]
