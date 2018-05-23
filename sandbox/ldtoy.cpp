#include <cassert>
#include "leveldb/db.h"

int main() {
    printf("main\n");
    leveldb::DB* db;
    leveldb::Options options;
    options.create_if_missing = true;
    leveldb::Status status = leveldb::DB::Open(options, "tmp/testdb", &db);
    assert(status.ok());
    printf("fin.\n");
}
