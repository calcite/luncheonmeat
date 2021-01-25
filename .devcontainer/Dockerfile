FROM debian:10

LABEL Name="avr-gcc-dev-container" \
    Maintainer="j74a8g+avr-gcc-dev@gmail.com" \
    Author="Juraj Giertl" \
    Date="2021.01.25" \
    Version="0.2" \
    Description="Visual Studio Dev container for AVR"

# Avoid warnings by switching to noninteractive
ENV DEBIAN_FRONTEND=noninteractive

# Or your actual UID, GID on Linux if not the default 1000
ARG USERNAME=vscode
ARG USER_UID=1000
ARG USER_GID=$USER_UID

# Configure apt and install packages
RUN apt-get update \
    && apt-get -y install --no-install-recommends apt-utils dialog 2>&1 \
    # 
    # Verify git, process tools, lsb-release (useful for CLI installs) installed
    && apt-get -y install git procps lsb-release curl bash-completion \
    #
    # Install C++ tools
    && apt-get -y install build-essential cppcheck valgrind clang \
    #
    # Install needed packages.
    # && apt-get -y install lldb llvm \
    && apt-get -y install --no-install-recommends gcc-avr binutils-avr avr-libc \
                gdb-avr avrdude \
    # 
    # Create a non-root user to use if preferred - see https://aka.ms/vscode-remote/containers/non-root-user.
    && groupadd --gid $USER_GID $USERNAME \
    && useradd -s /bin/bash --uid $USER_UID --gid $USER_GID -m $USERNAME \
    # [Optional] Add sudo support for non-root user
    && apt-get install -y sudo \
    && echo $USERNAME ALL=\(root\) NOPASSWD:ALL > /etc/sudoers.d/$USERNAME \
    && chmod 0440 /etc/sudoers.d/$USERNAME \
    #
    # Clean up
    && apt-get autoremove -y \
    && apt-get clean -y \
    && rm -rf /var/lib/apt/lists/*

RUN usermod -a -G dialout $USERNAME

# Switch back to dialog for any ad-hoc use of apt-get
ENV DEBIAN_FRONTEND=

ADD avr5/* /usr/lib/avr/lib/avr5/
ADD iom328pb.h /usr/lib/avr/include/avr/
ADD *.patch /tmp

RUN patch /usr/lib/avr/include/avr/io.h < /tmp/io.patch

USER $USERNAME
WORKDIR /workspaces

ENV LANGUAGE C.UTF-8
ENV LC_ALL C.UTF-8
ENV LANG C.UTF-8
