#ifndef TICKET_SYSTEM_H
#define TICKET_SYSTEM_H

#include <string>
#include <vector>
#include <unordered_map>
#include <map>

struct Ticket {
    std::string ticket_id;
    std::string train_id;
    std::string passenger;
    int from; // inclusive station index
    int to;   // exclusive station index
    int seat_no;
    double price;
};

struct Train {
    std::string id;
    std::vector<std::string> stations; // station names
    std::vector<double> segment_price; // size = stations.size() - 1
    int seat_count;
    // seats[seat_id][segment_idx] = true if occupied
    std::vector<std::vector<bool>> seats;
    // reserve pool size
    int reserve_count;

    Train() {}
    Train(const std::string &id_, int seat_cnt, const std::vector<std::string> &sts, const std::vector<double> &segp, int reserve)
      : id(id_), stations(sts), segment_price(segp), seat_count(seat_cnt), reserve_count(reserve) {
        int segs = (int)stations.size() - 1;
        seats.assign(seat_count, std::vector<bool>(segs, false));
    }
};

class TicketSystem {
public:
    TicketSystem();
    // create example train with N stations and seat_count seats
    void createExampleTrain(const std::string &train_id, int N, int seat_count);
    void listTrains() const;
    // return remaining seats for interval [from, to)
    int queryRemainingSeats(const std::string &train_id, int from, int to) const;
    // book ticket, returns pair(success, Ticket). if failed, success=false and second contains ticket with ticket_id empty; error string in second_err
    std::pair<bool, Ticket> bookTicket(const std::string &train_id, const std::string &passenger, int from, int to);
    std::pair<bool, Ticket> bookTicketResultErr(const std::string &train_id, const std::string &passenger, int from, int to, std::string &out_err);
    // refund ticket by ticket_id
    bool refundTicket(const std::string &ticket_id);
    // show sales summary
    void showSalesAndRevenue() const;

    // persistence
    void saveToFiles(const std::string &train_file, const std::string &ticket_file) const;
    void loadFromFiles(const std::string &train_file, const std::string &ticket_file);

private:
    std::map<std::string, Train> trains; // keyed by train id
    std::unordered_map<std::string, Ticket> tickets; // keyed by ticket id
    long long next_ticket_seq;
    // strategy parameters
    double reserve_ratio; // e.g., 0.2
    // helper
    std::string genTicketId();
    double computePrice(const Train &t, int from, int to) const;
    // try to find a seat respecting reserve strategy
    // returns seat_no >=0 on success else -1
    int findSeatForBooking(Train &t, int from, int to);
};

#endif // TICKET_SYSTEM_H
