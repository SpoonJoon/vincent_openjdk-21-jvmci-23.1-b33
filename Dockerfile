FROM ubuntu:24.04

#openjdk 20 used for the boot image -> use 21 b/c lts. now we build labsjdk21 with openjdk21 lol
RUN apt-get update && apt-get install -y \
  build-essential \
  git \
  openjdk-21-jdk \ 
  libfreetype6-dev \
  libcups2-dev \
  libx11-dev libxext-dev libxrender-dev libxrandr-dev libxtst-dev libxt-dev \
  libasound2-dev \
  libffi-dev \
  libfontconfig1-dev\
  zip\
  autoconf \
  file

COPY . /openjdk-21/

# Set working directory
WORKDIR /openjdk-21

# Set JAVA_HOME (arm64 for mac)
ENV JAVA_HOME=/usr/lib/jvm/java-21-openjdk-arm64 
ENV PATH=$JAVA_HOME/bin:$PATH

# you may need to run make images multiple times. the compilation server crashes for unknown reasons
# RUN bash configure --with-boot-jdk=$JAVA_HOME && \
#     make images

# Set the newly built JDK as the default Java
# ENV JAVA_HOME=/openjdk-21/build/*/images/jdk

# Modify PATH in a single RUN command instead
# RUN NEW_PATH=$(echo $PATH | tr ':' '\n' | grep -v '/usr/bin' | tr '\n' ':') && \
#     echo "PATH=$JAVA_HOME/bin:$NEW_PATH" >> /etc/environment

# RUN java -version

#TODO add graal

CMD ["bash"]