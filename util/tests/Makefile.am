check_PROGRAMS = test_series test_integration test_algorithm test_uncertainties

AM_CPPFLAGS = -I$(top_srcdir)

test_series_SOURCES = main.cpp test_series.cpp catch.hpp
test_integration_SOURCES = main.cpp test_integration.cpp catch.hpp
test_algorithm_SOURCES = main.cpp test_algorithm.cpp catch.hpp
test_uncertainties_SOURCES = main.cpp test_uncertainties.cpp catch.hpp

TESTS = $(check_PROGRAMS)