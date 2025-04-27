#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

class Product {
private:
    string id;
    string name;
    int price;

public:
    Product(string id, string name, int price) : id(id), name(name), price(price) {}
    string getId() { return id; }
    string getName() { return name; }
    int getPrice() { return price; }
};


struct CartNode {
    Product* product;
    int quantity;
    CartNode* next;
};


class PaymentStrategy {
public:
    virtual string pay(int amount) = 0;
};

class CashPayment : public PaymentStrategy {
public:
    string pay(int amount) override {
        cout << "Paid " << amount << " using Cash.\n";
        return "Cash";
    }
};

class CreditCardPayment : public PaymentStrategy {
public:
    string pay(int amount) override {
        cout << "Paid " << amount << " using Credit/Debit Card.\n";
        return "Credit/Debit Card";
    }
};

class GCashPayment : public PaymentStrategy {
public:
    string pay(int amount) override {
        cout << "Paid " << amount << " using GCash.\n";
        return "GCash";
    }
};


class ShoppingCart {
private:
    CartNode* head;
    static ShoppingCart* instance;
    int orderIdCounter;

    ShoppingCart() {
        head = nullptr;
        orderIdCounter = loadLastOrderId() + 1;
    }

    int loadLastOrderId() {
        ifstream file("orders.txt");
        if (!file) return 0;
        string line;
        int lastId = 0;
        while (getline(file, line)) {
            if (line.find("Order ID:") != string::npos) {
                int id = stoi(line.substr(9));
                if (id > lastId) lastId = id;
            }
        }
        file.close();
        return lastId;
    }

public:
    static ShoppingCart* getInstance() {
        if (!instance)
            instance = new ShoppingCart();
        return instance;
    }

    void addProduct(Product* product, int quantity) {
        CartNode* newNode = new CartNode{product, quantity, head};
        head = newNode;
        cout << "Product added successfully!\n";
    }

    void viewCart() {
        if (!head) {
            cout << "Cart is empty.\n";
            return;
        }

        CartNode* temp = head;
        cout << "\n--- Shopping Cart ---\n";
        cout << "Product ID\tName\tPrice\tQuantity\n";
        while (temp) {
            cout << temp->product->getId() << "\t\t" << temp->product->getName() << "\t"
                 << temp->product->getPrice() << "\t" << temp->quantity << "\n";
            temp = temp->next;
        }
    }

    void checkout(PaymentStrategy* paymentMethod) {
        if (!head) {
            cout << "Cart is empty!\n";
            return;
        }

        int total = 0;
        CartNode* temp = head;
        while (temp) {
            total += temp->product->getPrice() * temp->quantity;
            temp = temp->next;
        }

        string paymentType = paymentMethod->pay(total);

        saveOrder(orderIdCounter, paymentType, total);
        
        
        logPayment(orderIdCounter, paymentType);

        orderIdCounter++;

        clearCart(); 

        cout << "You have successfully checked out the products!\n";
        cout << "Order saved successfully!\n";
    }

    void viewOrders() {
        ifstream file("orders.txt");
        if (!file.is_open()) {
            cout << "No orders found.\n";
            return;
        }

        string line;
        bool foundOrder = false;
        cout << "\n--- Orders ---\n";
        while (getline(file, line)) {
            foundOrder = true;
            cout << line << endl;
        }
        if (!foundOrder) {
            cout << "No orders found.\n";
        }
        file.close();
    }

private:
    void clearCart() {
        while (head) {
            CartNode* temp = head;
            head = head->next;
            delete temp;
        }
    }

    void saveOrder(int orderId, const string& paymentType, int total) {
        ofstream file("orders.txt", ios::app);
        if (!file.is_open()) {
            cout << "Error: Could not open orders.txt to save order.\n";
            return;
        }

        file << "Order ID: " << orderId << "\n";
        file << "Total Amount: " << total << "\n";
        file << "Payment Method: " << paymentType << "\n";
        file << "Order Details:\n";

        CartNode* temp = head;
        while (temp) {
            file << temp->product->getId() << " "
                 << temp->product->getName() << " "
                 << temp->product->getPrice() << " "
                 << temp->quantity << "\n";
            temp = temp->next;
        }
        file << "---------------------------\n";
        file.close();
    }

    void logPayment(int orderId, const string& paymentType) {
        ofstream logfile("logs.txt", ios::app);
        if (!logfile.is_open()) {
            cout << "Error: Could not open logs.txt to save log.\n";
            return;
        }
        logfile << "[LOG] Order ID: " << orderId << " successfully checked out using " << paymentType << ".\n";
        logfile.close();
    }
};

ShoppingCart* ShoppingCart::instance = nullptr;


void displayProducts(Product* products[], int size) {
    cout << "\n--- Products ---\n";
    cout << "Product ID\tName\tPrice\n";
    for (int i = 0; i < size; ++i) {
        cout << products[i]->getId() << "\t\t" << products[i]->getName() << "\t" << products[i]->getPrice() << endl;
    }
}

void addToCart(Product* products[], int size) {
    ShoppingCart* cart = ShoppingCart::getInstance();
    string id;
    char choice;

    do {
        cout << "Enter Product ID to add to cart: ";
        cin >> id;
        bool found = false;

        for (int i = 0; i < size; ++i) {
            if (products[i]->getId() == id) {
                int quantity;
                cout << "Enter quantity: ";
                cin >> quantity;
                if (cin.fail() || quantity <= 0) {
                    cout << "Invalid quantity! Try again.\n";
                    cin.clear();
                    cin.ignore(10000, '\n');
                    continue;
                }
                cart->addProduct(products[i], quantity);
                found = true;
                break;
            }
        }

        if (!found) {
            cout << "Invalid Product ID.\n";
        }

        cout << "Do you want to add another product? (y/n): ";
        cin >> choice;
    } while (choice == 'y' || choice == 'Y');
}

int main() {
    
    Product* products[5] = {
        new Product("ABC", "Paper", 20),
        new Product("CDE", "Pencil", 10),
        new Product("QWE", "Notebook", 50),
        new Product("TRE", "Eraser", 5),
        new Product("POI", "Pen", 15)
    };

    ShoppingCart* cart = ShoppingCart::getInstance();
    int menuChoice;

    while (true) {
        cout << "\n==== Online Store Menu ====\n";
        cout << "1. View Products\n";
        cout << "2. View Shopping Cart\n";
        cout << "3. View Orders\n";
        cout << "4. Exit\n";
        cout << "Enter choice: ";
        cin >> menuChoice;

        try {
            if (cin.fail()) {
                throw runtime_error("Invalid input! Please enter a number.");
            }

            if (menuChoice == 1) {
                displayProducts(products, 5);
                addToCart(products, 5);
            }
            else if (menuChoice == 2) {
                cart->viewCart();
                char checkoutChoice;
                cout << "Do you want to checkout the products? (Y/N): ";
                cin >> checkoutChoice;

                if (checkoutChoice == 'y' || checkoutChoice == 'Y') {
                    int paymentChoice;
                    cout << "Select payment method:\n";
                    cout << "1. Cash\n2. Credit/Debit Card\n3. GCash\n";
                    cout << "Enter choice: ";
                    cin >> paymentChoice;

                    PaymentStrategy* paymentMethod = nullptr;
                    switch (paymentChoice) {
                        case 1: paymentMethod = new CashPayment(); break;
                        case 2: paymentMethod = new CreditCardPayment(); break;
                        case 3: paymentMethod = new GCashPayment(); break;
                        default: throw runtime_error("Invalid payment method selected!");
                    }

                    cart->checkout(paymentMethod);
                    delete paymentMethod;
                }
            }
            else if (menuChoice == 3) {
                cart->viewOrders();
            }
            else if (menuChoice == 4) {
                cout << "Exiting...\n";
                break;
            }
            else {
                throw runtime_error("Invalid menu option!");
            }
        }
        catch (exception& e) {
            cout << "Error: " << e.what() << endl;
            cin.clear();
            cin.ignore(10000, '\n');
        }
    }

    for (int i = 0; i < 5; ++i) {
        delete products[i];
    }

    return 0;
}