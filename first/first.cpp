#include <iostream>
#include <vector>
#include <string>
#include <fstream>
//#include <cstdlib>
#include <sstream>

using namespace std;

class Flight
{
protected:
    string flightNumber;
    string departureCity;
    string destinationCity;
    string departureTime;
    string flightDate;
    int seatsAvailable;
    double farePerSeat;

public:
    
    Flight(string fNum, string depCity, string destCity, string depTime, string date, int seats, double fare)
        : flightNumber(fNum), departureCity(depCity), destinationCity(destCity), departureTime(depTime), flightDate(date), seatsAvailable(seats), farePerSeat(fare) {}

   
    virtual void displayFlightDetails() const
    {
        cout << "Flight Number: " << flightNumber
             << ", Departure City: " << departureCity
             << ", Destination: " << destinationCity
             << ", Departure Time: " << departureTime
             << ", Flight Date: " << flightDate
             << ", Seats Available: " << seatsAvailable
             << ", Fare per Seat: $" << farePerSeat << endl;
    }

    string getFlightNumber() const { return flightNumber; }
    string getDepartureCity() const { return departureCity; }
    string getDestinationCity() const { return destinationCity; }
    string getDepartureTime() const { return departureTime; }
    string getFlightDate() const { return flightDate; }
    int getSeatsAvailable() const { return seatsAvailable; }
    double getFarePerSeat() const { return farePerSeat; }

    // Book seats if available
    void bookSeats(int numSeats)
    {
        seatsAvailable -= numSeats;
    }

    // Restore seats after cancellation
    void cancelSeats(int numSeats)
    {
        seatsAvailable += numSeats;
    }
};


class DomesticFlight : public Flight
{
public:
    DomesticFlight(string fNum, string depCity, string destCity, string depTime, string date, int seats)
        : Flight(fNum, depCity, destCity, depTime, date, seats, 100.0) {} // Domestic fare is $100 per seat
};


class InternationalFlight : public Flight
{
public:
    InternationalFlight(string fNum, string depCity, string destCity, string depTime, string date, int seats)
        : Flight(fNum, depCity, destCity, depTime, date, seats, 300.0) {} // International fare is $300 per seat
};

// Flight Reservation System with additional search functionality
class ReservationSystem
{
private:
    vector<Flight *> domesticFlights;
    vector<Flight *> internationalFlights;

public:
    // Add domestic and international flights
    void addDomesticFlight(Flight *flight) { domesticFlights.push_back(flight); }
    void addInternationalFlight(Flight *flight) { internationalFlights.push_back(flight); }


    void loadFlightsFromFile(const string &filename, bool isDomestic)
    {
        ifstream file(filename);
        if (!file.is_open())
        {
            cout << "Error opening file: " << filename << endl;
            return;
        }

        string flightNum, depCity, destCity, depTime, date;
        int seats;
        while (file >> flightNum >> depCity >> destCity >> depTime >> date >> seats)
        {
            if (isDomestic)
            {
                addDomesticFlight(new DomesticFlight(flightNum, depCity, destCity, depTime, date, seats));
            }
            else
            {
                addInternationalFlight(new InternationalFlight(flightNum, depCity, destCity, depTime, date, seats));
            }
        }
        file.close();
    }

    // Display available flights
    void displayAvailableFlights(bool isDomestic) const
    {
        const vector<Flight *> &flights = isDomestic ? domesticFlights : internationalFlights;
        if (flights.empty())
        {
            cout << "No available flights." << endl;
            return;
        }

        cout << "Available Flights:" << endl;
        for (const Flight *flight : flights)
        {
            flight->displayFlightDetails();
        }
    }

    // Book a ticket
    void bookTicket(bool isDomestic)
    {
        displayAvailableFlights(isDomestic);

        string flightNum, name;
        int age, numPassengers;

        cout << "Enter flight number to book: ";
        cin >> flightNum;

        Flight *selectedFlight = nullptr;
        for (Flight *flight : (isDomestic ? domesticFlights : internationalFlights))
        {
            if (flight->getFlightNumber() == flightNum)
            {
                selectedFlight = flight;
                break;
            }
        }

        if (!selectedFlight)
        {
            cout << "Flight not found!" << endl;
            return;
        }

        cout << "Enter number of passengers: ";
        cin >> numPassengers;

        if (selectedFlight->getSeatsAvailable() < numPassengers)
        {
            cout << "Not enough seats available!" << endl;
            return;
        }

        vector<pair<string, int>> passengers;
        for (int i = 0; i < numPassengers; ++i)
        {
            cout << "Enter name of passenger " << (i + 1) << ": ";
            cin >> name;
            cout << "Enter age of passenger " << (i + 1) << ": ";
            cin >> age;
            passengers.push_back(make_pair(name, age));
        }

        selectedFlight->bookSeats(numPassengers);
        double totalFare = selectedFlight->getFarePerSeat() * numPassengers;
        int reservationID = rand() % 10000 + 1000;
        saveReservation(passengers, selectedFlight->getFlightNumber(), selectedFlight->getFlightDate(), numPassengers, totalFare, reservationID);
        showReceipt(passengers, selectedFlight, numPassengers, totalFare, reservationID);
    }

    // Save reservation
    void saveReservation(const vector<pair<string, int>> &passengers, const string &flightNumber, const string &date, int numPassengers, double totalFare, int reservationID)
    {
        ofstream file("reservations.txt", ios::app);
        if (!file.is_open())
        {
            cout << "Error opening file: reservations.txt" << endl;
            return;
        }

        file << reservationID << " " << flightNumber << " " << date << " " << numPassengers << " " << totalFare << endl;
        for (const auto &passenger : passengers)
        {
            file << passenger.first << " " << passenger.second << endl;
        }
        file << "-----" << endl;
        file.close();
    }

    // Show booking receipt
    void showReceipt(const vector<pair<string, int>> &passengers, Flight *selectedFlight, int numPassengers, double totalFare, int reservationID)
    {
        cout << "\n*** Booking Receipt ***" << endl;
        cout << "Reservation ID: " << reservationID << endl;
        cout << "Flight Number: " << selectedFlight->getFlightNumber() << endl;
        cout << "Departure City: " << selectedFlight->getDepartureCity() << endl;
        cout << "Destination: " << selectedFlight->getDestinationCity() << endl;
        cout << "Departure Time: " << selectedFlight->getDepartureTime() << endl;
        cout << "Flight Date: " << selectedFlight->getFlightDate() << endl;
        cout << "Number of Passengers: " << numPassengers << endl;
        cout << "Total Fare: $" << totalFare << endl;
        for (const auto &passenger : passengers)
        {
            cout << "Name: " << passenger.first << ", Age: " << passenger.second << endl;
        }
        cout << "*************************" << endl;
    }

    // Cancel reservation by Reservation ID
    void cancelReservation()
    {
        cout << "Enter Reservation ID to cancel: ";
        int reservationID;
        cin >> reservationID;

        ifstream file("reservations.txt");
        if (!file.is_open())
        {
            cout << "Error opening reservations file!" << endl;
            return;
        }

        ofstream tempFile("temp.txt"); 
        string line;
        bool reservationFound = false;
        string canceledFlightNum;
        int canceledPassengers = 0;

        // Search for reservation ID and update the reservations file
        while (getline(file, line))
        {
            istringstream iss(line);
            int id;
            iss >> id;

            if (id == reservationID)
            {
                reservationFound = true;
                cout << "Reservation with ID " << reservationID << " has been canceled." << endl;

                
                string flightNum, date;
                int numPassengers;
                double fare;
                iss >> flightNum >> date >> numPassengers >> fare;

                canceledFlightNum = flightNum;
                canceledPassengers = numPassengers;

                
                while (getline(file, line) && line != "-----")
                {
                }
            }
            else
            {
                tempFile << line << endl; 
            }
        }

        file.close();
        tempFile.close();

        remove("reservations.txt");
        rename("temp.txt", "reservations.txt");

        if (!reservationFound)
        {
            cout << "Reservation ID not found." << endl;
        }
        else
        {
            updateSeatsAfterCancellation(canceledFlightNum, canceledPassengers);
        }
    }

   
    void updateSeatsAfterCancellation(const string &flightNum, int numSeats)
    {
        // Check domestic flights
        for (Flight *flight : domesticFlights)
        {
            if (flight->getFlightNumber() == flightNum)
            {
                flight->cancelSeats(numSeats); // Restore seats
                cout << "Updated seats for domestic flight " << flightNum << ": " << flight->getSeatsAvailable() << " seats now available." << endl;
                return;
            }
        }

        // Check international flights
        for (Flight *flight : internationalFlights)
        {
            if (flight->getFlightNumber() == flightNum)
            {
                flight->cancelSeats(numSeats); 
                cout << "Updated seats for international flight " << flightNum << ": " << flight->getSeatsAvailable() << " seats now available." << endl;
                return;
            }
        }

        // Flight not found
        cout << "Flight number " << flightNum << " not found." << endl;
    }

    // ** New Method: Search and display reservations by date **
    void getReservationsByDate(const string &date)
    {
        ifstream file("reservations.txt");
        if (!file.is_open())
        {
            cout << "Error opening reservations file!" << endl; 
            return;
        }

        string line;
        bool found = false;
        while (getline(file, line))
        {
            istringstream iss(line);
            int reservationID, numPassengers;
            string flightNum, flightDate;
            double fare;

            // Parse reservation details
            iss >> reservationID >> flightNum >> flightDate >> numPassengers >> fare;

            // Check if the flight matches the specified date
            if (flightDate == date)
            {
                found = true;
                cout << "\n*** Reservation on " << date << " ***" << endl;
                cout << "Reservation ID: " << reservationID << endl;
                cout << "Flight Number: " << flightNum << endl;
                cout << "Number of Passengers: " << numPassengers << endl;
                cout << "Total Fare: $" << fare << endl;

                
                while (getline(file, line) && line != "-----")
                {
                    cout << line << endl; 
                }
            }
        }

        if (!found)
        {
            cout << "No reservations found for the date: " << date << endl;
        }

        file.close();
    }
};

int main()
{
    ReservationSystem system;

    system.loadFlightsFromFile("domestic_flights.txt", true);
    system.loadFlightsFromFile("international_flights.txt", false);

    int choice;
    do
    {
        std::cout << "\t\t|------------------------------------------------------------------|\n";
		std::cout << "\t\t| WELCOME TO AIRLINE FLIGHT RESERVATION SYSTEM                     |\n";
		std::cout << "\t\t|------------------------------------------------------------------|\n";
		std::cout << "\t\t| 1) DISPLAY DEOMESTIC FLIGHTS                                     |\n";
		std::cout << "\t\t| 2) DISPLAY INTERNATIONAL FLIGHTS                                 |\n";
		std::cout << "\t\t| 3) BOOK A TICKET                                                 |\n";
		std::cout << "\t\t| 4) CANCEL A RESERVATION                                          |\n";
		std::cout << "\t\t| 5) SEARCH RESERVATION BY DATE                                    |\n";
		std::cout << "\t\t| 6) EXIT                                                          |\n";
		std::cout << "\t\t|------------------------------------------------------------------|\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice)
        {
        case 1:
            system.displayAvailableFlights(true);
            break;
        case 2:
            system.displayAvailableFlights(false);
            break;
        case 3:
        {
            cout << "1. Domestic Flight\n";
            cout << "2. International Flight\n";
            cout << "Enter your choice: ";
            int flightType;
            cin >> flightType;
            system.bookTicket(flightType == 1);
            break;
        }
        case 4:
            system.cancelReservation();
            break;
        case 5: 
        {
            string date;
            cout << "Enter the flight date (DD-MM-YYYY): ";
            cin >> date;
            system.getReservationsByDate(date);
            break;
        }
        case 6:
            cout << "Thank you for using the Flight Reservation System!" << endl;
            break;
        default:
            cout << "Invalid choice! Please try again." << endl;
        }
    } while (choice != 6);

    return 0;
}

