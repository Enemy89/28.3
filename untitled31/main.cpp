#include <iostream>
#include <thread>
#include <string>
#include <mutex>
#include <ctime>
#include <vector>

class Kitchen {
    std::mutex kitchenMutex;
    std::vector<std::string> dishes = { "salad", "sushi", "pizza", "steak", "soup" };
    int ordersToSend = 0;

public:
    void processOrder(const std::string& order) {
        std::this_thread::sleep_for(std::chrono::seconds(rand() % 11 + 5));

        std::lock_guard<std::mutex> lock(kitchenMutex);
        ordersToSend++;
        std::cout << "Order is ready: " << order << std::endl;
    }

    void addOrder(const std::string& order) {
        std::lock_guard<std::mutex> lock(kitchenMutex);
        std::thread cookThread(&Kitchen::processOrder, this, order);
        cookThread.detach();
    }

    std::string getRandomDish() {
        return dishes[rand() % dishes.size()];
    }

    int getOrdersToSend() {
        std::lock_guard<std::mutex> lock(kitchenMutex);
        return ordersToSend;
    }

    void resetOrdersToSend() {
        std::lock_guard<std::mutex> lock(kitchenMutex);
        ordersToSend = 0;
    }
};

class Courier {
    int completeOrders = 0;
    int totalOrders = 10;
public:
    void pickupOrders(Kitchen &kitchen) {
        for (int i = 1; i <= 10; ++i) {
            std::this_thread::sleep_for(std::chrono::seconds(30));
            int ordersToSend = kitchen.getOrdersToSend();
            if (ordersToSend > 0) {
                std::cout << "Courier picked up " << ordersToSend << " orders." << std::endl;
                completeOrders+=ordersToSend;
                kitchen.resetOrdersToSend();
            } else {
                std::cout << "Courier picked up 0 orders." << std::endl;
            }
            if (completeOrders >= totalOrders) {
                std::cout << "All orders delivered successfully!" << std::endl;
                break;
            }
        }
    }
};

int main() {
    std::srand(std::time(nullptr));

    Kitchen kitchen;
    Courier courier;
    
    std::thread courierThread(&Courier::pickupOrders, &courier, std::ref(kitchen));

    // Задержка перед поступлением первого заказа
    int initialDelay = rand() % 6 + 5;
    std::this_thread::sleep_for(std::chrono::seconds(initialDelay));

    int totalOrders = 10;

    for (int i = 1; i <= totalOrders; ++i) {
        std::string order = "Order #" + std::to_string(i) + " (" + kitchen.getRandomDish() + ")";
        kitchen.addOrder(order);
        std::cout << "New order received: " << order << std::endl;

        int sleepTime = rand() % 6 + 5;
        std::this_thread::sleep_for(std::chrono::seconds(sleepTime));
    }

    if (courierThread.joinable()) {
        courierThread.join();
    }
}
