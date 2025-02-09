#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <mutex>
#include <unordered_map>
#include <shared_mutex>

// Order class to represent an order with lot size and price
struct Order {
    int lotSize;  // Number of units in the order
    int price;    // Price per unit

    // Default constructor with default values for lotSize and price
    Order() : lotSize(10), price(2) {}

    // Parameterized constructor to initialize lotSize and price
    Order(int lotSize, int price) : lotSize(lotSize), price(price) {}

    // Destructor (not strictly needed, but defined for clarity)
    ~Order() {
        std::cout << "Order with lotSize " << lotSize << " and price " << price << " is being destroyed.\n";
    }
};

// ConcurrentHashMap class to manage orders with thread safety
class ConcurrentHashMap {
public:
    // Constructor
    ConcurrentHashMap() {
        std::cout << "ConcurrentHashMap created.\n";
    }

    // Destructor
    ~ConcurrentHashMap() {
        std::cout << "ConcurrentHashMap is being destroyed.\n";
    }

    // Insert a new order or update an existing order in the map
    void insert(const std::string& symbol, const Order& order) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        auto& orders = map_[symbol];
        // Check if the order with the same price already exists
        auto it = orders.find(order.price);
        if (it != orders.end()) {
            // If exists, update the order
            it->second = order;
            std::cout << "Updated order for " << symbol << " with price " << order.price << ".\n";
        } else {
            // Otherwise, add a new order
            orders[order.price] = order;
            std::cout << "Inserted order for " << symbol << " with price " << order.price << ".\n";
        }
    }

    // Remove all orders for a given symbol
    void remove(const std::string& symbol) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        auto it = map_.find(symbol);
        if (it != map_.end()) {
            map_.erase(it);
            std::cout << "Removed orders for " << symbol << ".\n";
        } else {
            std::cout << "No orders found for symbol: " << symbol << " to remove.\n";
        }
    }

    // Display all the orders in the map
    void display() const {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        for (const auto& pair : map_) {
            std::cout << pair.first << ": ";  // Print the symbol (key)
            for (const auto& order : pair.second) {
                std::cout << "{lotSize: " << order.second.lotSize << ", price: " << order.second.price << "} ";
            }
            std::cout << std::endl;
        }
    }

private:
    // Use unordered_map for efficient symbol lookups
    std::unordered_map<std::string, std::unordered_map<int, Order>> map_;  // Nested map for symbol -> (price -> order)
    mutable std::shared_mutex mutex_;  // Mutex to ensure thread safety during map operations
};

int main() {
    // Create an instance of ConcurrentHashMap
    ConcurrentHashMap concurrentMap;

    // List of symbols representing various companies or assets
    std::vector<std::string> symbols = {
        "NESTLEIND", "HDFCBANK", "RELIANCE", "TCS", "INFY",
        "SBIN", "ICICIBANK", "LT", "BAJFINANCE", "HINDUNILVR"
    };

    // Insert initial orders for each symbol with default lotSize and price
    for (const auto& symbol : symbols) {
        concurrentMap.insert(symbol, Order(10, 2));  // Insert default order with lotSize 10 and price 2
    }

    // Insert additional orders with specific lotSize and price for some symbols
    concurrentMap.insert("NESTLEIND", Order(20, 3));  // New order with price 3 for NESTLEIND
    concurrentMap.insert("HDFCBANK", Order(15, 4));  // New order with price 4 for HDFCBANK

    // Display the map content after inserting the orders
    std::cout << "Initial Map State:\n";
    concurrentMap.display();

    // Remove the "NESTLEIND" entry from the map (all orders for that symbol)
    concurrentMap.remove("NESTLEIND");

    // Display the map content after removal of NESTLEIND
    std::cout << "\nMap State after Removal:\n";
    concurrentMap.display();

    // Try to remove a non-existent symbol to test error handling
    concurrentMap.remove("NONEXISTENT");

    return 0;  // End the program
}
