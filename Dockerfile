FROM ubuntu:22.04

# Install necessary packages
RUN apt-get update && apt-get install -y \
    git \
    cmake \
    g++ \
    make \
    ccache \
    libncurses5-dev \
    && rm -rf /var/lib/apt/lists/*

COPY . /avida
WORKDIR "/avida"

RUN git submodule update --init --recursive

# Build Avida
RUN ./build_avida

# Set Avida executable as entrypoint
ENTRYPOINT ["/avida/cbuild/work/avida"]
