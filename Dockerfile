FROM ubuntu:jammy

RUN apt-get update && \
    apt-get install -y wget tar python3 python3-pip openjdk-8-jre-headless

RUN rm -rf /var/lib/apt/lists/* \
    apt-get clean

ENV JAVA_HOME=/usr/lib/jvm/java-8-openjdk-amd64

ENV SPARK_VERSION=3.5.5
ENV SPARK_RELEASE=spark-${SPARK_VERSION}
ENV SPARK_PACKAGE=${SPARK_RELEASE}-bin-hadoop3

WORKDIR /app

RUN wget https://dlcdn.apache.org/spark/${SPARK_RELEASE}/${SPARK_PACKAGE}.tgz
RUN tar -xzf ${SPARK_PACKAGE}.tgz

ENV SPARK_HOME=/app/${SPARK_PACKAGE}

COPY ./main.py ./requirements.txt /app/

RUN pip3 install -r /app/requirements.txt

EXPOSE 8000

ENTRYPOINT [ "fastapi" ]
CMD [ "run", "/app/main.py" ]
