#include "ticket_system.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <random>
#include <ctime>
using namespace std;

TicketSystem::TicketSystem() {
    next_ticket_seq = 1;
    reserve_ratio = 0.20; // 20% seats reserved for long-distance
}

string TicketSystem::genTicketId() {
    std::ostringstream oss;
    oss << "T" << std::setw(8) << std::setfill('0') << next_ticket_seq++;
    return oss.str();
}

void TicketSystem::createExampleTrain(const string &train_id, int N, int seat_count) {
    if (N < 2) N = 11;
    vector<string> sts;
    for (int i = 0; i < N; ++i) {
        ostringstream ss;
        ss << "S" << i;
        sts.push_back(ss.str());
    }
    vector<double> segp;
    std::mt19937_64 rng((unsigned)time(nullptr));
    std::uniform_real_distribution<double> dist(5.0, 30.0);
    for (int i = 0; i < N-1; ++i) segp.push_back(std::round(dist(rng)*100.0)/100.0);
    int reserve = max(1, int(seat_count * reserve_ratio + 0.5));
    Train t(train_id, seat_count, sts, segp, reserve);
    trains[train_id] = t;
}

void TicketSystem::listTrains() const {
    if (trains.empty()) { cout << "没有列车\n"; return; }
    for (const auto &p : trains) {
        const Train &t = p.second;
        cout << "列车 " << t.id << " 站数=" << t.stations.size() << " 座位=" << t.seat_count
             << " 预留=" << t.reserve_count << "\n";
        cout << "站点: ";
        for (size_t i=0;i<t.stations.size();++i) {
            cout << "[" << i << "]" << t.stations[i] << " ";
        }
        cout << "\n分段票价: ";
        for (double v: t.segment_price) cout << v << " ";
        cout << "\n\n";
    }
}

double TicketSystem::computePrice(const Train &t, int from, int to) const {
    if (from<0 || to> (int)t.stations.size() || from>=to) return 0.0;
    double s = 0;
    for (int i = from; i < to; ++i) s += t.segment_price[i];
    return s;
}

int TicketSystem::queryRemainingSeats(const string &train_id, int from, int to) const {
    auto it = trains.find(train_id);
    if (it == trains.end()) return -1;
    const Train &t = it->second;
    int segs = (int)t.stations.size() - 1;
    if (from<0 || to>segs+1 || from>=to) return -1;
    int cnt = 0;
    for (int s = 0; s < t.seat_count; ++s) {
        bool ok = true;
        for (int seg = from; seg < to; ++seg) {
            if (t.seats[s][seg]) { ok = false; break; }
        }
        if (ok) ++cnt;
    }
    return cnt;
}

int TicketSystem::findSeatForBooking(Train &t, int from, int to) {
    int segs = (int)t.stations.size() - 1;
    if (from<0 || to>segs+1 || from>=to) return -1;
    int journey_len = to - from;
    // define long-distance threshold
    int long_thr = max(1, (int)t.stations.size() / 3);
    bool is_long = (journey_len >= long_thr);

    // two pools: ordinary pool (first seat_count - reserve_count seats), reserve pool (last reserve_count seats)
    int ordinary_end = t.seat_count - t.reserve_count; // ordinary seats index: [0, ordinary_end)
    if (ordinary_end < 0) ordinary_end = 0;
    // helper to check seat s
    auto seat_ok = [&](int s)->bool {
        for (int seg = from; seg < to; ++seg) {
            if (t.seats[s][seg]) return false;
        }
        return true;
    };

    // If long journey -> try reserve pool first, then ordinary
    if (is_long) {
        // reserve pool indices [ordinary_end, seat_count)
        for (int s = ordinary_end; s < t.seat_count; ++s) {
            if (seat_ok(s)) return s;
        }
        for (int s = 0; s < ordinary_end; ++s) {
            if (seat_ok(s)) return s;
        }
        return -1;
    } else {
        // short journey -> try ordinary first
        for (int s = 0; s < ordinary_end; ++s) {
            if (seat_ok(s)) return s;
        }
        for (int s = ordinary_end; s < t.seat_count; ++s) {
            if (seat_ok(s)) return s;
        }
        return -1;
    }
}

pair<bool, Ticket> TicketSystem::bookTicket(const string &train_id, const string &passenger, int from, int to) {
    string err;
    return bookTicketResultErr(train_id, passenger, from, to, err);
}

pair<bool, Ticket> TicketSystem::bookTicketResultErr(const string &train_id, const string &passenger, int from, int to, string &out_err) {
    out_err.clear();
    auto it = trains.find(train_id);
    if (it == trains.end()) { out_err = "未找到列车"; return {false, Ticket()}; }
    Train &t = it->second;
    int segs = (int)t.stations.size() - 1;
    if (from < 0 || to > (int)t.stations.size() || from >= to) { out_err = "区间不合法"; return {false, Ticket()}; }
    int seat = findSeatForBooking(t, from, to);
    if (seat < 0) { out_err = "无可用座位"; return {false, Ticket()}; }
    // 标记占用
    for (int seg = from; seg < to; ++seg) t.seats[seat][seg] = true;
    double price = computePrice(t, from, to);
    Ticket tk;
    tk.ticket_id = genTicketId();
    tk.train_id = train_id;
    tk.passenger = passenger;
    tk.from = from;
    tk.to = to;
    tk.seat_no = seat;
    tk.price = price;
    tickets[tk.ticket_id] = tk;
    return {true, tk};
}

bool TicketSystem::refundTicket(const string &ticket_id) {
    auto it = tickets.find(ticket_id);
    if (it == tickets.end()) return false;
    Ticket tk = it->second;
    auto it2 = trains.find(tk.train_id);
    if (it2 == trains.end()) { tickets.erase(it); return true; }
    Train &t = it2->second;
    for (int seg = tk.from; seg < tk.to; ++seg) t.seats[tk.seat_no][seg] = false;
    tickets.erase(it);
    return true;
}

void TicketSystem::showSalesAndRevenue() const {
    double total = 0;
    cout << "已售票数: " << tickets.size() << "\n";
    for (const auto &p : tickets) {
        const Ticket &tk = p.second;
        cout << tk.ticket_id << " 车 " << tk.train_id << " 乘客:" << tk.passenger
             << " [" << tk.from << "->" << tk.to << ") 座位:" << tk.seat_no << " 价:" << tk.price << "\n";
        total += tk.price;
    }
    cout << "总收入: " << total << "\n";
}

void TicketSystem::saveToFiles(const string &train_file, const string &ticket_file) const {
    // trains 保存为简单文本
    ofstream tf(train_file);
    if (tf) {
        tf << trains.size() << "\n";
        for (const auto &p : trains) {
            const Train &t = p.second;
            tf << t.id << " " << t.seat_count << " " << t.reserve_count << " " << t.stations.size() << "\n";
            for (auto &s: t.stations) tf << s << " ";
            tf << "\n";
            for (double v: t.segment_price) tf << v << " ";
            tf << "\n";
            // seats
            for (int i=0;i<t.seat_count;++i) {
                for (bool b: t.seats[i]) tf << (b?1:0);
                tf << "\n";
            }
        }
    }
    ofstream of(ticket_file);
    if (of) {
        of << tickets.size() << "\n";
        for (const auto &p : tickets) {
            const Ticket &tk = p.second;
            of << tk.ticket_id << " " << tk.train_id << " " << tk.passenger << " "
               << tk.from << " " << tk.to << " " << tk.seat_no << " " << tk.price << "\n";
        }
    }
    // next_ticket_seq 保存到同文件 (简单处理)
    ofstream seqf("seq.dat");
    if (seqf) seqf << next_ticket_seq << "\n";
}

void TicketSystem::loadFromFiles(const string &train_file, const string &ticket_file) {
    trains.clear();
    tickets.clear();
    ifstream tf(train_file);
    if (tf) {
        int tcount;
        if (!(tf >> tcount)) return;
        for (int i=0;i<tcount;++i) {
            string id; int seat_count, reserve_count, stn;
            tf >> id >> seat_count >> reserve_count >> stn;
            vector<string> stations(stn);
            for (int j=0;j<stn;++j) tf >> stations[j];
            vector<double> segp(stn-1);
            for (int j=0;j<stn-1;++j) tf >> segp[j];
            Train t;
            t.id = id; t.seat_count = seat_count; t.reserve_count = reserve_count;
            t.stations = stations; t.segment_price = segp;
            t.seats.assign(seat_count, vector<bool>(stn-1,false));
            string line;
            // read seat occupancy lines
            for (int s=0;s<seat_count;++s) {
                tf >> line;
                for (int k=0;k<stn-1 && k<(int)line.size();++k) {
                    t.seats[s][k] = (line[k] == '1');
                }
            }
            trains[id] = t;
        }
    }
    ifstream of(ticket_file);
    if (of) {
        int tcount;
        if (of >> tcount) {
            for (int i=0;i<tcount;++i) {
                Ticket tk;
                of >> tk.ticket_id >> tk.train_id >> tk.passenger >> tk.from >> tk.to >> tk.seat_no >> tk.price;
                tickets[tk.ticket_id] = tk;
            }
        }
    }
    ifstream seqf("seq.dat");
    if (seqf) seqf >> next_ticket_seq;
}
