# HOW TO USE THIS DOCKERFILE
#
# 1. build image and tag it as ff7-build:latest
# docker build --tag ff7-build:latest .
#
# 2. launch container and mount current directory under /ff7
# docker run --name ff7-work -it -v $(pwd):/ff7 -v ff7_venv:/ff7/.venv -v ff7_build:/ff7/build ff7-build
#
# 3. you are now ready to build and work on FF7
# make expected
#
# 4. from now on, to re-use the same container execute the following:
# docker start -ai ff7-work

FROM ubuntu:noble

RUN apt-get update && \
    apt-get install -y binutils make git python3 python3-venv ninja-build 7zip bchunk && \
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