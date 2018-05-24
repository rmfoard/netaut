#include <iostream>
#include <fstream>
#include <json/json.h>

using namespace std;

int main() {
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
}

