#!/bin/bash
# Script de compilation et de lancement de l'application SH13 via le Makefile

echo "Lancement de la compilation avec make..."
make clean
make
if [ $? -ne 0 ]; then
    echo "Erreur lors de la compilation avec make"
    exit 1
fi

echo "Compilation réussie."
echo "Lancement du serveur sur le port 12345..."
./server 12345 &
server_pid=$!
echo "Serveur lancé (PID $server_pid)"
sleep 1  

echo "Lancement des clients..."
# Chaque client doit disposer d'un port client différent.
./sh13 localhost 12345 localhost 5000 Alice &
./sh13 localhost 12345 localhost 5001 Bob &
./sh13 localhost 12345 localhost 5002 Carol &
./sh13 localhost 12345 localhost 5003 Dave &

wait

