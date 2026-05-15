FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential \
    g++ \
    make \
    gdb \
    valgrind \
    vim \
    git \
    strace \
    curl \
    netcat-openbsd \
    net-tools \
    python3 \
    php-cgi \
    bash-completion \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /webserv

CMD ["/bin/bash"]