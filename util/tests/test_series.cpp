#include "catch.hpp"
#include "../secdecutil/series.hpp"

#include <complex>
#include <sstream>
#include <vector>
#include <type_traits>

// Helper struct to check is an iterator is a const_iterator
template<typename Iterator>
struct is_const_iterator
{
    typedef typename std::iterator_traits<Iterator>::pointer pointer;
    static const bool value = std::is_const<typename std::remove_pointer<pointer>::type>::value;
};

TEST_CASE( "Constructor exceptions for vector constructor", "[Series]" ) {

    // no content
    std::vector<int> vector_zero = {};
    REQUIRE_THROWS_AS( secdecutil::Series<int>(0,0,vector_zero), std::invalid_argument);
    REQUIRE_THROWS_AS( secdecutil::Series<int>(0,-1,vector_zero), std::invalid_argument);

    // content size too small
    std::vector<int> vector_one = {1};
    REQUIRE_THROWS_AS( secdecutil::Series<int>(0,1,vector_one), std::invalid_argument);

    // content size too large
    std::vector<int> vector_three = {1,2,3};
    REQUIRE_THROWS_AS( secdecutil::Series<int>(0,1,vector_three), std::invalid_argument);

};

TEST_CASE( "Constructor exceptions for initializer list constructor", "[Series]" ) {

    // no content
    REQUIRE_THROWS_AS( secdecutil::Series<int>(0,0,{}), std::invalid_argument);
    REQUIRE_THROWS_AS( secdecutil::Series<int>(0,-1,{}), std::invalid_argument);

    // content size too small
    REQUIRE_THROWS_AS( secdecutil::Series<int>(0,1,{1}), std::invalid_argument);

    // content size too large
    REQUIRE_THROWS_AS( secdecutil::Series<int>(0,1,{1,2,3}), std::invalid_argument);

};

TEST_CASE( "Check Access", "[Series]" ) {

    int order_min = -2;
    int order_max = 1;
    bool truncated_above = false;

    // vector constructor
    std::vector<int> test_vector = {1,2,3,4};
    auto test_vector_original_value = test_vector.at(0);
    auto test_vector_series = secdecutil::Series<int>(order_min,order_max,test_vector,truncated_above);

    // initializer constructor
    auto test_list_series = secdecutil::Series<int>(order_min,order_max,{1,2,3,4},truncated_above);

    SECTION( "Accessing fields" ) {

        REQUIRE( test_vector_series.get_order_min() == order_min );
        REQUIRE( test_vector_series.get_order_max() == order_max );
        REQUIRE( test_vector_series.get_truncated_above() == truncated_above );

        REQUIRE( test_list_series.get_order_min() == order_min );
        REQUIRE( test_list_series.get_order_max() == order_max );
        REQUIRE( test_list_series.get_truncated_above() == truncated_above );

    };

    SECTION( "Accessing elements of series with [] and at()" ) {

        // operator[]
        REQUIRE( test_vector_series[-2] == test_vector.at(0) );
        REQUIRE( test_vector_series[-1] == test_vector.at(1) );
        REQUIRE( test_vector_series[0] == test_vector.at(2) );
        REQUIRE( test_vector_series[1] == test_vector.at(3) );

        REQUIRE( test_list_series[-2] == test_vector.at(0) );
        REQUIRE( test_list_series[-1] == test_vector.at(1) );
        REQUIRE( test_list_series[0] == test_vector.at(2) );
        REQUIRE( test_list_series[1] == test_vector.at(3) );

        // at()
        REQUIRE( test_vector_series.at(-2) == test_vector.at(0) );
        REQUIRE( test_vector_series.at(-1) == test_vector.at(1) );
        REQUIRE( test_vector_series.at(0) == test_vector.at(2) );
        REQUIRE( test_vector_series.at(1) == test_vector.at(3) );

        REQUIRE( test_list_series.at(-2) == test_vector.at(0) );
        REQUIRE( test_list_series.at(-1) == test_vector.at(1) );
        REQUIRE( test_list_series.at(0) == test_vector.at(2) );
        REQUIRE( test_list_series.at(1) == test_vector.at(3) );

    };

    SECTION( "Modifying elements with []" ) {

        test_vector_series[0] = -test_vector.at(2);

        test_list_series[0] = -test_vector.at(2);

        REQUIRE( test_vector_series.at(0) == -test_vector.at(2) );

        REQUIRE( test_list_series.at(0) == -test_vector.at(2) );

    };

    SECTION( "Modifying vector does not change series [] or at()" ) {

        // Change test_vector value
        test_vector.at(0) = test_vector.at(0) + 1.;

        // Series should have value contained in vector on construction, not currently
        REQUIRE( test_vector_series[-2] == test_vector_original_value );
        REQUIRE( test_vector_series.at(-2) == test_vector_original_value );

    };

    SECTION( "Modifying series does not change vector" ) {

        // Change series value
        test_vector_series[-2] = test_vector_series.at(-2) + 1.;

        // Vector should still have original value
        REQUIRE( test_vector.at(0) == test_vector_original_value );

    };

    SECTION( "Access via iterators: begin, end, rbegin, rend, cbegin, cend, crbegin, crend" ) {

        REQUIRE( *test_vector_series.begin() == 1 ); // first element
        REQUIRE( *--test_vector_series.end() == 4 ); // last element
        REQUIRE( *test_vector_series.rbegin() == 4 ); // last element
        REQUIRE( *--test_vector_series.rend() == 1 ); // first element
        REQUIRE( *test_vector_series.cbegin() == 1 ); // first element
        REQUIRE( *--test_vector_series.cend() == 4 ); // last element
        REQUIRE( *test_vector_series.crbegin() == 4 ); // last element
        REQUIRE( *--test_vector_series.crend() == 1 ); // first element

        REQUIRE( !is_const_iterator<decltype(test_vector_series.begin())>::value );
        REQUIRE( !is_const_iterator<decltype(test_vector_series.end())>::value );
        REQUIRE( !is_const_iterator<decltype(test_vector_series.rbegin())>::value );
        REQUIRE( !is_const_iterator<decltype(test_vector_series.rend())>::value );
        REQUIRE( is_const_iterator<decltype(test_vector_series.cbegin())>::value );
        REQUIRE( is_const_iterator<decltype(test_vector_series.cend())>::value );
        REQUIRE( is_const_iterator<decltype(test_vector_series.crbegin())>::value );
        REQUIRE( is_const_iterator<decltype(test_vector_series.crend())>::value );

    };

    SECTION( "Access via element access: front, back" ) {

        REQUIRE( test_vector_series.front() == 1 ); // first element
        REQUIRE( test_vector_series.back() == 4 ); // last element

    };

    SECTION( "Access via element access: data" ) {

        REQUIRE( *test_vector_series.data() == 1 ); // first element
        REQUIRE( test_vector_series.data()[1] == 2 ); // second element

    };

    SECTION( "Access via element access: get_content" ) {

        REQUIRE( test_vector_series.get_content().at(0) == 1 ); // first element
        REQUIRE( test_vector_series.get_content()[1] == 2 ); // second element

    };

};

TEST_CASE( "Operators == and !=", "[Series]") {

    auto series_1 = secdecutil::Series<int>(-3,4,{-5,-6,-7,0,2,3,4,-1});
    auto series_2 = secdecutil::Series<int>(-3,4,{-5,-6,-7,0,2,3,4,-1}, true, "x");
    auto series_3 = secdecutil::Series<int>(-3,4,{-5,-6,-7,0,2,3,4,-1},false, "eps");
    auto series_4 = secdecutil::Series<int>(-3,4,{-5,-6,-7,0,2,3,4,-1},false, "eps");
    auto series_5 = secdecutil::Series<int>(-3,4,{-5,-6,-6,0,2,3,4,-1});
    auto series_6 = secdecutil::Series<int>(-4,4,{0,-5,-6,-7,0,2,3,4,-1});
    auto series_7 = secdecutil::Series<int>(-3,5,{-5,-6,-7,0,2,3,4,-1,2});
    auto series_8 = secdecutil::Series<int>(-3,4,{-5,-6,-7,0,2,3,4,-1},false, "mu");


    REQUIRE( series_1 == series_2 );
    REQUIRE( series_3 == series_4 );
    REQUIRE( series_1 != series_5 );
    REQUIRE( series_1 != series_6 );
    REQUIRE( series_1 != series_7 );
    REQUIRE( series_1 != series_8 );
    REQUIRE( series_3 != series_8 );

};

TEST_CASE ( "Unary Operators - and +", "[Series]" ) {

    auto series_one = secdecutil::Series<int>(-3,4,{-5,-6,-7,0,2,3,4,-1});
    auto series_one_minus = secdecutil::Series<int>(-3,4,{5,6,7,0,-2,-3,-4,1});

    REQUIRE ( -series_one == series_one_minus );
    REQUIRE ( +series_one == series_one );

};

TEST_CASE( "Operator +", "[Series]" ) {

    auto series_one = secdecutil::Series<int>(                     -3,4,{-5 ,-6 ,-7   , 0   , 2   , 3  , 4   ,-1          },true, "eps");
    auto series_exact_one = secdecutil::Series<int>(               -3,4,{-5 ,-6 ,-7   , 0   , 2   , 3  , 4   ,-1          },false,"eps");
    auto series_two = secdecutil::Series<int>(                     -1,6,{         8   , 6   ,-7   , 0  , 8   , 5  , 4 , 1 },true, "eps");
    auto series_exact_two = secdecutil::Series<int>(               -1,6,{         8   , 6   ,-7   , 0  , 8   , 5  , 4 , 1 },false,"eps");
    auto series_one_plus_two = secdecutil::Series<int>(            -3,4,{-5 ,-6 ,-7+8 , 0+6 , 2-7 , 3+0, 4+8 ,-1+5        },true, "eps");
    auto series_one_plus_exact_two = secdecutil::Series<int>(      -3,4,{-5 ,-6 ,-7+8 , 0+6 , 2-7 , 3+0 ,4+8 ,-1+5        },true, "eps");
    auto series_exact_one_plus_two = secdecutil::Series<int>(      -3,6,{-5 ,-6 ,-7+8 , 0+6 , 2-7 , 3+0 ,4+8 ,-1+5, 4 , 1 },true, "eps");
    auto series_exact_one_plus_exact_two = secdecutil::Series<int>(-3,6,{-5 ,-6 ,-7+8 , 0+6 , 2-7 , 3+0, 4+8 ,-1+5, 4 , 1 },false,"eps");

    auto series_without_constant_order = secdecutil::Series<int>(       -3,-1,{-5 ,-6, -7               },false,"eps");
    auto series_without_constant_order_plus_5 = secdecutil::Series<int>(-3, 0,{-5 ,-6, -7, 5            },false,"eps");
    auto series_one_plus_5 = secdecutil::Series<int>(                   -3, 4,{-5 ,-6 ,-7, 5, 2, 3, 4,-1},true, "eps");

    SECTION ( "Series + Series" ) {
        REQUIRE( ( series_one + series_two ) == series_one_plus_two );
        REQUIRE( ( series_one + series_exact_two ) == series_one_plus_exact_two );
        REQUIRE( ( series_exact_one + series_two ) == series_exact_one_plus_two );
        REQUIRE( ( series_exact_one + series_exact_two ) == series_exact_one_plus_exact_two );
    };

    SECTION ( "Series + scalar" ) {
        REQUIRE( ( series_without_constant_order + 5 ) == series_without_constant_order_plus_5 );
        REQUIRE( ( 5 + series_without_constant_order ) == series_without_constant_order_plus_5 );
        REQUIRE( ( series_one + 5 ) == series_one_plus_5 );
        REQUIRE( ( 5 + series_one ) == series_one_plus_5 );
    };

    SECTION ( "Test 1: +=" ) {
        REQUIRE( ( series_without_constant_order += 5 ) == series_without_constant_order_plus_5 );
        REQUIRE( ( series_one += series_two ) == series_one_plus_two );
    };

    SECTION ( "Test 2: +=" ) {
        REQUIRE( ( series_one += series_exact_two ) == series_one_plus_exact_two );
    };

    SECTION ( "Test 3: +=" ) {
        REQUIRE( ( series_exact_one += series_two ) == series_exact_one_plus_two );
    };

    SECTION ( "Test 4: +=" ) {
        REQUIRE( ( series_exact_one += series_exact_two ) == series_exact_one_plus_exact_two );
    };

};

TEST_CASE( "Operator -", "[Series]" ) {

    auto series_exact_one = secdecutil::Series<int>(                       -3,4,{-5 ,-6 ,-7   , 0   , 2   , 3  , 4   ,-1          },false);
    auto series_exact_two = secdecutil::Series<int>(                       -1,6,{         8   , 6   ,-7   , 0  , 8   , 5  , 4 , 1 },false);
    auto series_exact_one_minus_series_exact_two = secdecutil::Series<int>(-3,6,{-5 ,-6 ,-7-8 , 0-6 , 2+7 , 3-0, 4-8 ,-1-5,-4 ,-1 },false);

    auto series_exact_one_minus_five = secdecutil::Series<int>(            -3,4,{-5 ,-6 ,-7   ,-5   , 2   , 3  , 4   ,-1          },false);
    auto five_minus_series_exact_two = secdecutil::Series<int>(            -1,6,{        -8   ,-1   , 7   , 0  ,-8   ,-5  ,-4 ,-1 },false);


    // Check behaviour for double
    auto d_series_exact_one = secdecutil::Series<double>(                       -3,4,{-5. ,-6. ,-7.    , 0.    , 2.    , 3.   , 4.    ,-1.             },false);
    auto d_series_exact_two = secdecutil::Series<double>(                       -1,6,{           8.    , 6.    ,-7.    , 0.   , 8.    , 5.   , 4. , 1. },false);
    auto d_series_exact_one_minus_series_exact_two = secdecutil::Series<double>(-3,6,{-5. ,-6. ,-7.-8. , 0.-6. , 2.+7. , 3.-0., 4.-8. ,-1.-5.,-4. ,-1. },false);

    SECTION ( "Series - Series" ) {
        REQUIRE( ( series_exact_one - series_exact_two ) == series_exact_one_minus_series_exact_two );
        REQUIRE( ( d_series_exact_one - d_series_exact_two ) == d_series_exact_one_minus_series_exact_two );
    };

    SECTION ( "Series - scalar" ) {
        REQUIRE( ( series_exact_one - 5 ) == series_exact_one_minus_five );
        REQUIRE( ( 5 - series_exact_two ) == five_minus_series_exact_two );
    };

    SECTION ( "Test 1: -=" ) {
        REQUIRE( ( series_exact_one -= series_exact_two ) == series_exact_one_minus_series_exact_two );
    };

    SECTION ( "Test 2: -=" ) {
        REQUIRE( ( d_series_exact_one -= d_series_exact_two ) == d_series_exact_one_minus_series_exact_two );
    };

    SECTION ( "Test 3: -=" ) {
        REQUIRE( ( series_exact_one -= 5 ) == series_exact_one_minus_five );
    };

};

TEST_CASE( "Binary and compound assignment operators with different types", "[Series]" ) {

    auto series1 = secdecutil::Series<std::complex<int>>              (-1,6,{             8 ,  6,- 7 ,  0 ,  8 ,  1   ,  4   , 1  });
    auto series2 = secdecutil::Series<int>                            (-3,4,{- 5 ,- 6  ,- 7 ,  0,  2 ,  3 ,  4 ,- 1               });

    auto series1_plus_series2  = secdecutil::Series<std::complex<int>>(-3,4,{- 5 ,- 6  ,  1 ,  6,- 5 ,  3 , 12 ,  0               });
    auto series1_minus_series2 = secdecutil::Series<std::complex<int>>(-3,4,{  5 ,  6  , 15 ,  6,- 9 , -3 ,  4 ,  2               });
    auto series2_minus_series1 = secdecutil::Series<std::complex<int>>(-3,4,{- 5 ,- 6  ,-15 ,- 6,  9 ,  3 ,- 4 ,- 2               });
    auto series1_times_series2 = secdecutil::Series<std::complex<int>>(-4,3,{-40 ,-78  ,-57 ,  0, 25 ,-17 ,-46 ,-41               });

    auto series1_times_two = secdecutil::Series<std::complex<int>>    (-1,6,{            16 , 12,-14 , 0  , 16 ,  2  ,  8   , 2  });
    auto series2_times_two = secdecutil::Series<int>                  (-3,4,{-10 ,-12  ,-14 ,  0,  4 , 6  ,  8 , -2              });

    auto series2_times_imaginary_two = secdecutil::Series<std::complex<int>>(-3,4,{{0,-10},{0,-12},{0,-14},0,{0,4},{0,6},{0,8},{0,-2}});

    SECTION ( " + " ) {
        REQUIRE( ( series1 +  series2 ) == series1_plus_series2 );
        REQUIRE( ( series2 +  series1 ) == series1_plus_series2 );
        REQUIRE( ( series1 += series2 ) == series1_plus_series2 );
        // REQUIRE( ( series2 += series1 ) == series1_plus_series2 ); // should not compile
    };

    SECTION ( " - " ) {
        REQUIRE( ( series1 -  series2 ) == series1_minus_series2 );
        REQUIRE( ( series2 -  series1 ) == series2_minus_series1 );
        REQUIRE( ( series1 -= series2 ) == series1_minus_series2 );
        // REQUIRE( ( series2 -= series1 ) == series1_minus_series2 ); // should not compile
    };

    SECTION ( " Series<std::complex<int>> * Series<int> " ) {
        REQUIRE( ( series1 *  series2 ) == series1_times_series2 );
        REQUIRE( ( series2 *  series1 ) == series1_times_series2 );
        REQUIRE( ( series1 *= series2 ) == series1_times_series2 );
        // REQUIRE( ( series2 *= series1 ) == series1_times_series2 ); // should not compile
    };

    SECTION ( " Series<std::complex<int>> * int " ) {
        REQUIRE( ( 2 * series1  ) == series1_times_two );
        REQUIRE( ( series1 *  2 ) == series1_times_two );
        REQUIRE( ( series1 *= 2 ) == series1_times_two );
    };

    SECTION ( " Series<int> * int " ) {
        REQUIRE( ( 2 * series2  ) == series2_times_two );
        REQUIRE( ( series2 *  2 ) == series2_times_two );
        REQUIRE( ( series2 *= 2 ) == series2_times_two );
    };

    SECTION ( " Series<int> * std::complex<int> " ) {
        REQUIRE( ( std::complex<int>(0,2) * series2  ) == series2_times_imaginary_two );
        REQUIRE( ( series2 *  std::complex<int>(0,2) ) == series2_times_imaginary_two );
        // REQUIRE( ( series2 *= std::complex<int>(0,2) ) == series2_times_two ); // should not compile
    };

};

TEST_CASE( "Operator *", "[Series]" ) {

    auto series_one =  secdecutil::Series<int>(                 -2,1,{-5      ,-6      ,-7      , 3          },true, "eps");
    auto series_exact_one =  secdecutil::Series<int>(           -2,1,{-5      ,-6      ,-7      , 3          },false,"eps");
    auto series_two =  secdecutil::Series<int>(                 -1,3,{         -3      , 9      , 1   , 2, 3 },true, "eps");
    auto series_exact_two =  secdecutil::Series<int>(           -1,3,{         -3      , 9      , 1   , 2, 3 },false,"eps");
    auto three_times_series_exact_one = secdecutil::Series<int>(-2,1,{ 3*(-5) , 3*(-6) , 3*(-7) , 3*3        },false,"eps");
    auto series_exact_one_times_series_exact_two = secdecutil::Series<int>(-3,4,
                                                                           {
                                                                               (-5)*(-3),
                                                                               (-5)*9+(-6)*(-3),
                                                                               (-5)*1+(-6)*9+(-7)*(-3),
                                                                               (-5)*2+(-6)*1+(-7)*9+3*(-3),
                                                                               (-5)*3+(-6)*2+(-7)*1+3*9,
                                                                               (-6)*3+(-7)*2+3*1,
                                                                               (-7)*3+3*2,
                                                                               3*3
                                                                           },false,"eps");
    auto series_one_times_series_exact_two = secdecutil::Series<int>(-3,0,
                                                                     {
                                                                         (-5)*(-3),
                                                                         (-5)*9+(-6)*(-3),
                                                                         (-5)*1+(-6)*9+(-7)*(-3),
                                                                         (-5)*2+(-6)*1+(-7)*9+3*(-3)
                                                                     },true, "eps");
    auto series_exact_one_times_series_two = secdecutil::Series<int>(-3,1,
                                                                     {
                                                                         (-5)*(-3),
                                                                         (-5)*9+(-6)*(-3),
                                                                         (-5)*1+(-6)*9+(-7)*(-3),
                                                                         (-5)*2+(-6)*1+(-7)*9+3*(-3),
                                                                         (-5)*3+(-6)*2+(-7)*1+3*9
                                                                     },true, "eps");
    auto series_one_times_series_two = secdecutil::Series<int>(-3,0,
                                                               {
                                                                   (-5)*(-3),
                                                                   (-5)*9+(-6)*(-3),
                                                                   (-5)*1+(-6)*9+(-7)*(-3),
                                                                   (-5)*2+(-6)*1+(-7)*9+3*(-3)
                                                               },true, "eps");

    SECTION ( " * " ) {
        REQUIRE( ( series_exact_one * 3 ) == three_times_series_exact_one );
        REQUIRE( ( 3 * series_exact_one ) == three_times_series_exact_one );
        REQUIRE( ( series_exact_one * series_exact_two ) == series_exact_one_times_series_exact_two );
        REQUIRE( ( series_one * series_exact_two ) == series_one_times_series_exact_two );
        REQUIRE( ( series_exact_one * series_two ) == series_exact_one_times_series_two );
        REQUIRE( ( series_one * series_two ) == series_one_times_series_two );
    };

    SECTION ( "Test 1: *= " ) {
        REQUIRE( ( series_exact_one *= 3 ) == three_times_series_exact_one );
    };

    SECTION ( "Test 2: *= " ) {
        REQUIRE( ( series_exact_one *= series_exact_two ) == series_exact_one_times_series_exact_two );
    };

    SECTION ( "Test 3: *= " ) {
        REQUIRE( ( series_one *= series_exact_two ) == series_one_times_series_exact_two );
    };

    SECTION ( "Test 4: *= " ) {
        REQUIRE( ( series_exact_one *= series_two ) == series_exact_one_times_series_two );
    };

    SECTION ( "Test 5: *= " ) {
        REQUIRE( ( series_one *= series_two ) == series_one_times_series_two );
    };

};

TEST_CASE( "Operator * for complex<int>", "[Series]" ) {

    auto one_plus_i_times_x = secdecutil::Series<std::complex<int>>(0,1,{{1,0},{0,1}});
    auto minus_one_minus_i_times_x = secdecutil::Series<std::complex<int>>(0,1,{{-1,0},{0,-1}});
    auto result_operator_minus = - one_plus_i_times_x;
    REQUIRE( result_operator_minus == minus_one_minus_i_times_x );

};

TEST_CASE( "Operator * for complex<double>", "[Series]" ) {

    auto one_plus_i_times_x = secdecutil::Series<std::complex<double>>(0,1,{{1.1,0.},{0.,1.1}});
    auto minus_one_minus_i_times_x = secdecutil::Series<std::complex<double>>(0,1,{{-1.1,0.},{0.,-1.1}});
    auto result_operator_minus = - one_plus_i_times_x;
    REQUIRE( result_operator_minus == minus_one_minus_i_times_x );

};

TEST_CASE( "Check Multivariate Access", "[Series]" ) {

    // general series
    auto multivariate_series =
    secdecutil::Series<secdecutil::Series<int>>(-1,1,{
        secdecutil::Series<int>(-1,0,{1,2}),
        secdecutil::Series<int>(0,2,{3,4,5}),
        secdecutil::Series<int>(2,5,{6,7,8,9})
    });

    SECTION( "Multivariate Accessing elements of series with [] and at()" ) {
        REQUIRE( multivariate_series[-1][-1] == 1 );
        REQUIRE( multivariate_series[-1][0] == 2 );
        REQUIRE( multivariate_series[0][0] == 3 );
        REQUIRE( multivariate_series[0][1] == 4 );
        REQUIRE( multivariate_series[0][2] == 5 );
        REQUIRE( multivariate_series[1][2] == 6 );
        REQUIRE( multivariate_series[1][3] == 7 );
        REQUIRE( multivariate_series[1][4] == 8 );
        REQUIRE( multivariate_series[1][5] == 9 );
    };

};

TEST_CASE( "Multivariate Operator +" , "[Series]" ) {
    auto multivariate_series_one =
    secdecutil::Series<secdecutil::Series<int>>(0,2,{
        secdecutil::Series<int>(0,2,{1,1,1}),
        secdecutil::Series<int>(0,2,{1,1,1}),
        secdecutil::Series<int>(0,2,{1,1,1})
    });

    auto multivariate_series_two =
    secdecutil::Series<secdecutil::Series<int>>(-1,1,{
        secdecutil::Series<int>(-1,1,{1,1,1}),
        secdecutil::Series<int>(-1,1,{1,1,1}),
        secdecutil::Series<int>(-1,1,{1,1,1}),
    });

    // Note: result is not (-1,1,{1,2,2,1}) as last 1 is truncated
    auto multivariate_series_one_plus_two =
    secdecutil::Series<secdecutil::Series<int>>(-1,1,{
        secdecutil::Series<int>(-1,1,{1,1,1}),
        secdecutil::Series<int>(-1,1,{1,2,2}),
        secdecutil::Series<int>(-1,1,{1,2,2})
    });

    SECTION ( "+" ) {
        REQUIRE( (multivariate_series_one + multivariate_series_two) == multivariate_series_one_plus_two );
    };

    SECTION ( "Test 1: += " )
    {
        REQUIRE( (multivariate_series_one += multivariate_series_two) == multivariate_series_one_plus_two );
    }

};

TEST_CASE( "Multivariate Operator -" , "[Series]" ) {
    auto multivariate_series_one =
    secdecutil::Series<secdecutil::Series<int>>(0,2,{
        secdecutil::Series<int>(0,2,{1,1,1}),
        secdecutil::Series<int>(0,2,{1,1,1}),
        secdecutil::Series<int>(0,2,{1,1,1})
    });

    auto multivariate_series_two =
    secdecutil::Series<secdecutil::Series<int>>(-1,1,{
        secdecutil::Series<int>(-1,1,{1,1,1}),
        secdecutil::Series<int>(-1,1,{1,1,1}),
        secdecutil::Series<int>(-1,1,{1,1,1}),
    });

    // Note: result is not (-1,1,{1,2,2,1}) as last 1 is truncated
    auto multivariate_series_one_minus_two =
    secdecutil::Series<secdecutil::Series<int>>(-1,1,{
        secdecutil::Series<int>(-1,1,{-1,-1,-1}),
        secdecutil::Series<int>(-1,1,{-1,0,0}),
        secdecutil::Series<int>(-1,1,{-1,0,0})
    });

    SECTION ( "-" ) {
        REQUIRE( (multivariate_series_one - multivariate_series_two) == multivariate_series_one_minus_two );
    };

    SECTION ( "Test 1: -= " )
    {
        REQUIRE( (multivariate_series_one -= multivariate_series_two) == multivariate_series_one_minus_two );
    }

};

TEST_CASE( "Multivariate Operator *" , "[Series]" ) {

    auto multivariate_series_one =
    secdecutil::Series<secdecutil::Series<int>>(0,1,{
        secdecutil::Series<int>(-1,0,{1,1},false),
        secdecutil::Series<int>(0,1,{1,1},false),
    },false);

    auto multivariate_series_one_sq =
    secdecutil::Series<secdecutil::Series<int>>(0,2,{
        secdecutil::Series<int>(-2,0,{1,2,1},false),
        secdecutil::Series<int>(-1,1,{2,4,2},false),
        secdecutil::Series<int>(0,2,{1,2,1},false)
    },false);

    SECTION ( "*" ) {
        REQUIRE( (multivariate_series_one * multivariate_series_one) == multivariate_series_one_sq );
    };

    SECTION ( "Test 1: *=" ) {
        REQUIRE( (multivariate_series_one *= multivariate_series_one) == multivariate_series_one_sq );
    };
};

TEST_CASE( "Mismatch in expansion parameters in binary operators", "[Series]" ) {

    // default expansion parameter: 'x'
    auto one_plus_x = secdecutil::Series<int>(0,1,{1,1},false);
    auto one_plus_eps = secdecutil::Series<int>(0,2,{1,1,8},true,"eps");

    REQUIRE_THROWS_AS(one_plus_x + one_plus_eps, secdecutil::expansion_parameter_mismatch_error);
    REQUIRE_THROWS_AS(one_plus_x - one_plus_eps, secdecutil::expansion_parameter_mismatch_error);
    REQUIRE_THROWS_AS(one_plus_x * one_plus_eps, secdecutil::expansion_parameter_mismatch_error);

    try
    {
        one_plus_x * one_plus_eps;
    }
    catch (secdecutil::expansion_parameter_mismatch_error& error)
    {
        REQUIRE(error.what() == std::string("\"x\" != \"eps\""));
    }

};

TEST_CASE( "Naming the expansion parameter for operator <<" , "[Series]" ) {

    // default expansion parameter: 'x'
    auto one_plus_x = secdecutil::Series<int>(0,1,{1,1},false);
    std::stringstream stream_x;
    stream_x << one_plus_x;
    std::string target_x = " + (1) + (1)*x";
    REQUIRE( stream_x.str() == target_x );

    auto one_plus_eps = secdecutil::Series<int>(0,2,{1,1,8},true);
    one_plus_eps.expansion_parameter = "eps"; // rename
    std::stringstream stream_eps;
    stream_eps << one_plus_eps;
    std::string target_eps = " + (1) + (1)*eps + (8)*eps^2 + O(eps^3)";
    REQUIRE( stream_eps.str() == target_eps );

    auto plus_order_one = secdecutil::Series<int>(-3,-1,{1,1,8},true);
    std::stringstream stream_order_one;
    stream_order_one << plus_order_one;
    std::string target_order_one = " + (1)*x^-3 + (1)*x^-2 + (8)*x^-1 + O(x^0)";
    REQUIRE( stream_order_one.str() == target_order_one );

    auto plus_order_eps = secdecutil::Series<int>(-2,0,{1,1,8},true,"eps"); // name on construction
    std::stringstream stream_order_eps;
    stream_order_eps << plus_order_eps;
    std::string target_order_eps = " + (1)*eps^-2 + (1)*eps^-1 + (8) + O(eps)";
    REQUIRE( stream_order_eps.str() == target_order_eps );

};

TEST_CASE( "Propagation of the expansion parameter in arithmetic operations" , "[Series]" ) {

    auto s1 = secdecutil::Series<int>(0,1,{1,1},true,"eps");

    SECTION( " + " ) {

        auto added_series = s1 + s1;
        REQUIRE( added_series.expansion_parameter == "eps" );

        auto plus_series = + s1;
        REQUIRE( plus_series.expansion_parameter == "eps" );

    };

    SECTION( " - " ) {

        auto subtracted_series = s1 - s1;
        REQUIRE( subtracted_series.expansion_parameter == "eps" );

        auto minus_series = - s1;
        REQUIRE( minus_series.expansion_parameter == "eps" );

    };

    SECTION( " * " ) {

        auto multiplied_series = s1 * s1;
        REQUIRE( multiplied_series.expansion_parameter == "eps" );

    };

};

TEST_CASE( "Assignment with and without implicit conversion" , "[Series]" ) {

    auto original_series = secdecutil::Series<long>              ( 0, 1, {       2, 2}                 );

    auto s1              = secdecutil::Series<long>              ( 0, 1, {       2, 2}                 );
    auto s2              = secdecutil::Series<long>              (-2,-1, {-3, 3      }, false, "lambda");
    auto s3              = secdecutil::Series<int>               (-1, 1, {   -1, 0, 1}, false, "eps"   );

    auto s4              = secdecutil::Series<std::complex<long>>( 0, 1, {       2, 2}                 );
    auto s3_long_complex = secdecutil::Series<std::complex<long>>(-1, 1, {   -1, 0, 1}, false, "eps"   );

    SECTION( " assignment without conversion " ) {

        REQUIRE( s1 == original_series );
        s1 = s2;
        REQUIRE( s1 != original_series );
        REQUIRE( s1 == s2              );

    };

    SECTION( " assignment with conversion " ) {

        REQUIRE( s1 == original_series );
        s1 = s3;
        REQUIRE( s1 != original_series );
        REQUIRE( s1 == s3              );

        s4 = s3;
        // REQUIRE( s4 == s3 ); // error: no implementation of "==" for types "complex<long>" and "int"
        REQUIRE( s4 == s3_long_complex );

    };

};
