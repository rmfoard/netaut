#include <iostream>
#include <pqxx/pqxx>

int main()
{
    try
    {
        pqxx::connection C("dbname=toy");
        std::cout << "Connected to " << C.dbname() << std::endl;
        pqxx::work W(C);
        pqxx::result R = W.exec("SELECT runid FROM runs limit 3");

        std::cout << "Found " << R.size() << "runid's:" << std::endl;
        for (auto row: R)
            std::cout << row[0].c_str() << std::endl;
/*
        std::cout << "Doubling all employees' salaries..." << std::endl;
        W.exec("UPDATE employee SET salary = salary*2");
*/
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
