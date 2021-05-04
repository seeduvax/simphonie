/*
 * @file TestResolver.cpp
 *
 * Copyright 2021 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include <cppunit/extensions/HelperMacros.h>
#include <regex>
#include <sstream>

namespace test {

// ----------------------------------------------------------
// test fixture implementation
class TestResolver : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(TestResolver);
    // TODO for each test method:
    CPPUNIT_TEST(ResolveDot);
    CPPUNIT_TEST_SUITE_END();

private:
public:
    void setUp() {}

    void tearDown() {}

    void ResolveDot() {
        Resolver("../model/submodel[2].input");
    }

    void Resolver(std::string path) {
        std::vector<std::string> GlobalPath;
        std::vector<std::string> SlashPath = CutString(path, "/");
        for (std::string itr : SlashPath) {
            if (itr == "..") {
                std::cout << "get .." << std::endl;
                GlobalPath.push_back(itr);
            }
            else {
                std::vector<std::string> DotPath = CutString(itr, "[.]");
                for (std::string itr2 : DotPath) {
                    std::vector<std::string> BracketPath = CutString(itr2, "[[]");
                    // detect if we're between brackets
                    bool open = false;
                    for (std::string itr3 : BracketPath) {
                        if (open == true) {
                            itr3.erase(remove(itr3.begin(), itr3.end(), ']'), itr3.end());
                            // idx
                            std::cout << "idx : " << std::stoi(itr3) << std::endl;
                            GlobalPath.push_back(itr3);
                        }
                        else {
                            // object
                            GlobalPath.push_back(itr3);
                            open = true;
                        }
                    }
                }
            }
        }
        std::cout << "----------------------------------------------------------" << std::endl;
        for (std::string frag : GlobalPath) {
            std::cout << frag << std::endl;
        }
        std::cout << "----------------------------------------------------------" << std::endl;
    }

    std::vector<std::string> CutString(std::string path, std::string delimiter) {
        std::vector<std::string> composedPath;
        std::regex rgx(delimiter);
        std::sregex_token_iterator iter(path.begin(), path.end(), rgx, -1);
        std::sregex_token_iterator end;
        for (; iter != end; ++iter) {
            composedPath.push_back(*iter);
        }
        return composedPath;
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestResolver);
}  // namespace test
