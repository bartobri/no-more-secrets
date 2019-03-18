FROM ubuntu as buildenv

WORKDIR /

RUN apt-get update && apt-get install build-essential git -y
RUN git clone https://github.com/bartobri/no-more-secrets.git

WORKDIR /no-more-secrets
RUN make nms
RUN make sneakers
RUN make install

FROM ubuntu
COPY --from=buildenv /usr/local/bin/nms /usr/local/bin/
COPY --from=buildenv /usr/local/bin/sneakers /usr/local/bin/

ENTRYPOINT [ "nms" ]
