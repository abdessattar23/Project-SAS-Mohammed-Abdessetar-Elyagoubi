#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


struct Client{
    int id;
    char fullname[70];
    int phone;
    char pass[20];
    int tentative;
    int role;
}clients[100];
int count = 0;
void signup();
int verifyPassword(char pass[]);
void signin();

void checklogin(int id, char pass[], int index);
int lock(char pass[]);

void admindash();

void clientdash();

void moderatordash();

int admin = 0;
char passw[20] = "admin";


void signup(){
    printf("Entrer votre nom complet : ");
    fgets(clients[count].fullname, 70, stdin);
    clients[count].fullname[strcspn(clients[0].fullname, "\n")] = '\0';
    printf("Entrer votre numero de telephone : ");
    scanf("%d", &clients[count].phone);
    getchar();
    printf("Entrer votre mot de passe : ");
    fgets(clients[count].pass, 20, stdin);
    clients[count].pass[strcspn(clients[count].pass, "\n")] = '\0';
    if(verifyPassword(clients[count].pass) == 0){
        printf("Le mot de passe doit contenir au moins 8 caracteres dont une majuscule, une minuscule, un chiffre et un caractere special !\n");
        printf("Entrer un  nouveau mot de passe : ");

        fgets(clients[count].pass, 20, stdin);
        clients[count].pass[strcspn(clients[count].pass, "\n")] = '\0';
    }
    clients[count].id = count + 200;
    clients[count].tentative = 0;
    clients[count].role = 1;
    printf("Votre Compte a ete cree avec succes !\n");
    printf("Votre identifiant est : %d\n", clients[count].id);
}

int verifyPassword(char pass[]){
    int special = 0, capital = 0, min = 0, num = 0, length=0;
    if(strlen(pass) >= 8){
        length = 1;
    }
    for(int i = 0;i<strlen(pass); i++){
        if(pass[i] >= 65 && pass[i] <= 90){
            capital = 1;
        }
        else if(pass[i] >= 97 && pass[i] <= 122){
            min = 1;
        }
        else if(pass[i] >= 48 && pass[i] <= 57){
            num = 1;
        }
        else if(pass[i] >= 33 && pass[i] <= 47 || pass[i] >= 58 && pass[i] <= 64 || pass[i] >= 91 && pass[i] <= 96 || pass[i] >= 123 && pass[i] <= 126){
            special = 1;
        }
    }
    if(capital && min && num && special){
        return 1;
    }
    else{
        return 0;
    }
}


void checklogin(int id, char pass[], int index){
    char pass2[20];
    if(id == clients[0].id){
        printf("id correcte\n");
            if(clients[0].tentative < 3){
                if(strcmp(pass,clients[0].pass) == 0){
                    if(id == admin && strcmp(pass, passw) == 0){
                        admindash();
                    }
                    if(clients[0].role == 1){
                        clientdash();
                    }else if(clients[0].role == 2){
                        moderatordash();
                    }
                }else{
                    clients[0].tentative += 1;
                    printf("Mot de pass Incorrect!!\n");
                    printf("Entrer Votre mot de pass: ");
                    fgets(pass2, 20, stdin);
                    pass2[strcspn(pass2, "\n")] = '\0';
                    checklogin(id, pass2, 0);
                    }
                
            }else{
                printf("Vous avez depasse le nombre de tentative !\n");
                printf("Merci de attendez 24h avant de reessayer\n");

            }
            
        }else{
            printf("Il n'existe pas de compte avec cet identifiant\n");
        }
}
void signin(){
    int id;
    char pass[20];
    printf("Entrer votre Identifiant: ");
    scanf("%d", &id);
    getchar();
    printf("Entrer votre mot de passe : ");
    fgets(pass, 20, stdin);
    pass[strcspn(pass, "\n")] = '\0';
    for(int i =0; i<count; i++){
        checklogin(id, pass, i);
        
    }
    
}
void admindash(){
    printf("Admin Dash");
}
void clientdash(){
    printf("client Dash");
}

void moderatordash(){
    printf("mod Dash");
}

int main(){
    signup();
    signin();
    //return 0;
}