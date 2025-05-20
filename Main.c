#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#define MAX_NAME 50
#define MAX_PASS 20
#define MAX_CARD 16
#define INF 9999
#define ADMIN_REG_KEY "admin123"

// File Handling -------------------------------------------------------------------------------------------------------------------------------------------

#define USERS_FILE "data/customers.txt"
#define MENU_FILE "data/menu.txt"
#define ORDERS_FILE "data/orders.txt"
#define PAYMENT_FILE "data/payments.txt"
#define HISTORY_FILE "data/history.txt"

// Functinality ---------------------------------------------------------------------------------------------------------------------------------------------

void registerUser();
int loginUser(int *isAdmin);
void securePasswordInput(char *password);
void loadMenu();
void saveMenu();
void addMenuItem();
void removeMenuItem();
void updateMenuItem();
void displayMenu();
float getPriceFromMenu(int foodId);
const char* getFoodItemNameFromMenu(int foodId);
void loadOrders();
void saveOrders();
void placeOrder();
void processOrder();
void displayOrders();
void processPayment(int orderId, char *customerName, float amount);
void displayPayments();
void loadOrderHistory();
void saveOrderHistory();
void addOrderToHistory(int orderId, char *customer, char *food, float price);
void displayOrderHistory();
void showAboutInfo();

// Authantication ------------------------------------------------------------------------------------------------------------------------------------------

typedef struct {
    char username[MAX_NAME];
    char password[MAX_PASS];
    int isAdmin;
} User;

void securePasswordInput(char *password) {
    char ch;
    int i = 0;
    while (1) {
        ch = getch();
        if (ch == 13) { 
            password[i] = '\0';
            break;
        } else if (ch == 8 && i > 0) {
            i--;
            printf("\b \b");
        } else if (i < MAX_PASS - 1) {
            password[i++] = ch;
            printf("*");
        }
    }
}

void registerUser() {
    FILE *file = fopen(USERS_FILE, "a");
    if (!file) {
        printf("Error opening file!\n");
        return;
    }

    User newUser;
    printf("\nEnter username: ");
    scanf("%s", newUser.username);

    printf("Enter password: ");
    securePasswordInput(newUser.password);

    int isAdminRequest = 0;
    printf("\nAre you an Admin? (1 = Yes, 0 = No): ");
    scanf("%d", &isAdminRequest);

    if (isAdminRequest == 1) {
        char adminKey[50];
        printf("Enter Admin Registration Key: ");
        scanf("%s", adminKey);

        if (strcmp(adminKey, ADMIN_REG_KEY) != 0) {
            printf("Invalid Admin Key!\n");
            return registerUser();
        } else {
            newUser.isAdmin = 1;
        }
    } else {
        newUser.isAdmin = 0;
    }

    fprintf(file, "%s %s %d\n", newUser.username, newUser.password, newUser.isAdmin);
    fclose(file);
    printf("\nRegistered successfully!\n");
}

int loginUser(int *isAdmin) {
    FILE *file = fopen(USERS_FILE, "r");
    if (!file) {
        printf("No user database found! Please register first.\n");
        return 0;
    }
    User tempUser, inputUser;
    printf("\nEnter username: ");
    scanf("%s", inputUser.username);
    printf("Enter password: ");
    securePasswordInput(inputUser.password);
    while (fscanf(file, "%s %s %d", tempUser.username, tempUser.password, &tempUser.isAdmin) != EOF) {
        if (strcmp(tempUser.username, inputUser.username) == 0 &&
            strcmp(tempUser.password, inputUser.password) == 0) {
            *isAdmin = tempUser.isAdmin;
            fclose(file);
            return 1;
        }
    }
    fclose(file);
    printf("\nInvalid username or password!\n");
    return 0;
}

// Menu Management -------------------------------------------------------------------------------------------------------------------------------------------

typedef struct MenuItem {
    int id;
    char name[MAX_NAME];
    float price;
    int stock;
    struct MenuItem *next;
} MenuItem;

MenuItem *menuHead = NULL;

void loadMenu() {
    FILE *file = fopen(MENU_FILE, "r");
    if (!file) {
        printf("Menu file not found. Starting with an empty menu.\n");
        return;
    }
    while (!feof(file)) {
        MenuItem *newItem = (MenuItem *)malloc(sizeof(MenuItem));
        if (fscanf(file, "%d %s %f %d", &newItem->id, newItem->name, &newItem->price, &newItem->stock) == 4) {
            newItem->next = menuHead;
            menuHead = newItem;
        } else {
            free(newItem);
        }
    }
    fclose(file);
}

void saveMenu() {
    FILE *file = fopen(MENU_FILE, "w");
    if (!file) {
        printf("Error saving menu!\n");
        return;
    }
    MenuItem *temp = menuHead;
    while (temp) {
        fprintf(file, "%d %s %.2f %d\n", temp->id, temp->name, temp->price, temp->stock);
        temp = temp->next;
    }
    fclose(file);
}

void addMenuItem() {
    MenuItem *newItem = (MenuItem *)malloc(sizeof(MenuItem));

    printf("Enter Item ID: ");
    scanf("%d", &newItem->id);
    printf("Enter Item Name: ");
    scanf("%s", newItem->name);
    printf("Enter Price: ");
    scanf("%f", &newItem->price);
    printf("Enter Stock Quantity: ");
    scanf("%d", &newItem->stock);

    newItem->next = menuHead;
    menuHead = newItem;

    saveMenu();
    printf("Item added successfully!\n");
}

void removeMenuItem() {
    int id;
    printf("Enter Item ID to remove: ");
    scanf("%d", &id);

    MenuItem *temp = menuHead, *prev = NULL;

    while (temp && temp->id != id) {
        prev = temp;
        temp = temp->next;
    }
    if (!temp) {
        printf("Item not found!\n");
        return;
    }
    if (!prev)
        menuHead = temp->next;
    else
        prev->next = temp->next;

    free(temp);
    saveMenu();
    printf("Item removed successfully!\n");
}

void updateMenuItem() {
    int id;
    printf("Enter Item ID to update: ");
    scanf("%d", &id);

    MenuItem *temp = menuHead;
    while (temp && temp->id != id)
        temp = temp->next;

    if (!temp) {
        printf("Item not found!\n");
        return;
    }

    printf("Enter new price: ");
    scanf("%f", &temp->price);

    printf("Enter new Stock: ");
    scanf("%d", &temp->stock);

    saveMenu();
    printf("Item updated successfully!\n");
}

void displayMenu() {
    MenuItem *temp = menuHead;
    int count = 0;

    printf("\n========================== MENU ==========================\n");
    printf("%-5s %-30s %-10s %-5s\n", "ID", "Name", "Price", "Stock");
    printf("---------------------------------------------------------------\n");

    if (!temp) {
        printf("No menu items available!\n");
    } else {
        while (temp) {
            printf("%-5d %-30s %-10.2f %-5d\n", temp->id, temp->name, temp->price, temp->stock);
            temp = temp->next;
            count++;
        }
        printf("---------------------------------------------------------------\n");
        printf("Total Menu Items: %d\n", count);
    }
    printf("===============================================================\n");
}


float getPriceFromMenu(int foodId) {
    MenuItem *temp = menuHead;

    while (temp) {
        if (temp->id == foodId) {
            return temp->price;
        }
        temp = temp->next;
    }
    return -1;
}

const char* getFoodItemNameFromMenu(int foodId) {
    MenuItem *temp = menuHead;
    while (temp != NULL) {
        if (temp->id == foodId) {
            return temp->name;
        }
        temp = temp->next;
    }
    return NULL;
}

// Order Management -----------------------------------------------------------------------------------------------------------------------------------------

typedef struct Order {
    int orderId;
    char customerName[MAX_NAME];
    char foodItem[MAX_NAME];
    float price;
    int priority;
    struct Order *next;
} Order;

Order *front = NULL, *rear = NULL;

void enqueueOrder(Order *newOrder) {
    newOrder->next = NULL;
    if (!front) {
        front = rear = newOrder;
    } else if (newOrder->priority) {
        newOrder->next = front;
        front = newOrder;
    } else {
        rear->next = newOrder;
        rear = newOrder;
    }
}

void loadOrders() {
    FILE *file = fopen(ORDERS_FILE, "r");
    if (!file) {
        perror("Failed to open orders file");
        return;
    }

    int orderId, priority;
    float price;
    char customerName[MAX_NAME], foodItem[MAX_NAME];

    while (fscanf(file, "%d %s %s %f %d", &orderId, customerName, foodItem, &price, &priority) == 5) {
        Order *newOrder = (Order *)malloc(sizeof(Order));
        if (!newOrder) {
            printf("Memory allocation failed.\n");
            break;
        }

        newOrder->orderId = orderId;
        strcpy(newOrder->customerName, customerName);
        strcpy(newOrder->foodItem, foodItem);
        newOrder->price = price;
        newOrder->priority = priority;
        enqueueOrder(newOrder);
    }

    fclose(file);
}

void saveOrders() {
    FILE *file = fopen(ORDERS_FILE, "w");
    if (!file) {
        printf("Error saving orders!\n");
        return;
    }

    Order *temp = front;
    while (temp) {
        fprintf(file, "%d %s %s %.2f %d\n",
                temp->orderId, temp->customerName, temp->foodItem, temp->price, temp->priority);
        temp = temp->next;
    }

    fclose(file);
}

void placeOrder() {
    Order *newOrder = (Order *)malloc(sizeof(Order));
    if (!newOrder) {
        printf("Memory allocation failed!\n");
        return;
    }

    printf("Enter Customer ID: ");
    if (scanf("%d", &newOrder->orderId) != 1) {
        printf("Invalid input!\n");
        while (getchar() != '\n');
        free(newOrder);
        return;
    }

    printf("Enter Customer Name: ");
    scanf("%s", newOrder->customerName);

    printf("Enter Food Item ID: ");
    int foodId;
    if (scanf("%d", &foodId) != 1) {
        printf("Invalid input!\n");
        while (getchar() != '\n');
        free(newOrder);
        return;
    }

    newOrder->price = getPriceFromMenu(foodId);
    if (newOrder->price == -1) {
        printf("Error: This item ID is not available in the menu!\n");
        free(newOrder);
        return;
    }

    const char *itemName = getFoodItemNameFromMenu(foodId);
    if (itemName == NULL) {
        printf("Error: Food item not found in the menu!\n");
        free(newOrder);
        return;
    }
    strcpy(newOrder->foodItem, itemName);

    printf("Is this an urgent order? (1 = Yes, 0 = No): ");
    if (scanf("%d", &newOrder->priority) != 1) {
        printf("Invalid input!\n");
        while (getchar() != '\n');
        free(newOrder);
        return;
    }

    enqueueOrder(newOrder);
    processPayment(newOrder->orderId, newOrder->customerName, newOrder->price);
    saveOrders();
    printf("Order placed successfully! Item ID: %d, Price: %.2f\n", foodId, newOrder->price);
}

void processOrder() {
    if (!front) {
        printf("No orders to process!\n");
        return;
    }

    Order *temp = front;
    printf("Processing Order #%d for %s: %s - %.2f\n",
           temp->orderId, temp->customerName, temp->foodItem, temp->price);

    addOrderToHistory(temp->orderId, temp->customerName, temp->foodItem, temp->price);

    front = front->next;
    if (!front)
        rear = NULL;

    free(temp);
    saveOrders();
}


void displayOrders() {
    if (!front) {
        printf("\n================= PENDING ORDERS =================\n");
        printf("No pending orders!\n");
        printf("==================================================\n");
        return;
    }

    printf("\n================= PENDING ORDERS =================\n");
    printf("%-5s %-20s %-25s %-10s %-10s\n", "ID", "Customer", "Food Item", "Price", "Priority");
    printf("--------------------------------------------------------------------------\n");

    Order *temp = front;
    int count = 0;
    while (temp) {
        printf("%-5d %-20s %-25s %-10.2f %-10s\n",
               temp->orderId, temp->customerName, temp->foodItem, temp->price,
               temp->priority ? "Urgent" : "Normal");
        temp = temp->next;
        count++;
    }
    printf("--------------------------------------------------------------------------\n");
    printf("Total Pending Orders: %d\n", count);
    printf("==================================================\n");
}


// Payment-----------------------------------------------------------------------------------------------------------------------------

typedef struct Payment {
    int orderId;
    char customerName[MAX_NAME];
    float amount;
    char method[10];
} Payment;

void processPayment(int orderId, char *customerName, float amount) {
    FILE *file = fopen(PAYMENT_FILE, "a");
    if (!file) {
        printf("Error processing payment!\n");
        return;
    }

    int choice;
    char method[10], cardNumber[MAX_CARD + 1];

    printf("Choose Payment Method:\n1. Cash\n2. Card\nEnter choice: ");
    scanf("%d", &choice);

    if (choice == 1) {
        strcpy(method, "Cash");
    } else if (choice == 2) {
        strcpy(method, "Card");
        printf("Enter 16-digit Card Number: ");
        scanf("%s", cardNumber);

        if (strlen(cardNumber) != 16) {
            printf("Invalid card number! Payment failed.\n");
            fclose(file);
            return;
        }
    } else {
        printf("Invalid payment method!\n");
        fclose(file);
        return;
    }

    fprintf(file, "%d %s %.2f %s\n", orderId, customerName, amount, method);
    fclose(file);
    printf("Payment Successful! Order #%d paid via %s.\n", orderId, method);
}

void displayPayments() {
    FILE *file = fopen(PAYMENT_FILE, "r");
    if (!file) {
        printf("\n================= PAYMENT RECORDS =================\n");
        printf("No payment records found!\n");
        printf("===================================================\n");
        return;
    }

    Payment temp;
    int recordFound = 0;
    float totalAmount = 0.0;

    printf("\n================= PAYMENT RECORDS =================\n");
    printf("%-10s %-20s %-10s %-10s\n", "Order ID", "Customer", "Amount", "Method");
    printf("-------------------------------------------------------------\n");

    while (fscanf(file, "%d %s %f %s", &temp.orderId, temp.customerName, &temp.amount, temp.method) == 4) {
        printf("%-10d %-20s %-10.2f %-10s\n", temp.orderId, temp.customerName, temp.amount, temp.method);
        totalAmount += temp.amount;
        recordFound = 1;
    }

    if (!recordFound) {
        printf("No payment records found in the file!\n");
    } else {
        printf("-------------------------------------------------------------\n");
        printf("Total Payments: %.2f\n", totalAmount);
    }
    printf("===================================================\n");

    fclose(file);
}

// History --------------------------------------------------------------------------------------------------------------------------------------------------

typedef struct OrderHistory {
    int orderId;
    char customerName[MAX_NAME];
    char foodItem[MAX_NAME];
    float price;
    struct OrderHistory *next;
} OrderHistory;

OrderHistory *historyTop = NULL;

void loadOrderHistory() {
    FILE *file = fopen(HISTORY_FILE, "r");
    if (!file) return;

    while (!feof(file)) {
        OrderHistory *newHistory = (OrderHistory *)malloc(sizeof(OrderHistory));
        if (fscanf(file, "%d %s %s %f", &newHistory->orderId, newHistory->customerName, 
                   newHistory->foodItem, &newHistory->price) == 4) {
            newHistory->next = historyTop;
            historyTop = newHistory;
        } else {
            free(newHistory);
        }
    }
    fclose(file);
}

void saveOrderHistory() {
    FILE *file = fopen(HISTORY_FILE, "w");
    if (!file) {
        printf("Error saving order history!\n");
        return;
    }

    OrderHistory *temp = historyTop;
    while (temp) {
        fprintf(file, "%d %s %s %.2f\n", temp->orderId, temp->customerName, temp->foodItem, temp->price);
        temp = temp->next;
    }
    fclose(file);
}

void addOrderToHistory(int orderId, char *customer, char *food, float price) {
    OrderHistory *newHistory = (OrderHistory *)malloc(sizeof(OrderHistory));
    newHistory->orderId = orderId;
    strcpy(newHistory->customerName, customer);
    strcpy(newHistory->foodItem, food);
    newHistory->price = price;
    newHistory->next = historyTop;
    historyTop = newHistory;

    saveOrderHistory();
}

void displayOrderHistory() {
    if (!historyTop) {
        printf("\n================= ORDER HISTORY =================\n");
        printf("No order history available!\n");
        printf("================================================\n");
        return;
    }

    OrderHistory *temp = historyTop;
    double total = 0.0;

    printf("\n================= ORDER HISTORY =================\n");
    printf("%-5s %-20s %-25s %-10s\n", "ID", "Customer", "Food Item", "Price");
    printf("---------------------------------------------------------------\n");

    while (temp) {
        printf("%-5d %-20s %-25s %-10.2f\n", temp->orderId, temp->customerName, temp->foodItem, temp->price);
        total += temp->price;
        temp = temp->next;
    }
    printf("---------------------------------------------------------------\n");
    printf("Total Revenue: %.2f\n", total);
    printf("================================================\n");
}


// About - Us-----------------------------------------------------------------------------------------------------------------------------

void showAboutInfo() {
    printf("\n--- ABOUT BURPIT ---\n");
    printf("App Version: 1.0.0\n");
    printf("Credits: This app is developed by a group of four members as a part of college project under the guidance of Dr. Himani Maheshwari:-\n");
    printf("\nTeam member 1 (Team Lead):\nName: Sagar Negi\nStudent ID: 24711207\nEmail: sagarnegi13.0@gmail.com\n");
    printf("\nTeam member 2:\nName: Mohammad Kaif\nStudent ID: 24711363\nEmail: kaiiff2802@gmail.com\n");
    printf("\nTeam member 3:\nName: Ritik Uniyal\nStudent ID: 24711185 \nEmail: ritikuniyal9999@gmail.com\n");
    printf("\nTeam member 4:\nName: Rita Rathore\nStudent ID: 24712183\nEmail: Ritarathore97772@gmail.com\n");
    printf("\nFor further inquiries, feel free to contact us.\n");
}

// Burpit ---------------------------------------------------------------------------------------------------------------------------------------

void printLine() {
    printf("==========================================================================\n");
}

void printMainMenu() {
    printLine();
    printf("                          WELCOME TO BURP-IT\n");
    printLine();
    printf("  1. Register a New Account\n");
    printf("  2. Login to Your Account\n");
    printf("  3. Exit\n");
    printLine();
    printf("Please enter your choice: ");
}

void printCustomerMenu() {
    printLine();
    printf("                            CUSTOMER PANEL\n");
    printLine();
    printf("  1. View Menu\n");
    printf("  2. Place Order\n");
    printf("  3. About Us\n");
    printf("  4. Logout\n");
    printLine();
    printf("Please enter your choice: ");
}

void printAdminMenu() {
    printLine();
    printf("                              ADMIN PANEL\n");
    printLine();
    printf("  1. View Menu\n");
    printf("  2. Add Item\n");
    printf("  3. Remove Item\n");
    printf("  4. Update Item\n");
    printf("  5. View Orders\n");
    printf("  6. Process Orders\n");
    printf("  7. View Order History\n");
    printf("  8. View Payments\n");
    printf("  9. Logout\n");
    printLine();
    printf("Please enter your choice: ");
}

void runBurpit() {
    int running = 1;
    while (running) {
        int choice, isAdmin = 0, loggedIn = 0;

        printMainMenu();
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input! Please enter a number.\n");
            while (getchar() != '\n');
            continue;
        }

        switch (choice) {
            case 1:
                registerUser();
                break;
            case 2:
                loggedIn = loginUser(&isAdmin);
                if (loggedIn) {
                    printf("\nLogin successful! You are logged in as %s.\n", isAdmin ? "Admin" : "Customer");
                    if (!isAdmin) {
                        int customerPanel = 1;
                        while (customerPanel) {
                            int customerChoice;
                            printCustomerMenu();
                            if (scanf("%d", &customerChoice) != 1) {
                                printf("Invalid input! Please enter a number.\n");
                                while (getchar() != '\n');
                                continue;
                            }
                            switch (customerChoice) {
                                case 1: displayMenu(); break;
                                case 2: placeOrder(); break;
                                case 3: showAboutInfo(); break;
                                case 4:
                                    printf("Logging out from Customer Panel...\n");
                                    customerPanel = 0;
                                    break;
                                default:
                                    printf("Invalid choice! Please try again.\n");
                            }
                        }
                    } else {
                        int adminPanel = 1;
                        while (adminPanel) {
                            int adminChoice;
                            printAdminMenu();
                            if (scanf("%d", &adminChoice) != 1) {
                                printf("Invalid input! Please enter a number.\n");
                                while (getchar() != '\n');
                                continue;
                            }
                            switch (adminChoice) {
                                case 1: displayMenu(); break;
                                case 2: addMenuItem(); break;
                                case 3: removeMenuItem(); break;
                                case 4: updateMenuItem(); break;
                                case 5: displayOrders(); break;
                                case 6: processOrder(); break;
                                case 7: displayOrderHistory(); break;
                                case 8: displayPayments(); break;
                                case 9:
                                    printf("Logging out from Admin Panel...\n");
                                    adminPanel = 0;
                                    break;
                                default:
                                    printf("Invalid choice! Please try again.\n");
                            }
                        }
                    }
                } else {
                    printf("Login failed! Please try again.\n");
                }
                break;
            case 3:
                printf("Thank you for using BURP-IT! Goodbye.\n");
                running = 0;
                break;
            default:
                printf("Invalid choice! Please enter 1, 2, or 3.\n");
        }
    }
}

// Main ---------------------------------------------------------------------------------------------------------------------------------------

int main(){
    loadMenu();
    loadOrders();
    loadOrderHistory();
    runBurpit();
    return 0;
}