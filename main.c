#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define MAX_USERS 100

/* Global Declarations */
struct Users
{
    char fullname[50];
    char pass[20];
    int id;
    int phone;
    int tentative;
    int role;
} users[MAX_USERS];

struct Tickets
{
    char motif[50];
    char description[120];
    char categorie[20];
    int status;
    int id;
    int owner_id;
    time_t date;
} tickets[MAX_USERS * 10]; // MAX_USERS * 10 to let each user have 10 tickets.

int users_count = 0;
int tickets_count = 0;
char admin_user[] = "admin";
char admin_passwd[] = "admin";
int tentative_admin = 0;

/* Function Prototypes */
void signup();
void login_user();
void login_admin();
void manage_users();
void user_menu(int users_id);
void create(int user_id);
void modify_u();//modify a ticket as a client not admin
void display_user_tickets(int user_id);
void delete_user_ticket();
void admin_menu();
int check_creds(int id, char pass[]);
int verify_password(char pass[], char name[]);
void moderator_menu();
void manage_tickets_mod();

/* Signup Function */
void signup()
{
    printf("Entrer Votre nom complet: ");
    fgets(users[users_count].fullname, 50, stdin);
    users[users_count].fullname[strcspn(users[users_count].fullname, "\n")] = '\0';
    printf("Entrer votre mot de pass: ");
    fgets(users[users_count].pass, 50, stdin);
    users[users_count].pass[strcspn(users[users_count].pass, "\n")] = '\0';

    if (!verify_password(users[users_count].pass, users[users_count].fullname))
    {
        printf("Mot de pass doit contenir 8 characters au minimum et un character special et un nombre et une lettre majuscul et une lettre miniscul au minimum!!\n");
        signup();
        return;
    }

    printf("Entrer votre numero de telephone: ");
    scanf("%d", &users[users_count].phone);
    getchar();
    users[users_count].id = users_count + 1; // Use users_count as ID
    users[users_count].tentative = 0;
    users[users_count].role = 0;
    printf("Votre Nouveau profile est: \n");
    printf("[*] Identifiant: %d\n[*] Nom Complet: %s\n[*] Numero de telephone: %d\n[*] Mot de pass: %s\n", users[users_count].id, users[users_count].fullname, users[users_count].phone, users[users_count].pass);
    users_count++;
}

/*Modify a Ticket as a Client*/
void modify_u()
{
    int entred_ticket_id;
    time_t newt;
    printf("Entrer l'identifiant de la reclamation a modifier: ");
    scanf("%d", &entred_ticket_id);
    getchar();
    newt = time(NULL);
    double diff = difftime(newt, tickets[entred_ticket_id - 1].date);
    if (diff > 10)
    {
        printf("La reclamation a depassee le temps necessaire pour la modification\n");
        return;
    }

    printf("Entrer le nouveau motif: ");
    fgets(tickets[entred_ticket_id - 1].motif, 50, stdin);
    printf("Entrer la nouvelle description: ");
    fgets(tickets[entred_ticket_id - 1].description, 120, stdin);
    printf("Entrer la nouvelle categorie: ");
    fgets(tickets[entred_ticket_id - 1].categorie, 20, stdin);
    printf("La reclamation a modifiee avec succes!\n");
}

/* Create Ticket*/
void create(int user_id)
{
    time_t getnow;
    printf("Entrer le motif: ");
    fgets(tickets[tickets_count].motif, 50, stdin);
    tickets[tickets_count].motif[strcspn(tickets[tickets_count].motif, "\n")] = '\0';
    printf("Entrer la description: ");
    fgets(tickets[tickets_count].description, 120, stdin);
    tickets[tickets_count].description[strcspn(tickets[tickets_count].description, "\n")] = '\0';
    printf("Entrer la categorie: ");
    fgets(tickets[tickets_count].categorie, 20, stdin);
    tickets[tickets_count].categorie[strcspn(tickets[tickets_count].categorie, "\n")] = '\0';
    tickets[tickets_count].status = 0; //0 pending; 1: en cours; 2: resolu; 3: fermee
    tickets[tickets_count].owner_id = user_id;
    tickets[tickets_count].id = tickets_count + 1;
    getnow = time(NULL);
    tickets[tickets_count].date = getnow;
    printf("La reclamation a ete enregistree avec succes!\n");
    printf("Reclamation Identifiant: %d", tickets[tickets_count].id);
    tickets_count++;
}
/* Display all User tickets */
void display_user_tickets(int user_id)
{
    for(int i=0; i <tickets_count; i++)
    {
        if(tickets[i].owner_id == user_id)
        {
            char date[20];
            strftime(date, sizeof(date), "%d/%m/%y - %H:%M:%S", localtime(&tickets[i].date));
            printf("==============Reclamation %d==============\n", i);
            printf("Identifiant: %d\nMotif: %s\nDescription: %s\nCategorie: %s\nStatus: %d\nDate: %s\n", tickets[i].id,tickets[i].motif, tickets[i].description, tickets[i].categorie, tickets[i].status, date);
            printf("==========================================\n");
        }
    }
}
/* Delete User Ticket */
void delete_user_ticket()
{
    time_t newt;
    int target_id;
    printf("Entrer l'identifiant du reclamation a supprimer: ");
    scanf("%d", &target_id);
    getchar();

    if (target_id <= tickets_count)
    {
        for (int i = target_id; i < tickets_count - 1; i++)
        {
            tickets[i] = tickets[i + 1];
        }
        tickets_count--;
        printf("Reclamation supprime avec succes!\n");
    }
    else
    {
        printf("Reclamation ID invalide!\n");
    }

}
/* Password Verification Function */
int verify_password(char pass[], char name[])
{
    int capital = 0, mini = 0, special = 0, number = 0;
    char *n = strstr(pass, name);
    if (n)
    {
        return 0;
    }
    if (strlen(pass) >= 8)
    {
        for (int i = 0; i < strlen(pass); i++)
        {
            if (pass[i] >= 65 && pass[i] <= 90)
            {
                capital = 1;
            }
            if (pass[i] >= 97 && pass[i] <= 122)
            {
                mini = 1;
            }
            if ((pass[i] >= 33 && pass[i] <= 47) || (pass[i] >= 60 && pass[i] <= 64) || (pass[i] >= 123 && pass[i] <= 126))
            {
                special = 1;
            }
            if (pass[i] >= 48 && pass[i] <= 58)
            {
                number = 1;
            }
        }
        return (capital && mini && special && number) ? 1 : 0;
    }
    return 0;
}

/* Login User Function */
void login_user()
{
    int user_id;
    char user_pass[20];
    printf("Entrer votre Identifiant: ");
    scanf("%d", &user_id);
    getchar();
    printf("Entrer votre mot de pass: ");
    fgets(user_pass, 20, stdin);
    user_pass[strcspn(user_pass, "\n")] = '\0';

    int cred_check = check_creds(user_id, user_pass);
    if (cred_check == 1)
    {
        if (users[user_id].role == 0)
        {
            user_menu(user_id);
        }
        else if (users[user_id].role == 1)
        {
            moderator_menu();
        }
    }
    else if (cred_check == -1)
    {
        printf("Mot de pass Incorrect\n");
        users[user_id].tentative++;
        if (users[user_id].tentative >= 3)
        {
            printf("Vous avez depasse le nombre de tentative !\n");
            printf("Vous devez attendre 24h avant de ressayer !\n");
            return;
        }
        printf("Vous avez %d tentative a resseyer!!\n", 3 - users[user_id].tentative);
        login_user();
    }
    else if (cred_check == 0)
    {
        printf("Identifiant n'existe pas!!\n");
        login_user();
    }
}

/* Check Credentials Function */
int check_creds(int id, char pass[])
{
    for (int i = 0; i < users_count; i++)
    {
        if (id == users[i].id)
        {
            return (strcmp(pass, users[i].pass) == 0) ? 1 : -1;
        }
    }
    return 0;
}

/* Login Admin Function */
void login_admin()
{
    char admin[20];
    char admin_pass[20];
    printf("Entrer votre identifiant : ");
    fgets(admin, 20, stdin);
    admin[strcspn(admin, "\n")] = '\0';
    printf("Entrer votre mot de passe : ");
    fgets(admin_pass, 20, stdin);
    admin_pass[strcspn(admin_pass, "\n")] = '\0';

    if ((3 - tentative_admin) <= 0)
    {
        printf("Vous avez depasse le nombre de tentative !\n");
        printf("Vous devez attendre 24h avant de ressayer !\n");
        return;
    }

    if (strcmp(admin, admin_user) == 0 && strcmp(admin_pass, admin_passwd) == 0)
    {
        admin_menu();
    }
    else
    {
        tentative_admin++;
        printf("Mot de pass ou identification incorrecte !\n");
        printf("Vous avez %d tentative restante\n", 3 - tentative_admin);
        login_admin();
    }
}

/* Admin Menu Function */
void admin_menu()
{
    int choix;
    do
    {
        printf("===============MENU===============\n");
        printf("[1] Gestion des utilisateurs\n");
        printf("[2] Gestion des Reclamations\n");
        printf("[3] Gestion des statistiques\n");
        printf("[4] Retour\n");
        printf("==================================\n");
        printf("Entrer Votre choix: ");
        scanf("%d", &choix);
        getchar();
        switch (choix)
        {
        case 1:
            manage_users();
            break;
        case 2:
            login_user();
            break;
        case 3:
            login_admin();
            break;
        case 4:
            break;
        default:
            printf("Choix Incorrect!!!\n");
            break;
        }
    }
    while (choix != 4);
}

/* Manage Users Function */
void manage_users()
{
    int choix;
    do
    {
        printf("===============MENU===============\n");
        printf("[1] Changer le role d'un utilisateur\n");
        printf("[2] Supprimer un utilisateur\n");
        printf("[3] Afficher tous les utilisateurs\n");
        printf("[4] Retour\n");
        printf("==================================\n");
        printf("Entrer Votre choix: ");
        scanf("%d", &choix);
        getchar();

        switch (choix)
        {
        case 1:
        {
            int entred_user_id;
            printf("Entrer l'identifiant d'utilisateur a modifier: ");
            scanf("%d", &entred_user_id);
            getchar();
            printf("Entrer le nouveau role (1 pour moderateur, 0 pour client): ");
            scanf("%d", &users[entred_user_id].role);
            getchar();
            printf("Utilisateur modifiee avec succes!\n");
            break;
        }
        case 2:
        {
            int user_id;
            printf("Entrer l'identifiant d'utilisateur a supprimer: ");
            scanf("%d", &user_id);
            getchar();
            if (user_id < users_count)
            {
                for (int i = user_id; i < users_count - 1; i++)
                {
                    users[i] = users[i + 1];
                }
                users_count--;
                printf("Utilisateur supprime avec succes!\n");
            }
            else
            {
                printf("Identifiant invalide!\n");
            }
            break;
        }
        case 3:
            for (int i = 0; i < users_count; i++)
            {
                printf("===========Utilisateur %d================\n", i);
                printf("Nom: %s\n", users[i].fullname);
                printf("Mot de passe: %s\n", users[i].pass);
                printf("Identifiant: %d\n", users[i].id);
                printf("Telephone: %d\n", users[i].phone);
                printf("Tentative: %d\n", users[i].tentative);
                printf("Role: %d\n", users[i].role);
                printf("==================================\n");
            }
            break;
        case 4:
            break;
        default:
            printf("Choix Incorrect!!!\n");
            break;
        }
    }
    while (choix != 4);
}

/* User Menu Function */
void user_menu(int user_id)
{
    int choix;
    do
    {
        printf("===============MENU===============\n");
        printf("[1] Cree une reclamation\n");
        printf("[2] Modifier une reclamation\n");
        printf("[3] Supprimer une reclamation\n");
        printf("[4] Afficher tous mes reclamations\n");
        printf("[5] Retour\n");
        printf("==================================\n");
        printf("Entrer Votre choix: ");
        scanf("%d", &choix);
        getchar();
        switch (choix)
        {
        case 1:
            create(user_id);
            break;
        case 2:
            modify_u();
            break;
        case 3:
            delete_user_ticket();
            break;
        case 4:
            display_user_tickets(user_id);
        case 5:
            break;
        default:
            printf("Choix Incorrect!!!\n");
            break;
        }
    }
    while (choix != 5);
}

/* Moderator Menu */
void moderator_menu()
{
    int choix;
    do
    {
        printf("===============MENU===============\n");
        printf("[1] Gestion des Reclamations\n");
        printf("[2] Traitement des Reclamations\n");
        printf("[3] Retour\n");
        printf("==================================\n");
        printf("Entrer Votre choix: ");
        scanf("%d", &choix);
        getchar();
        switch (choix)
        {
        case 1:
            manage_tickets_mod();
            break;
        case 2:
            modify_u();
            break;
        case 3:
            break;
        default:
            printf("Choix Incorrect!!!\n");
            break;
        }
    }
    while (choix != 3);
}

/* Manage Tickets Moderator*/
void manage_tickets_mod(){
    int choix;
    do
    {
        printf("===============MENU===============\n");
        printf("[1] Afficher les Reclamations\n");
        printf("[2] Modifier une Reclamations\n");
        printf("[3] Supprimer une Reclamations\n");
        printf("[4] Retour\n");
        printf("==================================\n");
        printf("Entrer Votre choix: ");
        scanf("%d", &choix);
        getchar();
        switch (choix)
        {
        case 1:
            for (int i = 0; i < tickets_count; i++)
            {
                char date[20];
                strftime(date, sizeof(date), "%d/%m/%y - %H:%M:%S", localtime(&tickets[i].date));
                printf("===========Reclamation %d================\n", i);
                printf("Identifiant: %d\n", tickets[i].id);
                printf("Motif: %s\n", tickets[i].motif);
                printf("Description: %s\n", tickets[i].description);
                printf("Categorie: %s\n", tickets[i].categorie);
                printf("Status: %d\n", tickets[i].status);
                printf("Date: %s\n", date);
                printf("Owner ID: %d\n", tickets[i].owner_id);
                printf("=========================================\n");
            }
            break;
        case 2:
            modify_u();
            break;
        case 4:
            break;
        default:
            printf("Choix Incorrect!!!\n");
            break;
        }
    }
    while (choix != 4);
}

/* Menu Function */
void menu()
{
    int choix;
    do
    {
        printf("===============MENU===============\n");
        printf("[1] Inscrire en tant que client\n");
        printf("[2] Log In en tant que utilisateur\n");
        printf("[3] Log In en tant que Administrateur\n");
        printf("[4] Quitter\n");
        printf("==================================\n");
        printf("Entrer Votre choix: ");
        scanf("%d", &choix);
        getchar();
        switch (choix)
        {
        case 1:
            signup();
            break;
        case 2:
            login_user();
            break;
        case 3:
            login_admin();
            break;
        case 4:
            break;
        default:
            printf("Choix Incorrect!!!\n");
            break;
        }
    }
    while (choix != 4);
}

int main()
{
    menu();
    return 0;
}
