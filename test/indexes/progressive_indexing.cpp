#include <catch.hpp>
#include "tester.hpp"
#include "progressive_index.hpp"


TEST_CASE("Test Progressive Indexing ","[PI]" )
{
    Tester::test(ProgressiveIndex::ID);
}

TEST_CASE("Test Adaptive Progressive Indexing ","[API]" )
{
    std::map<std::string, std::string> config;
    config.insert(make_pair("interactivity_threshold","1"));
    Tester::test(ProgressiveIndex::ID,&config);
}