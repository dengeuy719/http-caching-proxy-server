FROM ubuntu:20.04
RUN apt-get update && apt-get install -y g++ libboost-dev make

RUN mkdir /var/log/erss
RUN touch /var/log/erss/proxy.log
# Copy the source code to the image
RUN mkdir app
COPY . /app/
WORKDIR /app
RUN chmod o+x run.sh
