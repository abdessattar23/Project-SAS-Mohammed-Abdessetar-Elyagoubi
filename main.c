#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define MAX_USERS 100
#include <ctype.h>

/* Global Declarations */
struct Users
{
    char fullname[50];
    char pass[20];
    int id;
    int phone;
    int tentative;
    time_t last_attemp;
    int role;
} users[MAX_USERS];

struct Tickets
{
    char motif[50];
    char description[120];
    char categorie[20];
    int status;
    int id; // Unique id for each ticket
    int owner_id;
    time_t date;
    int timetoberesolved;
    int priority;
} tickets[MAX_USERS * 10]; // MAX_USERS * 10 to let each user have 10 tickets.

int users_count = 0;
int tickets_count = 0;
int id_counter = 1; // To generate unique ids
char admin_user[] = "admin";
char admin_passwd[] = "admin";
int tentative_admin = 0;
time_t attemp_admin = 0;

/* Function Prototypes */
// authentication functions
void signup();
void login_user();
void login_admin();
int check_creds(int id, char pass[]);
int verify_password(char pass[], char name[]);

// menus
void menu();
void user_menu(int users_id);
void admin_menu();
void moderator_menu();

// user management
void manage_users();
void create(int user_id);
void modify_u();
void delete_user_ticket(int user_id);

// ticket management
void manage_tickets_mod();
void display_user_tickets(int user_id);
void delete_mod_ticket();

// search
void search();
void searchbyID(int target_id);
void searchbyStatus(int stat);
void searchbyCat(char cat[]);
void searchbyDate(char date[]);
void searchbyCID(int target_id);

// ticket proccessing and rapport generation
void process();
void process_tickets();
int prior(char desc[]);
void rapport();

/* Functions */
void signup()
{
    printf("\n************************************************************\n");
    printf("                   *** CREATION DE PROFIL ***               \n");
    printf("************************************************************\n");

    // Prompt for full name
    printf("Entrer Votre nom complet: ");
    fgets(users[users_count].fullname, 50, stdin);
    users[users_count].fullname[strcspn(users[users_count].fullname, "\n")] = '\0'; // Remove newline

    // Prompt for password
    printf("Entrer votre mot de passe: ");
    fgets(users[users_count].pass, 50, stdin);
    users[users_count].pass[strcspn(users[users_count].pass, "\n")] = '\0'; // Remove newline
    // Password validation
    if (!verify_password(users[users_count].pass, users[users_count].fullname))
    {
        system("@cls||clear");
        printf("\n************************************************************\n");
        printf("ERREUR: Mot de passe invalide!\n");
        printf("Le mot de passe doit contenir au minimum:\n");
        printf("  - 8 caracteres\n  - Un caractere special\n  - Un chiffre\n  - Une lettre majuscule\n  - Une lettre minuscule\n");
        printf("************************************************************\n\n");
        signup(); // Recursively call signup on invalid input
        return;
    }

    // Prompt for phone number
    printf("Entrer votre numero de telephone: ");
    scanf("%d", &users[users_count].phone);
    getchar(); // Clear the newline character left by scanf

    // Assign user details
    users[users_count].id = users_count + 1;
    users[users_count].tentative = 0;
    users[users_count].role = 0;
    system("@cls||clear");
    // Display newly created profile
    printf("\n************************************************************\n");
    printf("         *** PROFIL CRee AVEC SUCCeS ***                    \n");
    printf("************************************************************\n");
    printf("[*] Identifiant         : %d\n", users[users_count].id);
    printf("[*] Nom Complet         : %s\n", users[users_count].fullname);
    printf("[*] Numero de Telephone : %d\n", users[users_count].phone);
    printf("[*] Mot de Passe        : %s\n", users[users_count].pass);
    printf("************************************************************\n\n");

    users_count++; // Increment user count
}

/* Login User Function */
void login_user()
{
    system("@cls||clear");
    int user_id;
    char user_pass[20];

    printf("\n************************************************************\n");
    printf("                     *** CONNEXION ***                      \n");
    printf("************************************************************\n");

    // Prompt for user ID
    printf("Entrer votre Identifiant: ");
    scanf("%d", &user_id);
    getchar(); // Clear newline after scanf

    // Check if the user is temporarily locked
    if (users[user_id].tentative >= 3 && difftime(time(NULL), users[user_id].last_attemp) < 30)
    {
        printf("\nVous avez depasse le nombre de tentatives autorisees !\n");
        printf("Veuillez attendre 24h avant de reessayer.\n");
        return;
    }

    // Prompt for password
    printf("Entrer votre mot de passe: ");
    fgets(user_pass, 20, stdin);
    user_pass[strcspn(user_pass, "\n")] = '\0'; // Remove newline

    // Check credentials
    int cred_check = check_creds(user_id, user_pass);
    if (cred_check == 1)
    {                                 // Successful login
        users[user_id].tentative = 0; // Reset attempts on success

        // Redirect to user or moderator menu based on role
        if (users[user_id].role == 0)
        {
            user_menu(user_id); // Regular user
        }
        else if (users[user_id].role == 1)
        {
            moderator_menu(); // Moderator
        }
    }
    else if (cred_check == -1)
    { // Incorrect password
        printf("\nMot de passe incorrect!\n");
        users[user_id].tentative++;

        // Record first failed attempt time
        if (users[user_id].tentative >= 3 && users[user_id].last_attemp == 0)
        {
            time(&users[user_id].last_attemp);
        }

        // Lock user if too many failed attempts
        if (users[user_id].tentative >= 3 && difftime(time(NULL), users[user_id].last_attemp) < 30)
        {
            printf("\nVous avez depasse le nombre de tentatives autorisees !\n");
            printf("Veuillez attendre 24h avant de reessayer.\n");
            return;
        }

        // Notify remaining attempts
        printf("Vous avez encore %d tentative(s) !\n", 3 - users[user_id].tentative);
        login_user(); // Recursive call to retry
    }
    else if (cred_check == 0)
    { // User ID does not exist
        printf("\nIdentifiant n'existe pas!\n");
        login_user(); // Recursive call to retry
    }
}

/* Login Admin Function */
void login_admin()
{
    system("@cls||clear");
    char admin[20];
    char admin_pass[20];

    printf("\n************************************************************\n");
    printf("                     *** LOGIN ADMIN ***                   \n");
    printf("************************************************************\n");

    printf("Entrer votre identifiant : ");
    fgets(admin, 20, stdin);
    admin[strcspn(admin, "\n")] = '\0';

    printf("Entrer votre mot de passe : ");
    fgets(admin_pass, 20, stdin);
    admin_pass[strcspn(admin_pass, "\n")] = '\0';

    if (tentative_admin >= 3)
    {
        time(&attemp_admin);
    }

    if (tentative_admin >= 3 && difftime(time(NULL), attemp_admin) < 86400)
    { // 86400 seconds = 24 hours
        printf("\n*** Vous avez depasse le nombre de tentative ! ***\n");
        printf("*** Vous devez attendre 24h avant de ressayer ! ***\n");
        return;
    }

    if ((3 - tentative_admin) <= 1)
    {
        printf("\n*** Vous avez depasse le nombre de tentative ! ***\n");
        printf("*** Vous devez attendre 24h avant de ressayer ! ***\n");
        return;
    }

    if (strcmp(admin, admin_user) == 0 && strcmp(admin_pass, admin_passwd) == 0)
    {
        admin_menu();
    }
    else
    {
        tentative_admin++;
        printf("\n*** Mot de pass ou identification incorrecte ! ***\n");
        printf("*** Vous avez %d tentative restante ***\n", 3 - tentative_admin);
        login_admin();
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
        for (int i = 0; i < (int)strlen(pass); i++)
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

/* Menu Function */
void menu()
{
    system("@cls||clear");
    int choix;

    do
    {
        printf("\n************************************************************\n");
        printf("                        *** MAIN MENU ***                  \n");
        printf("************************************************************\n");
        printf("[1] Inscrire en tant que client\n");
        printf("[2] Log In en tant que utilisateur\n");
        printf("[3] Log In en tant que Administrateur\n");
        printf("[4] Quitter\n");
        printf("************************************************************\n");
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
            printf("\n*** Quitter le programme... ***\n");
            break;
        default:
            printf("\n*** Choix Incorrect! ***\n");
            break;
        }
    } while (choix != 4);

    printf("\n************************************************************\n");
    printf("                    FIN DE SESSION                          \n");
    printf("************************************************************\n");
}

/* User Menu Function */
void user_menu(int user_id)
{
    system("@cls||clear");
    int choix;
    do
    {
        printf("\n************************************************************\n");
        printf("                        *** USER MENU ***                  \n");
        printf("************************************************************\n");
        printf("[1] Cree une reclamation\n");
        printf("[2] Modifier une reclamation\n");
        printf("[3] Supprimer une reclamation\n");
        printf("[4] Afficher tous mes reclamations\n");
        printf("[5] Retour\n");
        printf("************************************************************\n");
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
            delete_user_ticket(user_id);
            break;
        case 4:
            display_user_tickets(user_id);
            break;
        case 5:
            printf("\n*** Retour au menu principal ***\n");
            break;
        default:
            printf("\n*** Choix Incorrect! ***\n");
            break;
        }
    } while (choix != 5);

    printf("\n************************************************************\n");
    printf("                    FIN DE SESSION                          \n");
    printf("************************************************************\n");
}

/* Admin Menu Function */
void admin_menu()
{
    system("@cls||clear");
    int choix;

    do
    {
        printf("\n************************************************************\n");
        printf("                     *** ADMIN MENU ***                    \n");
        printf("************************************************************\n");

        printf("[1] Gestion des Reclamations\n");
        printf("[2] Gestion des utilisateurs\n");
        printf("[3] Traitement des Reclamations\n");
        printf("[4] Generer un rapport\n");
        printf("[5] Retour\n");

        printf("************************************************************\n");
        printf("Entrer Votre choix: ");

        scanf("%d", &choix);
        getchar();

        switch (choix)
        {
        case 1:
            manage_tickets_mod();
            break;
        case 2:
            manage_users();
            break;
        case 3:
            process_tickets(); // Process Tickets as Mode
            break;
        case 4:
            rapport();
            break; // Added break to prevent fall-through
        case 5:
            break;
        default:
            printf("\n*** Choix Incorrect!!! ***\n");
            break;
        }

    } while (choix != 5);

    // End of the menu
    printf("\n************************************************************\n");
}

void moderator_menu()
{
    system("@cls||clear");
    int choix;

    do
    {
        printf("\n************************************************************\n");
        printf("                     *** MODERATOR MENU ***                \n");
        printf("************************************************************\n");
        printf("[1] Gestion des Reclamations\n");
        printf("[2] Traitement des Reclamations\n");
        printf("[3] Retour\n");
        printf("************************************************************\n");
        printf("Entrer Votre choix: ");
        scanf("%d", &choix);
        getchar();

        switch (choix)
        {
        case 1:
            manage_tickets_mod();
            break;
        case 2:
            process_tickets(); // Process Tickets as Mode
            break;
        case 3:
            printf("\n*** Retour au menu principal ***\n");
            break;
        default:
            printf("\n*** Choix Incorrect! ***\n");
            break;
        }
    } while (choix != 3);
}

/* Manage Users Function  */
void manage_users()
{
    system("@cls||clear");
    int choix;

    do
    {
        printf("\n************************************************************\n");
        printf("                     *** MANAGE USERS MENU ***             \n");
        printf("************************************************************\n");
        printf("[1] Changer le role d'un utilisateur\n");
        printf("[2] Supprimer un utilisateur\n");
        printf("[3] Afficher tous les utilisateurs\n");
        printf("[4] Retour\n");
        printf("************************************************************\n");
        printf("Entrer Votre choix: ");
        scanf("%d", &choix);
        getchar();

        switch (choix)
        {
        case 1:
        {
            int entred_user_id;
            printf("\nEntrer l'identifiant d'utilisateur a modifier: ");
            scanf("%d", &entred_user_id);
            getchar();
            printf("Entrer le nouveau role (1 pour moderateur, 0 pour client): ");
            scanf("%d", &users[entred_user_id].role);
            getchar();
            printf("\n*** Utilisateur modifiee avec succes! ***\n");
            break;
        }
        case 2:
        {
            int user_id;
            printf("\nEntrer l'identifiant d'utilisateur a supprimer: ");
            scanf("%d", &user_id);
            getchar();
            if (user_id > 0 && user_id <= users_count)
            {
                for (int i = user_id - 1; i < users_count - 1; i++)
                {
                    users[i] = users[i + 1];
                }
                users_count--;
                printf("\n*** Utilisateur supprime avec succes! ***\n");
            }
            else
            {
                printf("\n*** Identifiant invalide! ***\n");
            }
            break;
        }
        case 3:
            printf("\n************************************************************\n");
            printf("                 *** LISTE DES UTILISATEURS ***           \n");
            printf("************************************************************\n");
            for (int i = 0; i < users_count; i++)
            {
                printf("===========Utilisateur %d================\n", i);
                printf("Nom: %s\nMot de passe: %s\nIdentifiant: %d\nTelephone: %d\nTentative: %d\nRole: %d\n",
                       users[i].fullname, users[i].pass, users[i].id, users[i].phone, users[i].tentative, users[i].role);
                printf("==================================\n");
            }
            printf("************************************************************\n");
            break;
        case 4:
            printf("\n*** Retour au menu principal ***\n");
            break;
        default:
            printf("\n*** Choix Incorrect! ***\n");
            break;
        }
    } while (choix != 4);

    printf("\n************************************************************\n");
    printf("                     FIN DE GESTION                        \n");
    printf("************************************************************\n");
}

/* Create Ticket */
void create(int user_id)
{
    system("@cls||clear");
    time_t getnow;

    printf("\n************************************************************\n");
    printf("                     *** CREER UNE RECLAMATION ***          \n");
    printf("************************************************************\n");

    printf("Entrer le motif: ");
    fgets(tickets[tickets_count].motif, 50, stdin);
    tickets[tickets_count].motif[strcspn(tickets[tickets_count].motif, "\n")] = '\0';

    printf("Entrer la description: ");
    fgets(tickets[tickets_count].description, 120, stdin);
    tickets[tickets_count].description[strcspn(tickets[tickets_count].description, "\n")] = '\0';

    printf("Entrer la categorie: ");
    fgets(tickets[tickets_count].categorie, 20, stdin);
    tickets[tickets_count].categorie[strcspn(tickets[tickets_count].categorie, "\n")] = '\0';

    tickets[tickets_count].status = 0; // 0: en cours; 2: resolu; 3: fermee
    tickets[tickets_count].owner_id = user_id;
    tickets[tickets_count].id = id_counter++; // Assign unique id

    getnow = time(NULL);
    tickets[tickets_count].date = getnow;

    tickets[tickets_count].priority = 0;
    int pr = prior(tickets[tickets_count].description);
    tickets[tickets_count].priority = pr;

    printf("set ticket priority to '%d'\n", tickets[tickets_count].priority);
    printf("\n*** La reclamation a ete enregistree avec succes! ***\n");
    printf("*** Reclamation Identifiant: %d ***\n", tickets[tickets_count].id);
    printf("*** Owner: %d ***\n", tickets[tickets_count].owner_id);

    tickets_count++;

    printf("************************************************************\n");
}

/* Modify a Ticket as a Client */
void modify_u()
{
    system("@cls||clear");
    int entred_ticket_id;
    printf("\n************************************************************\n");
    printf("                *** MODIFIER UNE RECLAMATION ***           \n");
    printf("************************************************************\n");

    printf("Entrer l'identifiant de la reclamation a modifier: ");
    scanf("%d", &entred_ticket_id);
    getchar();

    for (int i = 0; i < tickets_count; i++)
    {
        if (tickets[i].id == entred_ticket_id && tickets[i].id != -1)
        {
            time_t newt = time(NULL);
            double diff = difftime(newt, tickets[i].date);
            if (diff > 10)
            {
                printf("\n*** La reclamation a depassee le temps necessaire pour la modification ***\n");
                return;
            }

            printf("Entrer le nouveau motif: ");
            fgets(tickets[i].motif, 50, stdin);
            printf("Entrer la nouvelle description: ");
            fgets(tickets[i].description, 120, stdin);
            printf("Entrer la nouvelle categorie: ");
            fgets(tickets[i].categorie, 20, stdin);
            printf("\n*** La reclamation a modifiee avec succes! ***\n");
            return;
        }
    }

    printf("\n*** Reclamation ID invalide ou introuvable! ***\n");

    printf("************************************************************\n");
}

/* Modify a Ticket as a Moderator */
void modify_m()
{
    system("@cls||clear");
    int entred_ticket_id;

    printf("\nEntrer l'identifiant de la reclamation a modifier: ");
    scanf("%d", &entred_ticket_id);
    getchar();

    printf("Entrer le nouveau motif: ");
    fgets(tickets[entred_ticket_id - 1].motif, 50, stdin);

    printf("Entrer la nouvelle description: ");
    fgets(tickets[entred_ticket_id - 1].description, 120, stdin);

    printf("Entrer la nouvelle categorie: ");
    fgets(tickets[entred_ticket_id - 1].categorie, 20, stdin);

    printf("\n*** La reclamation a modifiee avec succes! ***\n");
}

/* Delete User Ticket */
void delete_user_ticket(int user_id)
{
    system("@cls||clear");
    int target_id;
    printf("\n************************************************************\n");
    printf("              *** SUPPRIMER UNE RECLAMATION ***            \n");
    printf("************************************************************\n");

    printf("Entrer l'identifiant du reclamation a supprimer: ");
    scanf("%d", &target_id);
    getchar();

    time_t newt = time(NULL);
    double diff = difftime(newt, tickets[target_id - 1].date);

    if (diff > 10)
    {
        printf("\n*** La reclamation a depassee le temps necessaire pour la suppression ***\n");
        return;
    }

    for (int i = 0; i < tickets_count; i++)
    {
        if (tickets[i].id == target_id && tickets[i].owner_id == user_id)
        {
            tickets[i].id = -1; // Mark as deleted
            printf("\n*** Reclamation supprime avec succes! ***\n");
            return;
        }
    }

    printf("\n*** Reclamation ID invalide ou vous n'avez pas le droit de supprimer cette reclamation! ***\n");

    printf("************************************************************\n");
}

void manage_tickets_mod()
{
    system("@cls||clear");
    int choix;

    do
    {
        printf("\n************************************************************\n");
        printf("                     *** MANAGE TICKETS MENU ***           \n");
        printf("************************************************************\n");
        printf("[1] Afficher les Reclamations\n");
        printf("[2] Modifier une Reclamation\n");
        printf("[3] Supprimer une Reclamation\n");
        printf("[4] Rechercher une Reclamation\n");
        printf("[5] Retour\n");
        printf("************************************************************\n");
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
                if (tickets[i].priority == 2)
                {
                    printf("===========Reclamation %d================\n", i);
                    printf("Identifiant: %d\n", tickets[i].id);
                    printf("Motif: %s\n", tickets[i].motif);
                    printf("Description: %s\n", tickets[i].description);
                    printf("Categorie: %s\n", tickets[i].categorie);
                    printf("Status: ");
                    switch (tickets[i].status)
                    {
                    case 0:
                        printf("en cours\n");
                        break;
                    case 1:
                        printf("resolved\n");
                        break;
                    case 2:
                        printf("fermee\n");
                        break;
                    }
                    printf("Date: %s\n", date);
                    printf("Owner ID: %d\n", tickets[i].owner_id);
                    printf("Priority: ");
                    switch (tickets[i].priority)
                    {
                    case 0:
                        printf("basse\n");
                        break;
                    case 1:
                        printf("moyenne\n");
                        break;
                    case 2:
                        printf("Haute\n");
                        break;
                    }
                    printf("=========================================\n");
                }
            }
            for (int i = 0; i < tickets_count; i++)
            {
                char date[20];
                strftime(date, sizeof(date), "%d/%m/%y - %H:%M:%S", localtime(&tickets[i].date));
                if (tickets[i].priority == 1)
                {
                    printf("===========Reclamation %d================\n", i);
                    printf("Identifiant: %d\n", tickets[i].id);
                    printf("Motif: %s\n", tickets[i].motif);
                    printf("Description: %s\n", tickets[i].description);
                    printf("Categorie: %s\n", tickets[i].categorie);
                    printf("Status: ");
                    switch (tickets[i].status)
                    {
                    case 0:
                        printf("en cours\n");
                        break;
                    case 1:
                        printf("resolved\n");
                        break;
                    case 2:
                        printf("fermee\n");
                        break;
                    }
                    printf("Date: %s\n", date);
                    printf("Owner ID: %d\n", tickets[i].owner_id);
                    printf("Priority: ");
                    switch (tickets[i].priority)
                    {
                    case 0:
                        printf("basse\n");
                        break;
                    case 1:
                        printf("moyenne\n");
                        break;
                    case 2:
                        printf("Haute\n");
                        break;
                    }
                    printf("=========================================\n");
                }
            }
            for (int i = 0; i < tickets_count; i++)
            {
                char date[20];
                strftime(date, sizeof(date), "%d/%m/%y - %H:%M:%S", localtime(&tickets[i].date));
                if (tickets[i].priority == 0)
                {
                    printf("===========Reclamation %d================\n", i);
                    printf("Identifiant: %d\n", tickets[i].id);
                    printf("Motif: %s\n", tickets[i].motif);
                    printf("Description: %s\n", tickets[i].description);
                    printf("Categorie: %s\n", tickets[i].categorie);
                    printf("Status: ");
                    switch (tickets[i].status)
                    {
                    case 0:
                        printf("en cours\n");
                        break;
                    case 1:
                        printf("resolved\n");
                        break;
                    case 2:
                        printf("fermee\n");
                        break;
                    }
                    printf("Date: %s\n", date);
                    printf("Owner ID: %d\n", tickets[i].owner_id);
                    printf("Priority: ");
                    switch (tickets[i].priority)
                    {
                    case 0:
                        printf("basse\n");
                        break;
                    case 1:
                        printf("moyenne\n");
                        break;
                    case 2:
                        printf("Haute\n");
                        break;
                    }
                    printf("=========================================\n");
                }
            }
            break;

        case 2:
            modify_m();
            break;

        case 3:
            delete_mod_ticket();
            break;

        case 4:
            search();
            break;

        case 5:
            printf("\n*** Retour au menu principal ***\n");
            break;

        default:
            printf("\n*** Choix Incorrect! ***\n");
            break;
        }
    } while (choix != 5);

    // End of the menu
    printf("\n************************************************************\n");
    printf("                     FIN DE GESTION                          \n");
    printf("************************************************************\n");
}

/* Display all User ticket */
void display_user_tickets(int user_id)
{
    system("@cls||clear");
    printf("\n************************************************************\n");
    printf("                  *** VOS RECLAMATIONS ***                 \n");
    printf("************************************************************\n");

    int found = 0; // Flag to check if any tickets are found

    for (int i = 0; i < tickets_count; i++)
    {
        if (tickets[i].owner_id == user_id && tickets[i].id != -1)
        { // Skip deleted tickets
            char date[20];
            strftime(date, sizeof(date), "%d/%m/%y - %H:%M:%S", localtime(&tickets[i].date));
            printf("============== Reclamation %d ==============\n", tickets[i].id);
            printf("Identifiant: %d\n", tickets[i].id);
            printf("Motif: %s\n", tickets[i].motif);
            printf("Description: %s\n", tickets[i].description);
            printf("Categorie: %s\n", tickets[i].categorie);
            printf("Status: %d\n", tickets[i].status); // Consider using a switch for better readability
            printf("Date: %s\n", date);
            printf("=============================================\n");
            found = 1; // Set flag to indicate at least one ticket was found
        }
    }

    if (!found)
    {
        printf("\n*** Aucune reclamation trouvee pour cet utilisateur. ***\n");
    }

    printf("************************************************************\n");
}

void delete_mod_ticket()
{
    system("@cls||clear");
    int target_id;
    printf("\n************************************************************\n");
    printf("              *** SUPPRIMER UNE RECLAMATION ***            \n");
    printf("************************************************************\n");
    printf("Entrer l'identifiant du reclamation a supprimer: ");
    scanf("%d", &target_id);
    getchar();

    if (target_id > 0 && target_id <= tickets_count)
    {
        for (int i = target_id - 1; i < tickets_count - 1; i++)
        {
            tickets[i] = tickets[i + 1];
        }
        tickets_count--;
        printf("\n*** Reclamation supprime avec succes! ***\n");
    }
    else
    {
        printf("\n*** Reclamation ID invalide! ***\n");
    }

    printf("************************************************************\n");
}

void search()
{
    system("@cls||clear");
    int choix, target_id, c_id, stat;
    char cat[20], date[9];

    do
    {
        printf("\n************************************************************\n");
        printf("                        *** MENU ***                        \n");
        printf("************************************************************\n");
        printf("[1] Rechercher par ID\n");
        printf("[2] Rechercher par Categorie\n");
        printf("[3] Rechercher par Date\n");
        printf("[4] Rechercher par Client\n");
        printf("[5] Rechercher par Statut\n");
        printf("[6] Retour\n");
        printf("************************************************************\n");
        printf("Entrer votre choix: ");
        scanf("%d", &choix);
        getchar(); // Clear newline character

        switch (choix)
        {
        case 1:
            printf("\nEntrer l'identifiant de la reclamation a rechercher: ");
            scanf("%d", &target_id);
            searchbyID(target_id);
            break;

        case 2:
            printf("\nEntrer la categorie a rechercher: ");
            fgets(cat, 20, stdin);
            cat[strcspn(cat, "\n")] = '\0'; // Remove newline
            searchbyCat(cat);
            break;

        case 3:
            printf("\nEntrer la date a rechercher sous la forme DD/MM/YY (ex: 01/01/24): ");
            fgets(date, 9, stdin);
            date[strcspn(date, "\n")] = '\0'; // Remove newline
            searchbyDate(date);
            break;

        case 4:
            printf("\nEntrer l'identifiant du client qui a soumis la reclamation: ");
            scanf("%d", &c_id);
            getchar(); // Clear newline
            searchbyCID(c_id);
            break;

        case 5:
            printf("\nEntrer le statut a rechercher (0 pour En Cours, 1 pour Resolu, 2 pour Fermee): ");
            scanf("%d", &stat);
            searchbyStatus(stat);
            break;

        case 6:
            printf("\nRetour au menu principal...\n");
            break;

        default:
            printf("\nErreur: Choix incorrect! Veuillez entrer un numero valide.\n");
            break;
        }

    } while (choix != 6);

    printf("\n************************************************************\n");
    printf("                    FIN DE RECHERCHE                        \n");
    printf("************************************************************\n");
}

void searchbyID(int target_id)
{
    system("@cls||clear");
    printf("\n************************************************************\n");
    printf("                *** TICKET SEARCH BY ID ***                 \n");
    printf("************************************************************\n");

    for (int i = 0; i < tickets_count; i++)
    {
        if (tickets[i].id == target_id)
        {
            printf("\n************************************************************\n");
            printf("* %-15s : %-40d *\n", "IDENTIFIANT", tickets[i].id);
            printf("* %-15s : %-40s *\n", "MOTIF", tickets[i].motif);
            printf("* %-15s : %-40s *\n", "DESCRIPTION", tickets[i].description);
            printf("* %-15s : %-40s *\n", "CATEGORIE", tickets[i].categorie);

            printf("* %-15s : ", "STATUS");
            switch (tickets[i].status)
            {
            case 0:
                printf("%-40s *\n", "EN COURS");
                break;
            case 1:
                printf("%-40s *\n", "RESOLVED");
                break;
            case 2:
                printf("%-40s *\n", "FERMEE");
                break;
            }

            printf("* %-15s : %-40d *\n", "OWNER ID", tickets[i].owner_id);
            printf("* %-15s : %-40s *\n", "DATE", ctime(&tickets[i].date));
            printf("* %-15s : %-40d *\n", "PRIORITY", tickets[i].priority);
            printf("************************************************************\n");
        }
    }

    printf("\n************************************************************\n");
    printf("                    END OF SEARCH RESULTS                    \n");
    printf("************************************************************\n");
}

void searchbyStatus(int stat)
{
    system("@cls||clear");
    printf("\n************************************************************\n");
    printf("                *** TICKETS SEARCH BY STATUS ***             \n");
    printf("************************************************************\n");

    int found = 0; // Flag to track if any ticket matches the status

    for (int i = 0; i < tickets_count; i++)
    {
        if (tickets[i].status == stat)
        {
            found = 1; // Mark that a ticket is found
            printf("\n************************************************************\n");
            printf("* %-15s : %-40d *\n", "IDENTIFIANT", tickets[i].id);
            printf("* %-15s : %-40s *\n", "MOTIF", tickets[i].motif);
            printf("* %-15s : %-40s *\n", "DESCRIPTION", tickets[i].description);
            printf("* %-15s : %-40s *\n", "CATEGORIE", tickets[i].categorie);

            printf("* %-15s : ", "STATUS");
            switch (tickets[i].status)
            {
            case 0:
                printf("%-40s *\n", "EN COURS");
                break;
            case 1:
                printf("%-40s *\n", "RESOLVED");
                break;
            case 2:
                printf("%-40s *\n", "FERMEE");
                break;
            }

            printf("* %-15s : %-40d *\n", "OWNER ID", tickets[i].owner_id);
            printf("* %-15s : %-40s *\n", "DATE", ctime(&tickets[i].date));
            printf("************************************************************\n");
        }
    }

    if (!found)
    {
        printf("\nNo tickets found with STATUS: %d\n", stat);
    }

    printf("\n************************************************************\n");
    printf("                    END OF SEARCH RESULTS                    \n");
    printf("************************************************************\n");
}

void searchbyCat(char cat[])
{
    system("@cls||clear");
    for (int i = 0; i < tickets_count; i++)
    {
        if (strcmp(tickets[i].categorie, cat) == 0)
        {
            printf("Identifiant: %d\n", tickets[i].id);
            printf("Motif: %s\n", tickets[i].motif);
            printf("Description: %s\n", tickets[i].description);
            printf("Categorie: %s\n", tickets[i].categorie);
            printf("Status: ");
            switch (tickets[i].status)
            {
            case 0:
                printf("en cours\n");
                break;
            case 1:
                printf("resolved\n");
                break;
            case 2:
                printf("fermee\n");
                break;
            }
            printf("Owner ID: %d\n", tickets[i].owner_id);
            printf("Date: %s\n", ctime(&tickets[i].date));
        }
    }
}

void searchbyDate(char date[])
{
    system("@cls||clear");
    printf("\n************************************************************\n");
    printf("               *** TICKET SEARCH RESULTS ***                \n");
    printf("************************************************************\n");

    for (int i = 0; i < tickets_count; i++)
    {
        char date2[9];
        strftime(date2, sizeof(date2), "%d/%m/%y", localtime(&tickets[i].date));

        if (strcmp(date2, date) == 0)
        {
            printf("\n************************************************************\n");
            printf("* %-15s : %-40d *\n", "IDENTIFIANT", tickets[i].id);
            printf("* %-15s : %-40s *\n", "MOTIF", tickets[i].motif);
            printf("* %-15s : %-40s *\n", "DESCRIPTION", tickets[i].description);
            printf("* %-15s : %-40s *\n", "CATEGORIE", tickets[i].categorie);

            printf("* %-15s : ", "STATUS");
            switch (tickets[i].status)
            {
            case 0:
                printf("%-40s *\n", "EN COURS");
                break;
            case 1:
                printf("%-40s *\n", "RESOLVED");
                break;
            case 2:
                printf("%-40s *\n", "FERMEE");
                break;
            }

            printf("* %-15s : %-40d *\n", "OWNER ID", tickets[i].owner_id);
            printf("* %-15s : %-40s *\n", "DATE", date2);
            printf("************************************************************\n");
        }
    }
    printf("\n************************************************************\n");
    printf("                    END OF SEARCH RESULTS                    \n");
    printf("************************************************************\n");
}

void searchbyCID(int target_id)
{
    system("@cls||clear");
    printf("\n************************************************************\n");
    printf("                *** TICKETS SEARCH BY OWNER ID ***           \n");
    printf("************************************************************\n");

    int found = 0; // Flag to check if any ticket is found

    for (int i = 0; i < tickets_count; i++)
    {
        if (tickets[i].owner_id == target_id)
        {
            found = 1; // Mark that a ticket is found
            printf("\n************************************************************\n");
            printf("* %-15s : %-40d *\n", "IDENTIFIANT", tickets[i].id);
            printf("* %-15s : %-40s *\n", "MOTIF", tickets[i].motif);
            printf("* %-15s : %-40s *\n", "DESCRIPTION", tickets[i].description);
            printf("* %-15s : %-40s *\n", "CATEGORIE", tickets[i].categorie);

            printf("* %-15s : ", "STATUS");
            switch (tickets[i].status)
            {
            case 0:
                printf("%-40s *\n", "EN COURS");
                break;
            case 1:
                printf("%-40s *\n", "RESOLVED");
                break;
            case 2:
                printf("%-40s *\n", "FERMEE");
                break;
            }

            printf("* %-15s : %-40d *\n", "OWNER ID", tickets[i].owner_id);
            printf("* %-15s : %-40s *\n", "DATE", ctime(&tickets[i].date));
            printf("************************************************************\n");
        }
    }

    if (!found)
    {
        printf("\nNo tickets found for OWNER ID: %d\n", target_id);
    }

    printf("\n************************************************************\n");
    printf("                    END OF SEARCH RESULTS                    \n");
    printf("************************************************************\n");
}

void process()
{
    system("@cls||clear");
    int target_id;

    printf("\n************************************************************\n");
    printf("                     *** PROCESS MENU ***                   \n");
    printf("************************************************************\n");

    printf("Entrer l'ID de la reclamation: ");
    scanf("%d", &target_id);
    getchar();

    if (target_id < 1 || target_id > tickets_count)
    {
        printf("\n*** Identifiant Invalide ***\n");
        return;
    }

    target_id -= 1;
    int stat;
    printf("Entrer le Status: 1 pour en cours; 2 pour Resolu; 3 pour Fermee: ");
    scanf("%d", &stat);
    getchar();

    tickets[target_id].status = stat;
    time_t t = time(NULL);
    tickets[target_id].timetoberesolved = difftime(t, tickets[target_id].date);

    printf("\n************************************************************\n");
    printf("       Reclamation Id: %d a ete modifie avec succes.       \n", target_id + 1);
    printf("************************************************************\n");
}
/* Process Tickets As a Mod */
void process_tickets()
{
    system("@cls||clear");
    int choix;

    do
    {
        printf("\n************************************************************\n");
        printf("                     *** TICKETS MENU ***                  \n");
        printf("************************************************************\n");
        printf("[1] Modifier une Reclamation\n");
        printf("[2] Traitement des Reclamations\n");
        printf("[3] Retour\n");
        printf("************************************************************\n");
        printf("Entrer Votre choix: ");
        scanf("%d", &choix);
        getchar();

        switch (choix)
        {
        case 1:
            manage_tickets_mod();
            break;
        case 2:
            process(); // Process Tickets as Mode
            break;
        case 3:
            printf("\n*** Retour au menu principal ***\n");
            break;
        default:
            printf("\n*** Choix Incorrect! ***\n");
            break;
        }
    } while (choix != 3);
}

int prior(char desc[])
{
    char *occurrence;
    char *occ;
    char urgent[][7] = {"urgent", "billing", "panne", "urgence"};
    char moyenne[][13] = {"tros", "wifi", "possible"};
    int ur = 0, moy = 0;

    for (int i = 0; i < 2; i++)
    {
        occ = strstr(desc, moyenne[i]);
        while (occ != NULL)
        {
            moy++;
            occ = strstr(occ + strlen(moyenne[i]), moyenne[i]);
        }
    }
    for (int i = 0; i < 3; i++)
    {
        occurrence = strstr(desc, urgent[i]);
        while (occurrence != NULL)
        {
            ur++;
            occurrence = strstr(occurrence + strlen(urgent[i]), urgent[i]);
        }
    }
    if (ur != 0 && ur > moy)
    {
        return 2;
    }
    else if (moy != 0 && moy > ur)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void rapport()
{
    system("@cls||clear");
    float resolved = 0, tot = 0;
    int res = 0, tot2 = 0;

    printf("\n************************************************************\n");
    printf("                     *** RAPPORT ***                        \n");
    printf("************************************************************\n");

    printf("Le total reclamation cree est: %d\n", tickets_count);

    for (int i = 0; i < tickets_count; i++)
    {
        if (tickets[i].status == 2)
        {
            resolved++;
            res++;
            tot += tickets[i].timetoberesolved;
            tot2 += tickets[i].timetoberesolved;
        }
    }

    if (resolved == 0)
    {
        printf("*** No ticket got resolved ***\n");
        return;
    }

    printf("Le total reclamation resolue est: %d\n", res);
    printf("Le Resolution Rate est: %.2f%%\n", ((float)(resolved) / (float)(tickets_count)) * 100);
    printf("Le temps moyenne pour traiter les reclamations est: %d\n", tot2 / res);

    char infile[1000] = "===========Le journal pour Ce jour===========\n";
    strcat(infile, "\n\nLes Reclamations de ce jours sont:\n");

    for (int i = 0; i < tickets_count; i++)
    {
        char rec[300];
        sprintf(rec, "Identifiant: %d\nMotif: %s\nDescription: %s\nCategorie: %s\nStatus: %d\nDate: %s\nOwner ID: %d\nPriority: %d\n",
                tickets[i].id, tickets[i].motif, tickets[i].description,
                tickets[i].categorie, tickets[i].status, ctime(&tickets[i].date),
                tickets[i].owner_id, tickets[i].priority);
        strcat(infile, rec);
    }

    strcat(infile, "\n\nLe nombre des reclamations resolue est: ");

    char buff[8];
    sprintf(buff, "%d", resolved);
    strcat(infile, buff);

    strcat(infile, "\n\nLe nombre totals des reclamations est: ");

    sprintf(buff, "%d", tickets_count);
    strcat(infile, buff);

    strcat(infile, "\n=======================================");

    FILE *fptr;
    fptr = fopen("rapport_de_jour.txt", "w");

    if (fptr != NULL)
    {
        fprintf(fptr, infile);
        fclose(fptr);
        printf("*** Rapport genere avec succes! ***\n");
    }
    else
    {
        printf("*** Erreur lors de la creation du rapport! ***\n");
    }
}

int main()
{
    menu();
    return 0;
}