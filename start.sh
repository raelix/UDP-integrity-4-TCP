#!/bin/bash
xterm -e ./Ritardatore.out &
xterm -e netcat -l 127.0.0.1 64000 &
xterm -e ./Proxysender.out 127.0.0.1 &
xterm -e ./Proxyreciver.out 127.0.0.1 &
xterm -e netcat 127.0.0.1 59000 &

