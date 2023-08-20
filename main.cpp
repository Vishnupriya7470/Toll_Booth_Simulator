#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <ctime>
#include <iomanip>
#include <stdexcept>
#include <limits>

const double INITIAL_TOLL_FARE = 10.0;
const double DEMAND_THRESHOLD = 0.1;
const double DEMAND_INCREASE_PERCENT = 0.05;

const std::string INPUT_FILE_PATH = "input.txt";
const std::string OUTPUT_FILE_PATH = "output.txt";

class Vehicle {
public:
    Vehicle(const std::string& number, const std::string& type, double fastTagBalance)
        : vehicleNumber(number), vehicleType(type), fastTagBalance(fastTagBalance) {}

    const std::string& getVehicleNumber() const {
        return vehicleNumber;
    }

    const std::string& getVehicleType() const {
        return vehicleType;
    }

    double getFastTagBalance() const {
        return fastTagBalance;
    }


    operator std::string() const {
        std::stringstream ss;
        ss << "Vehicle Number: " << vehicleNumber << ", Type: " << vehicleType;
        return ss.str();
    }

private:
    std::string vehicleNumber;
    std::string vehicleType;
    double fastTagBalance;
};

class TollBooth {
public:
    void processInputFile();
    void displayDetails() const;

private:
    std::map<std::string, int> vehicleCounts;
    double totalTollCollected = 0.0;
    int totalNumberOfVehicles = 0;

    friend double getTollFare(const std::string& vehicleType, const std::map<std::string, int>& vehicleCounts, double totalTollCollected);
    static double getTollFare(const std::string& vehicleType, const std::map<std::string, int>& vehicleCounts, double totalTollCollected);
 
};

class InvalidInputException : public std::runtime_error {
public:
    InvalidInputException(const std::string& message)
        : std::runtime_error(message) {}
};


void TollBooth::processInputFile() {
    std::ifstream inputFile(INPUT_FILE_PATH);
    std::ofstream outputFile(OUTPUT_FILE_PATH, std::ios_base::app);
    std::string line;

    while (std::getline(inputFile, line)) {
        std::istringstream is(line);
        std::string vehicleNumber, vehicleType, fastTagBalanceStr;

        if (!(std::getline(is, vehicleNumber, ',') &&
              std::getline(is, vehicleType, ',') &&
              std::getline(is, fastTagBalanceStr))) {
            std::cout << "Error parsing input line: " << line << std::endl;
            continue;
        }

        
        double tollFare = getTollFare(vehicleType, vehicleCounts, totalTollCollected);
       double fastTagBalance = std::stod(fastTagBalanceStr);

        if (fastTagBalance < tollFare) {
            std::cout << "Vehicle number " << vehicleNumber << ": Please recharge your FastTag." << std::endl;
        }

        std::time_t now = std::time(nullptr);
        std::tm* currentTime = std::localtime(&now);
        std::ostringstream oss;
        oss << std::put_time(currentTime, "%Y-%m-%d,%H:%M:%S");
        std::string arrivalDate = oss.str();

        outputFile << vehicleNumber << "," << vehicleType << "," << tollFare << "," << arrivalDate << std::endl;

        totalTollCollected += tollFare;
        totalNumberOfVehicles++;
       
    }

    std::cout << "Input file processed." << std::endl;
}


void TollBooth::displayDetails() const {
    std::ifstream outputFile(OUTPUT_FILE_PATH);
    if (!outputFile.is_open()) {
        std::cout << "Failed to open output file." << std::endl;
        return;
    }

    std::cout << ".................................................................." << std::endl;
    std::cout << "=== Details ===" << std::endl;
    std::cout << "1. Number of vehicles passed of each type" << std::endl;
    std::cout << "2. Total vehicles passed" << std::endl;
    std::cout << "3. Total toll amount collected" << std::endl;

    bool validChoice = false;

    while (!validChoice) {
        std::cout << "Enter your choice: ";
        std::string choiceStr;
        std::cin >> choiceStr;

        try {
            int detailChoice = std::stoi(choiceStr);
            if (detailChoice < 1 || detailChoice > 3) {
                throw InvalidInputException("Invalid choice. Please enter a valid choice.");
            }

            validChoice = true;

            switch (detailChoice) {
                case 1: {
                    std::map<std::string, int> vehicleCounts;

                    std::string line;
                    while (std::getline(outputFile, line)) {
                        std::istringstream is(line);
                        std::string vehicleNumber, vehicleType, tollFareStr, timestamp;

                        if (!(std::getline(is, vehicleNumber, ',') &&
                              std::getline(is, vehicleType, ',') &&
                              std::getline(is, tollFareStr, ',') &&
                              std::getline(is, timestamp))) {
                            std::cout << "Error parsing line: " << line << std::endl;
                            continue;
                        }

                        if (vehicleCounts.find(vehicleType) == vehicleCounts.end()) {
                            vehicleCounts[vehicleType] = 1;
                        } else {
                            vehicleCounts[vehicleType]++;
                        }
                    }

                    std::cout << "=== Number of vehicles passed of each type ===" << std::endl;
                    for (const auto& entry : vehicleCounts) {
                        std::cout << entry.first << ": " << entry.second << std::endl;
                    }
                    break;
                }

                case 2: {
                    std::string line;
                    int totalVehiclesPassed = 0;

                    while (std::getline(outputFile, line)) {
                        totalVehiclesPassed++;
                    }

                    std::cout << "=== Total Number of vehicles passed through toll: ===" << std::endl;
                    std::cout << "Total vehicles: " << totalVehiclesPassed << std::endl;
                    break;
                }

                case 3: {
                    std::string line;
                    double totalToll = 0.0;

                    while (std::getline(outputFile, line)) {
                        std::istringstream is(line);
                        std::string vehicleNumber, vehicleType, tollFareStr, timestamp;

                        try {
                            if (!(std::getline(is, vehicleNumber, ',') &&
                                  std::getline(is, vehicleType, ',') &&
                                  std::getline(is, tollFareStr, ',') &&
                                  std::getline(is, timestamp))) {
                                std::cout << "Error parsing line: " << line << std::endl;
                                continue;
                            }

                            double tollFare = std::stod(tollFareStr);
                            totalToll += tollFare;
                        } catch (const std::invalid_argument& e) {
                            std::cout << "Error parsing line: " << line << std::endl;
                        }
                    }

                    std::cout << "=== Total toll amount collected: ===" << std::endl;
                    std::cout << "Total toll: " << totalToll << std::endl;
                    break;
                }
            }
        } catch (const InvalidInputException& e) {
            std::cout << e.what() << std::endl;
        } catch (const std::exception& e) {
            std::cout << "Invalid input. Please enter a valid choice." << std::endl;
        }

        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    std::cout << ".................................................................." << std::endl;
    outputFile.close();
}




double TollBooth::getTollFare(const std::string& vehicleType, const std::map<std::string, int>& vehicleCounts, double totalTollCollected) {
    double tollFare = INITIAL_TOLL_FARE;

    auto it = vehicleCounts.find(vehicleType);
    if (it != vehicleCounts.end()) {
        int vehicleCount = it->second;
        double demandPercentage = static_cast<double>(vehicleCount) / totalTollCollected;

        if (demandPercentage > DEMAND_THRESHOLD) {
            tollFare += tollFare * DEMAND_INCREASE_PERCENT;
        }
    }

    return tollFare;
}

int main() {
    TollBooth tollBooth;

    while (true) {
        std::cout << "=== TOLL BOOTH SIMULATION ===" << std::endl;
        std::cout << "1. Process Input File" << std::endl;
        std::cout << "2. Display Details of the Output File" << std::endl;
        std::cout << "3. Exit" << std::endl;
        std::cout << "Enter your choice: ";

        std::string choiceStr;
        std::cin >> choiceStr;

        int choice = -1;  
    try {
            choice = std::stoi(choiceStr);
            if (choice < 1 || choice > 3) {
                throw InvalidInputException("Invalid choice. Please enter a valid choice.");
            }
        } catch (const InvalidInputException& e) {
            std::cout << e.what() << std::endl;
            continue; // Continue to the next iteration of the loop
        } catch (const std::exception& e) {
            std::cout << "Invalid input. Please enter a valid choice." << std::endl;
            continue; // Continue to the next iteration of the loop
        }

        switch (choice) {
            case 1:
                tollBooth.processInputFile();
                std::cout << "Processed input file." << std::endl;
                break;
            case 2:
                tollBooth.displayDetails();
                break;
            case 3:
                return 0;
            default:
                std::cout << "Invalid choice. Please try again." << std::endl;
                break;
        }
    }

    return 0;
}
