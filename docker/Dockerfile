FROM debian:unstable

MAINTAINER Rainer Poisel <rainer.poisel@gmail.com>

COPY sources.list /etc/apt/sources.list
RUN apt-get update && apt-get install -yy \
    gcc/unstable \
    gcc-multilib/unstable \
    gcc-mingw-w64-i686/unstable \
    gcc-mingw-w64-x86-64/unstable \
    cmake/unstable \
    git/unstable \
    unzip/unstable \
    wget/unstable \
    subversion/unstable

WORKDIR /usr/local/bin
RUN wget 'https://github.com/ninja-build/ninja/releases/download/v1.7.1/ninja-linux.zip' -O /tmp/ninja.zip \
    && unzip -x /tmp/ninja.zip \
    && chmod a+rx ninja \
    && rm /tmp/ninja.zip

WORKDIR /usr/local
RUN svn checkout https://github.com/raspberrypi/tools/trunk/arm-bcm2708/arm-rpi-4.9.3-linux-gnueabihf \
    && rm -rf arm-rpi-4.9.3-linux-gnueabihf/.svn \
    && ln -s arm-rpi-4.9.3-linux-gnueabihf arm-linux-gnueabihf

COPY entrypoint.sh /entrypoint.sh

RUN useradd -ms /bin/bash compile && echo 'compile:start' | chpasswd
USER compile

ENTRYPOINT ["/entrypoint.sh"]
