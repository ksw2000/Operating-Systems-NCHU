FROM ubuntu
MAINTAINER liao2000
VOLUME ["/Lab", "/HW"]

RUN cd ~
RUN bash
RUN apt-get update
RUN apt-get upgrade -y
RUN apt-get install build-essential -y
RUN apt-get install nano -y
RUN apt-get install default-jre -y
RUN apt-get install default-jdk -y

CMD ["bash"]
