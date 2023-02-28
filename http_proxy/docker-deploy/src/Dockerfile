FROM ubuntu:20.04
RUN apt-get update && apt-get install -y g++ libboost-dev make

RUN mkdir /var/log/erss
RUN touch /var/log/erss/proxy.log
# Copy the source code to the image
RUN mkdir app
COPY . /app/
WORKDIR /app

# Build the program using Makefile
RUN make clean
RUN make

# Set the command to run the program
CMD ["./Proxy 4321"]