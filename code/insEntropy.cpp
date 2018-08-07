#include <iostream>
#include <assert.h>
#include <math.h>
#include <pqxx/pqxx>

static
double Entropy(std::vector<int>& v, int tFreq) { // empties 'v'
    double sum = 0.0;
    double nrDegrees = v.size();
    while (v.size() > 0) {
        double pk = (double) v.back() / tFreq;
        v.pop_back();
        sum += pk * log2(pk);
    }

    if (nrDegrees == 1.0)
        return 0.0;
    else
        return -sum / log2(nrDegrees);
}

static
void UpdateFinInDegreeEntropy(pqxx::connection& C, const char* runId, double fide) {
    pqxx::work w(C);
    std::string query = "UPDATE runs SET finInDegreeEntropy = "
      + std::to_string(fide)
      + " WHERE runId = '" + C.esc(runId) + "'";
    pqxx::result r = w.exec(query);
    w.commit();
}

int main(int argc, char* argv[]) {
    try {
        pqxx::connection C(std::string("dbname=") + argv[1]);
        pqxx::work w(C);
        pqxx::result R = w.exec("SELECT * FROM runid ORDER BY runid");
        w.commit();

        std::vector<int> freq;
        int totalFreq;
        std::string curId = "";
        int count = 0;

        for (auto row : R) {
            if (count++ % 10000 == 0) std::cout << count << std::endl;
            if (row[0].c_str() != curId) {
                if (curId != "")
                    UpdateFinInDegreeEntropy(C, row[0].c_str(), Entropy(freq, totalFreq));

                // Initialize for next runid
                curId = row[0].c_str();
                assert(freq.size() == 0);
                freq.push_back(row[2].as<int>());
                totalFreq = row[2].as<int>();
            }
            else {
                freq.push_back(row[2].as<int>());
                totalFreq += row[2].as<int>();
            }
        }
        //UpdateFinInDegreeEntropy(C, row[0].c_str(), Entropy(freq, totalFreq));
    }
    catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}
