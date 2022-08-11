FROM ubuntu
WORKDIR /root/ 
RUN apt-get update && apt-get install -y redis-tools
COPY game /root/
COPY game_start.sh /root/
COPY random_first.txt /root/
COPY random_last.txt /root/
EXPOSE 51111 
ENTRYPOINT ["./game_start.sh"]
