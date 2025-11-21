#include <iostream>
#include <map>
#include <vector>
#include <stack>
#include <queue>
#include <list>
#include <limits>  // for numeric_limits
using namespace std;

// Service structure
struct Service {
    string name;
    int price;
};

// Booking structure
struct Booking {
    int id;
    string customer;
    string branch;
    vector<Service> services;
    string slot;
    int total;
    string paymentMode;
    string feedback;
    float rating; // now supports decimal ratings like 4.5
};

// Global Data
map<int,string> branches = {
    {1,"Branch1"}, {2,"Branch2"}, {3,"Branch3"},{4,"Branch4"}
};

// Discounts for each branch (per service)
map<string,map<string,int>> discounts = {
    {"Branch1", {{"Haircut",10},{"Facial",15},{"Spa",20},{"Manicure",5}}},
    {"Branch2", {{"Manicure",10},{"Pedicure",15},{"Hair Coloring",25},{"Massage",10}}},
    {"Branch3", {{"Haircut",5},{"Facial",10},{"Spa",10},{"Pedicure",20}}},
    {"Branch4", {{"Manicure",15},{"Hair Coloring",30},{"Massage",20},{"Body Scrub",25}}}
};

// All services
vector<Service> allServices = {
    {"Haircut",200},{"Facial",500},{"Spa",1000},
    {"Manicure",300},{"Pedicure",400},{"Hair Coloring",800},{"Massage",600},{"Body Scrub",700}
};

// Slots
map<int,string> slots = {
    {1,"Morning (9AM - 12PM)"},
    {2,"Evening (4PM - 7PM)"},
    {3,"Night (8PM - 10PM)"}
};

// DSA structures
stack<Service> undoStack;          // Undo last service
queue<string> appointments;        // Appointment queue
list<string> waitingList;          // Waiting list
map<string,vector<string>> history;// Customer history
map<string, vector<string>> feedbackList; // Feedback storage
int bookingCounter=1000;

// Helper: Discounted Price
int getPrice(string branch, Service s) {
    int d = discounts[branch][s.name];
    return s.price - (s.price*d/100);
}

int main() {
    string customer; 
    cout<<"Enter your name: "; 
    getline(cin,customer);

    // Branch choice
    cout<<"\nChoose a Salon Branch:\n";
    for(auto &b:branches) cout<<b.first<<". "<<b.second<<"\n";
    int bch; cin>>bch; string branch=branches[bch];

    // Slot choice
    cout<<"\nChoose a Slot (Booking is valid 2 days later):\n";
    for(auto &s:slots) cout<<s.first<<". "<<s.second<<"\n";
    int sch; cin>>sch; string slot=slots[sch];

    // Services choice
    cout<<"\nServices at "<<branch<<" with Discounts:\n";
    for(int i=0;i<allServices.size();i++) {
        Service s=allServices[i];
        cout<<i+1<<". "<<s.name<<" - Rs."<<s.price
            <<" -> Rs."<<getPrice(branch,s)<<" after discount\n";
    }

    vector<Service> chosen; int ch;
    do {
        cout<<"Enter service number (0 to finish): ";
        cin>>ch;
        if(ch>0 && ch<=allServices.size()) {
            Service s=allServices[ch-1];
            chosen.push_back(s);
            undoStack.push(s);
            cout<<s.name<<" added.\n";
        }
    } while(ch!=0);

    // Undo option
    char u; cout<<"Undo last service? (y/n): "; cin>>u;
    if(u=='y' && !undoStack.empty()) {
        Service last=undoStack.top(); undoStack.pop();
        chosen.pop_back();
        cout<<last.name<<" removed.\n";
    }

    // Bill calculation
    int total=0;
    for(auto &s:chosen) total+=getPrice(branch,s);

    // ---------------- ONLINE PAYMENT FEATURE ----------------
    cout<<"\nChoose Payment Mode:\n";
    cout<<"1. UPI (Google Pay/PhonePe)\n2. Credit/Debit Card\n3. Cash on Visit\n";
    int payOpt; cin>>payOpt;
    string payMode;
    switch(payOpt) {
        case 1: payMode="UPI"; break;
        case 2: payMode="Card"; break;
        case 3: payMode="Cash on Visit"; break;
        default: payMode="Cash on Visit";
    }
    cout<<"Processing payment...\n";
    if(payOpt==1 || payOpt==2)
        cout<<"✅ Payment Successful via "<<payMode<<"!\n";
    else
        cout<<"💰 Please pay at the salon counter.\n";

    // Create booking
    Booking bk={++bookingCounter,customer,branch,chosen,slot,total,payMode,"",0.0f};
    appointments.push(customer);
    for(auto &s:chosen) history[customer].push_back(s.name+" at "+branch);

    // Confirmation slip
    cout<<"\n--- Booking Confirmation ---\n";
    cout<<"Booking ID: "<<bk.id<<"\nCustomer: "<<bk.customer
        <<"\nBranch: "<<bk.branch<<"\nSlot: "<<bk.slot
        <<" (2 days later)\nServices: ";
    for(auto &s:bk.services) cout<<s.name<<" ";
    cout<<"\nTotal Bill: Rs."<<bk.total<<"\n";
    cout<<"Payment Mode: "<<bk.paymentMode<<"\n";

    // Queue & Waiting list example
    cout<<"\nNext Appointment in Queue: "<<appointments.front()<<"\n";
    waitingList.push_back("WaitingCustomer1");
    cout<<"First in Waiting List: "<<waitingList.front()<<"\n";

    // Customer history
    cout<<"\n--- Customer History ---\n";
    for(auto &h:history[customer]) cout<<h<<"\n";

    // ---------------- FEEDBACK SYSTEM (updated) ----------------
    cout<<"\nWould you like to give feedback for your booking? (y/n): ";
    char f; cin>>f;
    if(f=='y' || f=='Y') {
        float rating;
        cout<<"Please rate our service (1.0 - 5.0): ";
        cin>>rating;
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // clear input buffer
        cout<<"Write a short review: ";
        string review; 
        getline(cin, review);

        bk.feedback = review;
        bk.rating = rating;
        feedbackList[branch].push_back(customer + " (" + to_string(rating) + "/5): " + review);
        cout<<"Thank you for your valuable feedback!\n";
    }

    // Display feedback summary for that branch
    cout<<"\n--- Recent Feedbacks for "<<branch<<" ---\n";
    for(auto &f:feedbackList[branch]) cout<<f<<"\n";

    cout<<"\nThank you, "<<customer<<"! Your booking is confirmed.\n";

    return 0;
}