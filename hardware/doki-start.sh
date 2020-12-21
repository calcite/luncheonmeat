#!/bin/bash
# 
# DoKi, run KiCad using docker.
# Copyright (C) 2020 Juraj Giertl <j74a8g+doki@gmail.com>
# 
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#  

set -o pipefail
set -E

version="0.1.0"
scriptName=$(basename "$0")
bashVersion=$(echo "$BASH_VERSION" | cut -d. -f1)

if [ -z "$BASH_VERSION" ] || [ "$bashVersion" -lt 4 ]; then
  echo >&2 "You need bash v4+ to run this script. Aborting..."
  exit 1
fi


WORKDIR=${PWD}
PROJECT=${PWD##*/}

mkdir -p ${WORKDIR}/.config

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

function info() {
    
    lspci -v -s $(lspci | grep VGA | cut -d" " -f 1)

}

function main() {
    echo "starting doki"
    docker run -ti \
        --rm \
        --name doki \
        --hostname DoKi-v0.1.0 \
        -e DISPLAY=$DISPLAY \
        -v /tmp/.X11-unix:/tmp/.X11-unix \
        --mount type=bind,source=${WORKDIR},target=/workspace/${PROJECT} \
        --mount type=bind,source=${WORKDIR}/.config,target=/home/kicad/.config \
        --device=/dev/dri:/dev/dri \
        --cap-add=SYS_PTRACE --security-opt seccomp=unconfined \
        doki
}

main "$@"