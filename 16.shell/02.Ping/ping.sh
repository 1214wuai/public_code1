#/bin/bash

i=1
while [ $i -le 254 ]
do
  ping -c1 192.168.0.$i &
  let i++
done

arp -a | grep -v "incomplete"
