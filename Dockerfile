# ---------- Этап 1: сборка и юнит-тесты ----------
FROM ubuntu:24.04 AS build
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update \
 && apt-get install -y --no-install-recommends \
        build-essential qtbase5-dev qtbase5-dev-tools libqt5sql5-sqlite \
 && rm -rf /var/lib/apt/lists/*
WORKDIR /src
COPY MathServer       ./MathServer
COPY MathServerTests  ./MathServerTests
COPY MathClient       ./MathClient
COPY MathClientTests  ./MathClientTests
RUN mkdir -p /build/server && cd /build/server \
 && qmake /src/MathServer/MathServer.pro && make -j"$(nproc)"
RUN mkdir -p /build/server-tests && cd /build/server-tests \
 && qmake /src/MathServerTests/MathServerTests.pro && make -j"$(nproc)" && ./MathServerTests \
 && mkdir -p /build/client-tests && cd /build/client-tests \
 && qmake /src/MathClientTests/MathClientTests.pro && make -j"$(nproc)" && QT_QPA_PLATFORM=offscreen ./MathClientTests

# ---------- Этап 2: лёгкий образ только для запуска ----------
FROM ubuntu:24.04
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update \
 && apt-get install -y --no-install-recommends libqt5network5t64 libqt5sql5t64 libqt5sql5-sqlite \
 && rm -rf /var/lib/apt/lists/*
COPY --from=build /build/server/MathServer /usr/local/bin/MathServer
WORKDIR /data
VOLUME ["/data"]
EXPOSE 33333
CMD ["MathServer"]
