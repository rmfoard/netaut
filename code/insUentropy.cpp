#include <iostream>
#include <assert.h>
#include <math.h>
#include <pqxx/pqxx>

static
double uEntropy(std::vector<int>& v, int tFreq) {
    double sum = 0.0;
    while (v.size() > 0) {
        double pk = (double) v.back() / tFreq;
        v.pop_back();
        sum += pk * log2(pk);
    }

    if (v.size() == 1)
        return 0.0;
    else
        return -sum;
}

int main()
{
    try
    {
        pqxx::connection C("dbname=toy");
        pqxx::work W(C);
        pqxx::result R = W.exec("SELECT * FROM runid ORDER BY runid");

        std::vector<int> freq;
        int totalFreq;
        std::string curId = "";
        for (auto row : R) {
            if (row[0].c_str() != curId) {
                if (curId != "") {
                    // compute and store entropy
                    std::cout << "uentropy: " << uEntropy(freq, totalFreq) << std::endl;
                }
                curId = row[0].c_str();
                assert(freq.size() == 0);
                freq.push_back(row[2].as<int>());
                totalFreq = row[2].as<int>();
                std::cout << row[0].c_str() << std::endl;
            }
            else {
                freq.push_back(row[2].as<int>());
                totalFreq += row[2].as<int>();
            }
        }
        std::cout << "uentropy: " << uEntropy(freq, totalFreq) << std::endl;
        std::cout << "Making changes definite: ";
        W.commit();
        std::cout << "OK." << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}
