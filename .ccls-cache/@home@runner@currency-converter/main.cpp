
#include <iostream>
// Libcurl library for the http requests
#include <curl/curl.h>
#include <json/json.h>


//function helps receive data from the server, and appends it to a string. returns the size of the data. 

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}


//initiates a curl handle for an http request, and has an option to call data as its received


std::string fetchExchangeRates(const std::string& apiUrl) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, apiUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }

    return readBuffer;
}


//uses a json reader to parse it into a json object, else it will print an error message

void printJsonData(const std::string& jsonString) {
    Json::CharReaderBuilder rbuilder;
    Json::CharReader* reader = rbuilder.newCharReader();
    Json::Value jsonData;
    std::string errs;

    if (reader->parse(jsonString.c_str(), jsonString.c_str() + jsonString.length(), &jsonData, &errs)) {
        std::cout << jsonData << std::endl;
    } else {
        std::cerr << "Failed to parse JSON data: " << errs << std::endl;
    }

    delete reader;
}

// getting exchange rate function, checks if currencies exist, and if they do we retrieve their data and calculate the exchange rate 


double getExchangeRate(const std::string& jsonString, const std::string& fromCurrency, const std::string& toCurrency) {
    Json::CharReaderBuilder rbuilder;
    Json::CharReader* reader = rbuilder.newCharReader();
    Json::Value jsonData;
    std::string errs;

    if (reader->parse(jsonString.c_str(), jsonString.c_str() + jsonString.length(), &jsonData, &errs)) {
        if (jsonData["rates"].isMember(fromCurrency) && jsonData["rates"].isMember(toCurrency)) {
            double fromRate = jsonData["rates"][fromCurrency].asDouble();
            double toRate = jsonData["rates"][toCurrency].asDouble();
            return toRate / fromRate;
        } else {
            std::cerr << "Exchange rate not found!" << std::endl;
            return 0.0;
        }
    } else {
        std::cerr << "Failed to parse JSON data: " << errs << std::endl;
        return 0.0;
    }

    delete reader;
}

// main function defines the URL to find exchange rates from
// checks if we fetched the data successfully


// //Fetches exchange rates from an API.
// Parses the JSON data to extract exchange rates.
// Calculates the exchange rate between two currencies and converts the amount.
// Handles errors in fetching and parsing data.


int main() {
    const std::string apiUrl = "https://api.exchangerate-api.com/v4/latest/USD";

    std::string jsonString = fetchExchangeRates(apiUrl);

    if (jsonString.empty()) {
        std::cerr << "Failed to fetch exchange rates!" << std::endl;
        return 1;
    }

    std::cout << "Fetched JSON data: " << std::endl;
    printJsonData(jsonString);

    std::string fromCurrency, toCurrency;
    double amount;

    std::cout << "Enter the currency you want to convert from (e.g., USD): ";
    std::cin >> fromCurrency;
    std::cout << "Enter the currency you want to convert to (e.g., EUR): ";
    std::cin >> toCurrency;
    std::cout << "Enter the amount you want to convert: ";
    std::cin >> amount;

    double exchangeRate = getExchangeRate(jsonString, fromCurrency, toCurrency);

    if (exchangeRate != 0.0) {
        double convertedAmount = amount * exchangeRate;
        std::cout << amount << " " << fromCurrency << " is equal to " << convertedAmount << " " << toCurrency << std::endl;
    }

    return 0;
}
