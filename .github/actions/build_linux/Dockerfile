FROM ubuntu:16.04

RUN apt-get update
RUN apt-get install -y build-essential cmake curl gcc g++ git make tar unzip zip libgl1-mesa-dev libglu1-mesa-dev jq

ADD entrypoint.sh /entrypoint.sh
RUN chmod a+x /entrypoint.sh

ENTRYPOINT ["/entrypoint.sh"]
