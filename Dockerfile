FROM alpine:3.4

WORKDIR /app
RUN apk add --no-cache gcc ncurses git make sudo ncurses-dev musl-dev; \
    git clone https://github.com/bartobri/no-more-secrets.git; \
    cd /app/no-more-secrets; \
    make && make install && make clean; \
    apk del gcc git make sudo ncurses-dev musl-dev

CMD nms


