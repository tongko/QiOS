FROM debian
LABEL maintainer "tongko <github.com/tongko>"

ARG BINUTILS_VER=2.35.1
ARG GCC_VER=10.2.0

#Prerequisites
RUN set -x \
	&& apt-get update && apt-get upgrade -y \
	&& apt-get install -y \
	wget gcc libgmp3-dev libmpfr-dev libisl-dev libmpc-dev \
	texinfo bison flex make bzip2 patch build-essential nasm xorriso \
	grub-pc-bin grub-common

#Pull binutils and gcc source code
RUN set -x \
	&& mkdir -p /usr/local/src \
	&& cd /usr/local/src \
	&& wget -q ftp://ftp.gnu.org/gnu/binutils/binutils-${BINUTILS_VER}.tar.xz \
	&& wget -q ftp://ftp.gnu.org/gnu/gcc/gcc-${GCC_VER}/gcc-${GCC_VER}.tar.xz \
	&& tar -xvf binutils-${BINUTILS_VER}.tar.xz \
	&& tar -xvf gcc-${GCC_VER}.tar.xz \
	&& rm binutils-${BINUTILS_VER}.tar.xz gcc-${GCC_VER}.tar.xz \
	&& chown -R root:root binutils-${BINUTILS_VER} \
	&& chmod -R o-w,g+w binutils-${BINUTILS_VER} \
	&& chown -R root:root gcc-${GCC_VER} \
	&& chmod -R o-w,g+w gcc-${GCC_VER}

#Copy build script and patch files
COPY files/*.sh /usr/local/src/
COPY files/gcc/t-x86_64-elf /usr/local/src/gcc-${GCC_VER}/gcc/config/i386/
COPY files/gcc/config.gcc.patch /usr/local/src/gcc-${GCC_VER}/gcc/

RUN set -x \
	&& chown -R root:root /usr/local/src/*.sh \
	&& chmod -R +x /usr/local/src/*.sh

#Build and install
RUN set -x \
	&& cd /usr/local/src \
	&& /usr/local/src/build-binutils.sh ${BINUTILS_VER} \
	&& /usr/local/src/build-gcc.sh ${GCC_VER}
