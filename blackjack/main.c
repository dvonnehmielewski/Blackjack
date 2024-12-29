#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "deck.h"
#include "card.h"
#include "hand.h"


//contains all the game information
typedef struct Game{
    Hand playerHand;
    Hand dealerHand;
    int playerChips;
    int bet;
    Deck deck;
}Game;

int RunGame(Game* r, int bet);
void SaveGameToFile(Game game);
Game LoadGameFromFile();
void SaveDeckToFile(Deck deck, FILE *file);
Deck LoadDeckFromFile(FILE *file);
void SaveHandToFile(Hand hand, FILE *file);
Hand LoadHandFromFile(FILE *file);

int main(int argc, char** argv){
   
    Game game;
    game.playerHand = MakeHand();
    game.dealerHand = MakeHand();
    game.playerChips = 1000;
    game.deck = MakeDeck();

    int loaded = argc > 1;

  while(game.playerChips > 0){
   
    if(loaded == 0){
        printf("\nYou have %d chips\n",game.playerChips);
        printf("How much would you like to bet?\n");
        scanf("%d",&game.bet);
        int c;
        while ((c = getchar()) != '\n' && c != EOF) { } //clear input buffer to avoid issues
        if(game.bet == 0){
            break; //we're done
        }
       
        //this is how we deal cards
        AddCard(&game.playerHand, DrawCard(&game.deck));
        AddCard(&game.dealerHand, DrawCard(&game.deck));
        AddCard(&game.playerHand, DrawCard(&game.deck));
        AddCard(&game.dealerHand, DrawCard(&game.deck));
    }
    else{
        loaded = 0;
        game = LoadGameFromFile();
    }

    if(CalculatePoints(&game.playerHand) == 21){
        printf("You were dealt a 21!");
        PrintHand(game.playerHand);
        game.playerChips += (int)(double)game.bet/2.0;
        continue;
    }

    game.playerChips += RunGame(&game, game.bet);
    game.playerHand = MakeHand();
    game.dealerHand = MakeHand();
    game.deck = MakeDeck();
  }


  printf("Game over.\n");
  printf("You left with %d chips.\n", game.playerChips);
}

int RunGame(Game* r, int bet) { 

    printf("\n");

    int pPoints = CalculatePoints(&r->playerHand);
    int dPoints = CalculatePoints(&r->dealerHand);
    char pick;

    if(pPoints > 21) {
        while (dPoints < 16) {
            AddCard(&r->dealerHand, DrawCard(&r->deck));
            dPoints = CalculatePoints(&r->dealerHand);
        }
        if(dPoints > 21) {
            printf("Both busted, game results in tie\n");
            return 0;
        }
        else {
             printf("You busted\n");
             printf("Your Score: %d\n", pPoints);
             PrintHand(r->playerHand);
             printf("Dealer score: %d\n",dPoints);
             PrintHand(r->dealerHand);
             return -bet;
        }
    }
   
    int dealerOne = GetCardPoints(r->dealerHand.cards[0]);
    printf("Dealer Score: %d + ???\n", dealerOne);
    printf("Dealer Hand: ");
    PrintCard(r->dealerHand.cards[0]);
    printf(", ??? of ???\n");
    printf("Player Score: %d\n", pPoints);
    printf("Player Hand: ");
    PrintHand(r->playerHand);
    printf("Will you hit 'h' or stay 's' or take a break 'b'?\n");
    scanf(" %c", &pick);

    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
   
    if (pick == 'h') {
        AddCard(&r->playerHand, DrawCard(&r->deck));
        return RunGame(r, bet);
    }

    if (pick == 'b') {
        printf("Saving Game...");
        SaveGameToFile(*r);
        exit(0);
    }

    if (pick == 's') {

        while (dPoints < 16) {
            AddCard(&r->dealerHand, DrawCard(&r->deck));
            dPoints = CalculatePoints(&r->dealerHand);
        }

        if (dPoints > 21) {

            printf("\nWON\n");
            printf("Your Score: %d\n", pPoints);
            PrintHand(r->playerHand);
            printf("Dealer score: %d\n",dPoints);
            PrintHand(r->dealerHand);
            return bet;

        }
        
        if (pPoints==dPoints) {

            printf("\nTIED\n");
            printf("Your Score: %d\n", pPoints);
            PrintHand(r->playerHand);
            printf("Dealer score: %d\n",dPoints);
            PrintHand(r->dealerHand);
            return 0;
        }

        else if (pPoints>dPoints) {

            printf("\nWON\n");
            printf("Your Score: %d\n", pPoints);
            PrintHand(r->playerHand);
            printf("Dealer score: %d\n",dPoints);
            PrintHand(r->dealerHand);
            return bet;
        }

        else{

            printf("\nLOST\n");
            printf("Your Score: %d\n", pPoints);
            PrintHand(r->playerHand);
            printf("Dealer score: %d\n",dPoints);
            PrintHand(r->dealerHand);
            return -bet;
        }
    }
    return 0;
}

void SaveGameToFile(Game game) {

    FILE* outFile = NULL;  
    outFile = fopen("save.txt", "w");

    if (outFile == NULL) {
       printf("ERROR opening save.txt.\n");
    }

    fprintf(outFile,"%d\n",game.playerChips);
    fprintf(outFile, "%d\n",game.bet);
    SaveHandToFile(game.dealerHand, outFile);
    SaveHandToFile(game.playerHand, outFile);
    SaveDeckToFile(game.deck,outFile);

    fclose(outFile);  

}

Game LoadGameFromFile() {

    Game game;
    FILE* file = NULL;  
    file = fopen("save.txt", "r");

    if (file == NULL) {
       printf("ERROR opening save.txt.\n");
       exit(10);
    }

    char temp[10];
    fgets(temp, 10, file);
    sscanf(temp, "%d" , &game.playerChips);

    temp[0] = '\0';
    fgets(temp,10,file);
    sscanf(temp, "%d", &game.bet);

    game.dealerHand = LoadHandFromFile(file);
    game.playerHand = LoadHandFromFile(file);

    game.deck = LoadDeckFromFile(file);
    fclose(file);
   
    return game;
}

void SaveHandToFile(Hand hand, FILE *file) {

    int handSize;
    fprintf(file,"%d\n",hand.handSize);

    for (int i = 0; i<hand.handSize; i++) {
        fprintf(file,"%d$", hand.cards[i].value);
        switch(hand.cards[i].suit) {

            case HEARTS:
                fprintf(file,"HEARTS\n");
                break;
            case DIAMONDS:
                fprintf(file,"DIAMONDS\n");
                break;
            case CLUBS:
                fprintf(file,"CLUBS\n");
                break;
            case SPADES:
                fprintf(file,"SPADES\n");
                break;
            default:
                printf("ERROR\n");
                break;
        }    
    }
}

Hand LoadHandFromFile(FILE *file) {

    Hand a;
    char* scan = (char*) malloc(50 * sizeof(char));
    fgets (scan, 50 , file);
    sscanf(scan, "%d", &a.handSize);
    scan[0]='\0';

    for (int i = 0; i<a.handSize; i++) {
        char check[10];
        fgets (scan, 50 , file);
        sscanf(scan, "%d$%s", &a.cards[i].value, check);

        if (strcmp(check,"HEARTS") == 0) {
            a.cards[i].suit = HEARTS;
        } 
        else if (strcmp(check, "DIAMONDS") == 0) {
            a.cards[i].suit = DIAMONDS;
        } 
        else if (strcmp(check, "CLUBS") == 0) {
            a.cards[i].suit = CLUBS;
        } 
        else if (strcmp(check, "SPADES") == 0) {
            a.cards[i].suit = SPADES;
        }
       
        scan[0]='\0';
    }
    free(scan);
    return a;
}

void SaveDeckToFile(Deck deck, FILE *file) {

    fprintf(file, "%d\n", deck.top);
    for (int i = deck.top; i<DECK_SIZE ; i++) {

        fprintf(file,"%d$", deck.cards[i].value);
        switch(deck.cards[i].suit) {

            case HEARTS:
                fprintf(file,"HEARTS\n");
                break;
            case DIAMONDS:
                fprintf(file,"DIAMONDS\n");
                break;
            case CLUBS:
                fprintf(file,"CLUBS\n");
                break;
            case SPADES:
                fprintf(file,"SPADES\n");
                break;
            default:
                printf("ERROR\n");
                break;
        }
    }
}

Deck LoadDeckFromFile(FILE *file) {

    Deck b;
    char* scan = (char*) malloc(50 * sizeof(char));
    fgets (scan, 50 , file);
    sscanf(scan, "%d", &b.top);

    for (int i = b.top; i< DECK_SIZE; i++){

        char check[10];
        fgets (scan, 50 , file);
        sscanf(scan, "%d$%s",&b.cards[i].value, check);

        if (strcmp(check,"HEARTS") == 0) {
            b.cards[i].suit = HEARTS;
        } 
        else if (strcmp(check, "DIAMONDS") == 0) {
            b.cards[i].suit = DIAMONDS;
        } 
        else if (strcmp(check, "CLUBS") == 0) {
            b.cards[i].suit = CLUBS;
        } 
        else if (strcmp(check,"SPADES") == 0) {
            b.cards[i].suit = SPADES;
        }
        scan[0]='\0';
    }
    return b;
}






