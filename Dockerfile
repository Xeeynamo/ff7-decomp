# HOW TO USE THIS DOCKERFILE
#
# 1. Build image (one time):
#    docker build --platform=linux/amd64 --tag ff7-build:latest .
#
# 2. Run builds using the wrapper script (recommended):
#    ./tools/docker-build.sh "make build"
#    ./tools/docker-build.sh "make format"
#    ./tools/docker-build.sh bash              # interactive shell
#
# 3. Or run interactively and reuse the container:
#    docker run --name ff7-work -it --platform=linux/amd64 \
#        -v $(pwd):/ff7 -v ff7_venv:/ff7/.venv -v ff7_build:/ff7/build \
#        -v go_cache:/gocache ff7-build
#    # Then reattach with: docker start -ai ff7-work

FROM ubuntu:noble

RUN apt-get update && \
    apt-get install -y build-essential binutils make git python3 python3-venv ninja-build 7zip bchunk && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*
RUN echo "deb [arch=amd64] http://archive.ubuntu.com/ubuntu/ questing main universe multiverse restricted" > /etc/apt/sources.list.d/questing.list && \
    apt-get update && \
    apt-get install -y -t questing binutils-mipsel-linux-gnu gcc-mipsel-linux-gnu && \
    rm /etc/apt/sources.list.d/questing.list && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

COPY --from=golang:1.25-bookworm /usr/local/go/ /usr/local/go/
ENV PATH="${PATH}:/usr/local/go/bin"
ENV GOMODCACHE=/gocache/mod
ENV GOCACHE=/gocache/build

RUN mkdir -p /gocache/mod /gocache/build && chmod -R 777 /gocache

USER ubuntu
WORKDIR /ff7
COPY requirements.txt requirements.txt
RUN chown ubuntu:ubuntu /ff7 && \
    mkdir -p /ff7/.venv /ff7/build && \
    mkdir -p ~/go/bin && \
    ln -s /usr/local/go/bin/go ~/go/bin/go && \
    git config --global --add safe.directory /ff7 && \
    python3 -m venv .venv && \
    . .venv/bin/activate && \
    pip3 install -r requirements.txt

ENTRYPOINT ["/bin/bash"]