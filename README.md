# OS_USER
Ce projet est la réalisation d’un jeu de cartes multijoueur basé sur SH13 (Sherlock 13), développé en C.
Il utilise SDL2 pour l’affichage graphique, et des sockets TCP pour la communication entre les joueurs et le serveur.

# Fichiers inclus

- server.c	: Code source du serveur (logique du jeu)
  
-  sh13.c	: Code source du client SDL (interface graphique)

-  Makefile	: Fichier de compilation standard avec make

-  cmd.sh	: Script Bash qui compile et lance automatiquement le projet

-  SH13_*.png	: Images des cartes et objets (utilisées dans l’interface)

-  Compte Rendu

  # Installation des dépendances (obligatoire)
  Avant de compiler, il faut installer les bibliothèques SDL2 suivantes :
  - sudo apt update
  - sudo apt install libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev

