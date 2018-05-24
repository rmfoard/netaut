#include <iostream>
#include <fstream>
#include <json/json.h>

//using namespace std;

void output(const Json::Value&);

int main() {
/*
    ifstream ifs("alice.json");
    Json::Reader reader;
    Json::Value obj;
    //reader.parse(ifs, obj); // reader can also read strings
    //reader.parse("{ \"alpha\": 1 }", obj); // reader can also read strings
    reader.parse("{ \"alpha\": 1 }", obj); // reader can also read strings
    cout << "value of alpha is " << obj["alpha"] << endl;
    exit(0);

    cout << "Book: " << obj["book"].asString() << endl;
    cout << "Year: " << obj["year"].asUInt() << endl;
    const Json::Value& characters = obj["characters"]; // array of characters
    for (unsigned int i = 0; i < characters.size(); i++){
        cout << "    name: " << characters[i]["name"].asString();
        cout << " chapter: " << characters[i]["chapter"].asUInt();
        cout << endl;
    }
*/

    Json::Value fromScratch;
    Json::Value array;
    array.append("hello");
    array.append("world");
    fromScratch["hello"] = "world";
    fromScratch["number"] = 2;
    fromScratch["array"] = array;
    fromScratch["object"]["hello"] = "world";

    output(fromScratch);

    // write in a nice readible way
    Json::StyledWriter styledWriter;
    std::cout << styledWriter.write(fromScratch);

    // ---- parse from string ----

    // write in a compact way
    Json::FastWriter fastWriter;
    std::string jsonMessage = fastWriter.write(fromScratch);

    Json::Value parsedFromString;
    Json::Reader reader;
    bool parsingSuccessful = reader.parse(jsonMessage, parsedFromString);
    if (parsingSuccessful)
    {
        std::cout << styledWriter.write(parsedFromString) << std::endl;
    }
}

void output(const Json::Value & value)
{
    // querying the json object is very simple
    std::cout << value["hello"];
    std::cout << value["number"];
    std::cout << value["array"][0] << value["array"][1];
    std::cout << value["object"]["hello"];
}
