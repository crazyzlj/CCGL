#ifndef CCGLTEST_GLOBAL_DEFINITIONS_H
#define CCGLTEST_GLOBAL_DEFINITIONS_H
#include "../src/basic.h"
#include "../src/db_mongoc.h"

#ifdef USE_MONGODB
using namespace ccgl::db_mongoc;

class GlobalEnvironment : public ::testing::Environment {
public:
    GlobalEnvironment(MongoClient* client, MongoGridFs* gfs) : client_(client), gfs_(gfs) {}
    virtual ~GlobalEnvironment() {}

    // Override this to define how to set up the environment.
    void SetUp() override {}

    // Override this to define how to tear down the environment.
    void TearDown() override {
        //client_->Destroy(); // the MongoClient MUST not be destroyed or deleted!
        //delete client_;
        ;
    }
public:
    MongoClient* client_;
    MongoGridFs* gfs_;
};
#endif /* USE_MONGODB */
#endif /* CCGLTEST_GLOBAL_DEFINITIONS_H */
