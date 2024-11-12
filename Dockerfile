FROM ubuntu:23.04

#openjdk 20 used for the boot image
RUN apt-get update && apt-get install -y \
  build-essential \
  git \
  openjdk-20-jdk \ 
  libfreetype6-dev \
  libcups2-dev \
  libx11-dev libxext-dev libxrender-dev libxrandr-dev libxtst-dev libxt-dev \
  libasound2-dev \
  libffi-dev \
  libfontconfig1-dev\
  zip\
  autoconf \
  file

#Dependencies for GraalVM 
#TODO: remove vim later
RUN apt-get update && apt-get install -y \
  curl \
  zlib1g-dev \
  wget \
  vim 

COPY . /openjdk-21/

# Set working directory
WORKDIR /openjdk-21

# Set JAVA_HOME
ENV JAVA_HOME=/usr/lib/jvm/java-20-openjdk-arm64
ENV PATH=$JAVA_HOME/bin:$PATH

# you may need to run make images multiple times. the compilation server crashes for unknown reasons
RUN bash configure --with-boot-jdk=$JAVA_HOME && \
    make images

# Set the newly built JDK as the default Java
# TODO make the path generic
ENV JAVA_HOME=/openjdk-21/build/linux-x86_64-server-release/images/jdk
ENV PATH=$JAVA_HOME/bin:$PATH

RUN java -version

#TODO add graal
RUN mkdir /workspace
# Setup MX to build graal compiler
ENV MX_PATH=/workspace/mx
RUN git clone https://github.com/graalvm/mx.git ${MX_PATH} && chmod +x ${MX_PATH}/mx 
ENV PATH="/workspace/mx:$PATH"

RUN cd /workspace 
RUN git clone

CMD ["bash"]