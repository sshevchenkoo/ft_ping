#!/bin/sh
# Dev shell: Debian 11 with the reference inetutils ping, project mounted at /ft_ping.
# Usage: ./dev.sh
set -e

IMAGE=ft_ping-dev

docker build -t "$IMAGE" - <<'EOF'
FROM debian:11
# Debian 11 is EOL: its security packages now live only on archive.debian.org
RUN sed -i 's|http://deb.debian.org/debian-security|http://archive.debian.org/debian-security|' \
	/etc/apt/sources.list \
	&& apt-get update && apt-get install -y --no-install-recommends \
	build-essential inetutils-ping valgrind tcpdump \
	&& rm -rf /var/lib/apt/lists/*
WORKDIR /ft_ping
EOF

exec docker run -it --rm -v "$(cd "$(dirname "$0")" && pwd)":/ft_ping "$IMAGE" bash
