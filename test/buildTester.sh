cxxtestgen --error-printer --have-std -o tests.cpp  sta_test_1.h sta_test_2.h
g++ -stdlib=libstdc++ -o tester -I$CXXTEST tests.cpp
