#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-ADC");
    BOOST_CHECK(GetBoolArg("-ADC"));
    BOOST_CHECK(GetBoolArg("-ADC", false));
    BOOST_CHECK(GetBoolArg("-ADC", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-ADCo"));
    BOOST_CHECK(!GetBoolArg("-ADCo", false));
    BOOST_CHECK(GetBoolArg("-ADCo", true));

    ResetArgs("-ADC=0");
    BOOST_CHECK(!GetBoolArg("-ADC"));
    BOOST_CHECK(!GetBoolArg("-ADC", false));
    BOOST_CHECK(!GetBoolArg("-ADC", true));

    ResetArgs("-ADC=1");
    BOOST_CHECK(GetBoolArg("-ADC"));
    BOOST_CHECK(GetBoolArg("-ADC", false));
    BOOST_CHECK(GetBoolArg("-ADC", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noADC");
    BOOST_CHECK(!GetBoolArg("-ADC"));
    BOOST_CHECK(!GetBoolArg("-ADC", false));
    BOOST_CHECK(!GetBoolArg("-ADC", true));

    ResetArgs("-noADC=1");
    BOOST_CHECK(!GetBoolArg("-ADC"));
    BOOST_CHECK(!GetBoolArg("-ADC", false));
    BOOST_CHECK(!GetBoolArg("-ADC", true));

    ResetArgs("-ADC -noADC");  // -ADC should win
    BOOST_CHECK(GetBoolArg("-ADC"));
    BOOST_CHECK(GetBoolArg("-ADC", false));
    BOOST_CHECK(GetBoolArg("-ADC", true));

    ResetArgs("-ADC=1 -noADC=1");  // -ADC should win
    BOOST_CHECK(GetBoolArg("-ADC"));
    BOOST_CHECK(GetBoolArg("-ADC", false));
    BOOST_CHECK(GetBoolArg("-ADC", true));

    ResetArgs("-ADC=0 -noADC=0");  // -ADC should win
    BOOST_CHECK(!GetBoolArg("-ADC"));
    BOOST_CHECK(!GetBoolArg("-ADC", false));
    BOOST_CHECK(!GetBoolArg("-ADC", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--ADC=1");
    BOOST_CHECK(GetBoolArg("-ADC"));
    BOOST_CHECK(GetBoolArg("-ADC", false));
    BOOST_CHECK(GetBoolArg("-ADC", true));

    ResetArgs("--noADC=1");
    BOOST_CHECK(!GetBoolArg("-ADC"));
    BOOST_CHECK(!GetBoolArg("-ADC", false));
    BOOST_CHECK(!GetBoolArg("-ADC", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-ADC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-ADC", "eleven"), "eleven");

    ResetArgs("-ADC -bar");
    BOOST_CHECK_EQUAL(GetArg("-ADC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-ADC", "eleven"), "");

    ResetArgs("-ADC=");
    BOOST_CHECK_EQUAL(GetArg("-ADC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-ADC", "eleven"), "");

    ResetArgs("-ADC=11");
    BOOST_CHECK_EQUAL(GetArg("-ADC", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-ADC", "eleven"), "11");

    ResetArgs("-ADC=eleven");
    BOOST_CHECK_EQUAL(GetArg("-ADC", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-ADC", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-ADC", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-ADC", 0), 0);

    ResetArgs("-ADC -bar");
    BOOST_CHECK_EQUAL(GetArg("-ADC", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-ADC=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-ADC", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-ADC=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-ADC", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--ADC");
    BOOST_CHECK_EQUAL(GetBoolArg("-ADC"), true);

    ResetArgs("--ADC=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-ADC", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noADC");
    BOOST_CHECK(!GetBoolArg("-ADC"));
    BOOST_CHECK(!GetBoolArg("-ADC", true));
    BOOST_CHECK(!GetBoolArg("-ADC", false));

    ResetArgs("-noADC=1");
    BOOST_CHECK(!GetBoolArg("-ADC"));
    BOOST_CHECK(!GetBoolArg("-ADC", true));
    BOOST_CHECK(!GetBoolArg("-ADC", false));

    ResetArgs("-noADC=0");
    BOOST_CHECK(GetBoolArg("-ADC"));
    BOOST_CHECK(GetBoolArg("-ADC", true));
    BOOST_CHECK(GetBoolArg("-ADC", false));

    ResetArgs("-ADC --noADC");
    BOOST_CHECK(GetBoolArg("-ADC"));

    ResetArgs("-noADC -ADC"); // ADC always wins:
    BOOST_CHECK(GetBoolArg("-ADC"));
}

BOOST_AUTO_TEST_SUITE_END()
