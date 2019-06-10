FROM 32bit/ubuntu:16.04
WORKDIR /usr/src/myapp
RUN echo "*** Installing gcc (4.9->8) ***" \
  && apt-get update \
  && apt-get install -y cmake \
  && apt-get install -y g++ \
  && apt-get autoremove --purge -y \
  && apt-get autoclean -y \
  && rm -rf /var/cache/apt/* /tmp/* 
COPY ./ressourceKiller /usr/src/myapp
CMD ["bash"]
