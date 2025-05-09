#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>       
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

pthread_t thread_serveur_tcp_id;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
char gbuffer[256];
char gServerIpAddress[256];
int gServerPort;
char gClientIpAddress[256];
int gClientPort;
char gName[256];
char gNames[4][256];
int gId;
int joueurSel;
int objetSel;
int guiltSel;
int guiltGuess[13];
int tableCartes[4][8];
int b[3];
int goEnabled;
int connectEnabled;

char *nbobjets[] = {"5", "5", "5", "5", "4", "3", "3", "3"};
char *nbnoms[] = {"Sebastian Moran", "irene Adler", "inspector Lestrade",
  "inspector Gregson", "inspector Baynes", "inspector Bradstreet",
  "inspector Hopkins", "Sherlock Holmes", "John Watson", "Mycroft Holmes",
  "Mrs. Hudson", "Mary Morstan", "James Moriarty"};

volatile int synchro;

void *fn_serveur_tcp(void *arg)
{
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    int n;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        printf("sockfd error\n");
        exit(1);
    }

    bzero((char *)&serv_addr, sizeof(serv_addr));
    portno = gClientPort;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("bind error\n");
        exit(1);
    }

    listen(sockfd, 5);
    clilen = sizeof(cli_addr);
    while (1)
    {
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
        if (newsockfd < 0)
        {
            printf("accept error\n");
            exit(1);
        }

        bzero(gbuffer, 256);
        n = read(newsockfd, gbuffer, 255);
        if (n < 0)
        {
            printf("read error\n");
            exit(1);
        }

        // Afficher le message reçu (pour debug)
        printf("Reçu du serveur: |%s|\n", gbuffer);

        synchro = 1;
        while (synchro)
            ;  // attente active simple (à améliorer)
    }
    return NULL;
}

void sendMessageToServer(char *ipAddress, int portno, char *mess)
{
    int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char sendbuffer[256];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    server = gethostbyname(ipAddress);
    if (server == NULL)
    {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("ERROR connecting\n");
        exit(1);
    }

    sprintf(sendbuffer, "%s\n", mess);
    n = write(sockfd, sendbuffer, strlen(sendbuffer));
    close(sockfd);
}

int main(int argc, char **argv)
{
    int ret;
    int i, j;

    int quit = 0;
    SDL_Event event;
    int mx, my;
    char sendBuffer[256];
    int id;

    if (argc < 6)
    {
        printf("<app> <Main server ip address> <Main server port> <Client ip address> <Client port> <player name>\n");
        exit(1);
    }

    strcpy(gServerIpAddress, argv[1]);
    gServerPort = atoi(argv[2]);
    strcpy(gClientIpAddress, argv[3]);
    gClientPort = atoi(argv[4]);
    strcpy(gName, argv[5]);

    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    SDL_Window *window = SDL_CreateWindow("SDL2 SH13",
                                            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1024, 768, 0);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);

    SDL_Surface *deck[13], *objet[8], *gobutton, *connectbutton;
    deck[0] = IMG_Load("SH13_0.png");
    deck[1] = IMG_Load("SH13_1.png");
    deck[2] = IMG_Load("SH13_2.png");
    deck[3] = IMG_Load("SH13_3.png");
    deck[4] = IMG_Load("SH13_4.png");
    deck[5] = IMG_Load("SH13_5.png");
    deck[6] = IMG_Load("SH13_6.png");
    deck[7] = IMG_Load("SH13_7.png");
    deck[8] = IMG_Load("SH13_8.png");
    deck[9] = IMG_Load("SH13_9.png");
    deck[10] = IMG_Load("SH13_10.png");
    deck[11] = IMG_Load("SH13_11.png");
    deck[12] = IMG_Load("SH13_12.png");

    objet[0] = IMG_Load("SH13_pipe_120x120.png");
    objet[1] = IMG_Load("SH13_ampoule_120x120.png");
    objet[2] = IMG_Load("SH13_poing_120x120.png");
    objet[3] = IMG_Load("SH13_couronne_120x120.png");
    objet[4] = IMG_Load("SH13_carnet_120x120.png");
    objet[5] = IMG_Load("SH13_collier_120x120.png");
    objet[6] = IMG_Load("SH13_oeil_120x120.png");
    objet[7] = IMG_Load("SH13_crane_120x120.png");

    gobutton = IMG_Load("gobutton.png");
    connectbutton = IMG_Load("connectbutton.png");

    strcpy(gNames[0], "-");
    strcpy(gNames[1], "-");
    strcpy(gNames[2], "-");
    strcpy(gNames[3], "-");

    joueurSel = -1;
    objetSel = -1;
    guiltSel = -1;

    b[0] = -1;
    b[1] = -1;
    b[2] = -1;

    for (i = 0; i < 13; i++)
        guiltGuess[i] = 0;

    for (i = 0; i < 4; i++)
        for (j = 0; j < 8; j++)
            tableCartes[i][j] = -1;

    goEnabled = 0;
    connectEnabled = 1;

    SDL_Texture *texture_deck[13], *texture_gobutton, *texture_connectbutton, *texture_objet[8];

    for (i = 0; i < 13; i++)
        texture_deck[i] = SDL_CreateTextureFromSurface(renderer, deck[i]);
    for (i = 0; i < 8; i++)
        texture_objet[i] = SDL_CreateTextureFromSurface(renderer, objet[i]);

    texture_gobutton = SDL_CreateTextureFromSurface(renderer, gobutton);
    texture_connectbutton = SDL_CreateTextureFromSurface(renderer, connectbutton);

    TTF_Font *Sans = TTF_OpenFont("sans.ttf", 15);
    printf("Sans=%p\n", Sans);

    /* Création du thread serveur TCP. */
    printf("Création du thread serveur tcp !\n");
    synchro = 0;
    ret = pthread_create(&thread_serveur_tcp_id, NULL, fn_serveur_tcp, NULL);

    while (!quit)
    {
        if (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                quit = 1;
                break;
            case SDL_MOUSEBUTTONDOWN:
                SDL_GetMouseState(&mx, &my);
                if ((mx < 200) && (my < 50) && (connectEnabled == 1))
                {
                    sprintf(sendBuffer, "C %s %d %s", gClientIpAddress, gClientPort, gName);
                    // Envoi du message de connexion au serveur
                    sendMessageToServer(gServerIpAddress, gServerPort, sendBuffer);
                    connectEnabled = 0;
                }
                else if ((mx >= 0) && (mx < 200) && (my >= 90) && (my < 330))
                {
                    joueurSel = (my - 90) / 60;
                    guiltSel = -1;
                }
                else if ((mx >= 200) && (mx < 680) && (my >= 0) && (my < 90))
                {
                    objetSel = (mx - 200) / 60;
                    guiltSel = -1;
                }
                else if ((mx >= 100) && (mx < 250) && (my >= 350) && (my < 740))
                {
                    joueurSel = -1;
                    objetSel = -1;
                    guiltSel = (my - 350) / 30;
                }
                else if ((mx >= 250) && (mx < 300) && (my >= 350) && (my < 740))
                {
                    int ind = (my - 350) / 30;
                    guiltGuess[ind] = 1 - guiltGuess[ind];
                }
                else if ((mx >= 500) && (mx < 700) && (my >= 350) && (my < 450) && (goEnabled == 1))
                {
                    printf("go! joueur=%d objet=%d guilt=%d\n", joueurSel, objetSel, guiltSel);
                    if (guiltSel != -1)
                    {
                        sprintf(sendBuffer, "G %d %d", gId, guiltSel);
                        // Envoi de la commande d'accusation
                        sendMessageToServer(gServerIpAddress, gServerPort, sendBuffer);
                    }
                    else if ((objetSel != -1) && (joueurSel == -1))
                    {
                        sprintf(sendBuffer, "O %d %d", gId, objetSel);
                        // Envoi de la commande enquête type "r"
                        sendMessageToServer(gServerIpAddress, gServerPort, sendBuffer);
                    }
                    else if ((objetSel != -1) && (joueurSel != -1))
                    {
                        sprintf(sendBuffer, "S %d %d %d", gId, joueurSel, objetSel);
                        // Envoi de la commande enquête type "z"
                        sendMessageToServer(gServerIpAddress, gServerPort, sendBuffer);
                    }
                }
                else
                {
                    joueurSel = -1;
                    objetSel = -1;
                    guiltSel = -1;
                }
                break;
            case SDL_MOUSEMOTION:
                SDL_GetMouseState(&mx, &my);
                break;
            }
        }

        if (synchro == 1)
        {
            printf("consomme |%s|\n", gbuffer);
            switch (gbuffer[0])
            {
                // Message 'I' : le joueur reçoit son id
            case 'I':
                sscanf(gbuffer, "I %d", &gId);
                break;
                // Message 'L' : le joueur reçoit la liste des joueurs
            case 'L':
                sscanf(gbuffer, "L %s %s %s %s", gNames[0], gNames[1], gNames[2], gNames[3]);
                break;
                // Message 'D' : le joueur reçoit ses 3 cartes et sa ligne de tableCartes
            case 'D':
                // Format attendu: "D card1 card2 card3 t0 t1 ... t7"
                sscanf(gbuffer, "D %d %d %d %d %d %d %d %d %d %d %d",
                       &b[0], &b[1], &b[2],
                       &tableCartes[gId][0], &tableCartes[gId][1], &tableCartes[gId][2],
                       &tableCartes[gId][3], &tableCartes[gId][4], &tableCartes[gId][5],
                       &tableCartes[gId][6], &tableCartes[gId][7]);
                break;
                // Message 'M' : le joueur reçoit le n° du joueur courant
            case 'M':
            {
                int current;
                sscanf(gbuffer, "M %d", &current);
                if (current == gId)
                    goEnabled = 1;
                else
                    goEnabled = 0;
            }
            break;
                // Message 'V' : le joueur reçoit une valeur de tableCartes pour un symbole donné
            case 'V':
            {
                int obj, newval;
                sscanf(gbuffer, "V %d %d", &obj, &newval);
                tableCartes[gId][obj] = newval;
            }
            break;
            }
            synchro = 0;
        }

        // (Ensuite, le code d'affichage graphique reste inchangé.)
        SDL_Rect dstrect_grille = {512 - 250, 10, 500, 350};
        SDL_Rect dstrect_image = {0, 0, 500, 330};
        SDL_Rect dstrect_image1 = {0, 340, 250, 330 / 2};

        SDL_SetRenderDrawColor(renderer, 255, 230, 230, 230);
        SDL_Rect rect = {0, 0, 1024, 768};
        SDL_RenderFillRect(renderer, &rect);

        if (joueurSel != -1)
        {
            SDL_SetRenderDrawColor(renderer, 255, 180, 180, 255);
            SDL_Rect rect1 = {0, 90 + joueurSel * 60, 200, 60};
            SDL_RenderFillRect(renderer, &rect1);
        }

        if (objetSel != -1)
        {
            SDL_SetRenderDrawColor(renderer, 180, 255, 180, 255);
            SDL_Rect rect1 = {200 + objetSel * 60, 0, 60, 90};
            SDL_RenderFillRect(renderer, &rect1);
        }

        if (guiltSel != -1)
        {
            SDL_SetRenderDrawColor(renderer, 180, 180, 255, 255);
            SDL_Rect rect1 = {100, 350 + guiltSel * 30, 150, 30};
            SDL_RenderFillRect(renderer, &rect1);
        }

        {
            SDL_Rect dstrect_pipe = {210, 10, 40, 40};
            SDL_RenderCopy(renderer, texture_objet[0], NULL, &dstrect_pipe);
            SDL_Rect dstrect_ampoule = {270, 10, 40, 40};
            SDL_RenderCopy(renderer, texture_objet[1], NULL, &dstrect_ampoule);
            SDL_Rect dstrect_poing = {330, 10, 40, 40};
            SDL_RenderCopy(renderer, texture_objet[2], NULL, &dstrect_poing);
            SDL_Rect dstrect_couronne = {390, 10, 40, 40};
            SDL_RenderCopy(renderer, texture_objet[3], NULL, &dstrect_couronne);
            SDL_Rect dstrect_carnet = {450, 10, 40, 40};
            SDL_RenderCopy(renderer, texture_objet[4], NULL, &dstrect_carnet);
            SDL_Rect dstrect_collier = {510, 10, 40, 40};
            SDL_RenderCopy(renderer, texture_objet[5], NULL, &dstrect_collier);
            SDL_Rect dstrect_oeil = {570, 10, 40, 40};
            SDL_RenderCopy(renderer, texture_objet[6], NULL, &dstrect_oeil);
            SDL_Rect dstrect_crane = {630, 10, 40, 40};
            SDL_RenderCopy(renderer, texture_objet[7], NULL, &dstrect_crane);
        }

        SDL_Color col1 = {0, 0, 0};
        for (i = 0; i < 8; i++)
        {
            SDL_Surface *surfaceMessage = TTF_RenderText_Solid(Sans, nbobjets[i], col1);
            SDL_Texture *Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
            SDL_Rect Message_rect;
            Message_rect.x = 230 + i * 60;
            Message_rect.y = 50;
            Message_rect.w = surfaceMessage->w;
            Message_rect.h = surfaceMessage->h;
            SDL_RenderCopy(renderer, Message, NULL, &Message_rect);
            SDL_DestroyTexture(Message);
            SDL_FreeSurface(surfaceMessage);
        }

        for (i = 0; i < 13; i++)
        {
            SDL_Surface *surfaceMessage = TTF_RenderText_Solid(Sans, nbnoms[i], col1);
            SDL_Texture *Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
            SDL_Rect Message_rect;
            Message_rect.x = 105;
            Message_rect.y = 350 + i * 30;
            Message_rect.w = surfaceMessage->w;
            Message_rect.h = surfaceMessage->h;
            SDL_RenderCopy(renderer, Message, NULL, &Message_rect);
            SDL_DestroyTexture(Message);
            SDL_FreeSurface(surfaceMessage);
        }

        for (i = 0; i < 4; i++)
            for (j = 0; j < 8; j++)
            {
                if (tableCartes[i][j] != -1)
                {
                    char mess[10];
                    if (tableCartes[i][j] == 100)
                        sprintf(mess, "*");
                    else
                        sprintf(mess, "%d", tableCartes[i][j]);
                    SDL_Surface *surfaceMessage = TTF_RenderText_Solid(Sans, mess, col1);
                    SDL_Texture *Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
                    SDL_Rect Message_rect;
                    Message_rect.x = 230 + j * 60;
                    Message_rect.y = 110 + i * 60;
                    Message_rect.w = surfaceMessage->w;
                    Message_rect.h = surfaceMessage->h;
                    SDL_RenderCopy(renderer, Message, NULL, &Message_rect);
                    SDL_DestroyTexture(Message);
                    SDL_FreeSurface(surfaceMessage);
                }
            }

        // Affichage simplifié des icônes pour certains personnages (exemple)
        {
            SDL_Rect dstrect = {750, 0, 250, 165};
            if(b[0] != -1)
                SDL_RenderCopy(renderer, texture_deck[b[0]], NULL, &dstrect);
            dstrect.y = 200;
            if(b[1] != -1)
                SDL_RenderCopy(renderer, texture_deck[b[1]], NULL, &dstrect);
            dstrect.y = 400;
            if(b[2] != -1)
                SDL_RenderCopy(renderer, texture_deck[b[2]], NULL, &dstrect);
        }

        // Le bouton go
        if (goEnabled == 1)
        {
            SDL_Rect dstrect = {500, 350, 200, 150};
            SDL_RenderCopy(renderer, texture_gobutton, NULL, &dstrect);
        }
        // Le bouton connect
        if (connectEnabled == 1)
        {
            SDL_Rect dstrect = {0, 0, 200, 50};
            SDL_RenderCopy(renderer, texture_connectbutton, NULL, &dstrect);
        }

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(texture_deck[0]);
    SDL_DestroyTexture(texture_deck[1]);
    SDL_FreeSurface(deck[0]);
    SDL_FreeSurface(deck[1]);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

