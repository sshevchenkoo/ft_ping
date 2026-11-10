#!/bin/sh
# Dev shell: Debian 11 with the reference inetutils ping, project mounted at /ft_ping.
# Usage: ./dev.sh
set -e

IMAGE=ft_ping-dev

docker build -t "$IMAGE" - <<'EOF'
FROM debian:11
RUN apt-get update && apt-get install -y --no-install-recommends \
	build-essential inetutils-ping valgrind tcpdump \
	&& rm -rf /var/lib/apt/lists/*
WORKDIR /ft_ping
EOF

exec docker run -it --rm -v "$(cd "$(dirname "$0")" && pwd)":/ft_ping "$IMAGE" bash
