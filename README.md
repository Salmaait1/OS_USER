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

# Compilation du projet
Méthode 1 – Avec make:

                make
          
Nettoyer la compilation :
      make clean
# Méthode 2 – Avec le script cmd.sh (tout en un):
Le script cmd.sh :

- Compile le serveur et le client

- Lance le serveur (./server 12345)

- Lance automatiquement 4 clients (Alice, Bob, Carol, Dave)
Pour l'utiliser :
      chmod +x cmd.sh   # (à faire une seule fois)
      ./cmd.sh
# Lancer manuellement
Étape 1 : Lancer le serveur
      ./server 12345
Étape 2 : Lancer les clients (chacun dans un terminal différent)
      ./sh13 localhost 12345 localhost 5000 Alice
      ./sh13 localhost 12345 localhost 5001 Bob
      ./sh13 localhost 12345 localhost 5002 Carol
      ./sh13 localhost 12345 localhost 5003 Dave

# À propos du jeu

Le serveur distribue 3 cartes à chaque joueur.

Une 13e carte est le coupable.

À son tour, un joueur peut interroger ou accuser via l'interface.

Une accusation correcte fait gagner la partie.



