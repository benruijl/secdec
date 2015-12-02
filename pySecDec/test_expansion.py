"""Unit tests for the expansion routines"""

from .expansion import *
from .expansion import _expand_singular_step, _flatten
from .polynomial import Polynomial, ExponentiatedPolynomial, LogOfPolynomial, PolynomialProduct
from nose.plugins.attrib import attr
import sympy as sp
import unittest

class TestSingularExpansion(unittest.TestCase):
    def setUp(self):
        self.unit_polynomial = Polynomial.from_expression('1', ['eps0','eps1'])
        self.p0 = Polynomial([(0,1),(1,0)], coeffs=[36, 12], polysymbols=['eps0','eps1'])
        self.p1 = Polynomial([(0,1),(1,0)], coeffs=[ 3,  1], polysymbols=['eps0','eps1'])
        self.p2 = Polynomial([(0,1)], coeffs=[1], polysymbols=['eps0','eps1'])
        self.p3 = ExponentiatedPolynomial([(0,1),(1,0)], coeffs=[36, 12], polysymbols=['eps0','eps1'], exponent=-1)
        self.p4 = LogOfPolynomial([(0,1),(1,0)], coeffs=[36, 12], polysymbols=['eps0','eps1'])
        self.p5 = ExponentiatedPolynomial([(0,1),(1,0)], coeffs=[36, 12], polysymbols=['eps0','eps1'], exponent='eps0')

        self.numerator = self.unit_polynomial
        self.denominator = self.p0 * self.p1 * self.p2
        self.denominator = ExponentiatedPolynomial(self.denominator.expolist, self.denominator.coeffs, polysymbols=self.denominator.polysymbols, exponent=-1)
        self.rational_polynomial = PolynomialProduct(self.numerator, self.denominator)

    #@attr('active')
    def test_basic_checks(self):
        correct_input = PolynomialProduct(self.p0, self.p3)
        three_factors = PolynomialProduct(self.p0, self.p3, self.p0)
        first_factor_wrong_type = PolynomialProduct(self.p4, self.p3)
        second_factor_wrong_type = PolynomialProduct(self.p0, self.p4)
        second_factor_wrong_exponent = PolynomialProduct(self.p0, self.p5)

        for expansion_function in (_expand_singular_step, expand_singular):
            # must have a rational polynomial (polynomial product of the form p * p**-1) in the first arg
            self.assertRaisesRegexp(TypeError, 'product.*must.*PolynomialProduct', expansion_function, self.p0, 0, 0)
            self.assertRaisesRegexp(TypeError, 'product.*must.*two factors', expansion_function, three_factors, 0, 0)
            self.assertRaisesRegexp(TypeError, 'first factor.*Polynomial.*not.*subtype', expansion_function, first_factor_wrong_type, 0, 0)
            self.assertRaisesRegexp(TypeError, 'second factor.*ExponentiatedPolynomial', expansion_function, second_factor_wrong_type, 0, 0)
            self.assertRaisesRegexp(TypeError, 'second factor.*exponent.*-1', expansion_function, second_factor_wrong_exponent, 0, 0)
            expansion_function(correct_input, 0, 0) # should not raise an error

        self.assertRaisesRegexp(AssertionError, 'indices.*orders.*same length', expand_singular, correct_input, indices=1, orders=[1,2])

    #@attr('active')
    def test_two_regulators_step(self):
        self.assertRaisesRegexp(ValueError, 'lowest order.*higher than the requested order', _expand_singular_step, self.rational_polynomial, index=1, order=-2)
        expansion = _expand_singular_step(self.rational_polynomial, index=1, order=1)

        self.assertTrue(type(expansion) is Polynomial)
        self.assertEqual(expansion.number_of_variables, 2)
        for coeff in expansion.coeffs:
            self.assertTrue(type(coeff) is PolynomialProduct)

        # expansion in eps1 yields a simple pole --> expansion to order epsilon has three terms
        self.assertEqual(len(expansion.coeffs), 3)

        pole_order = sp.sympify('1/(12*eps0**2) * 1/eps1')
        constant_order = sp.sympify('-(12*3+36)*eps0/(12*12*eps0**4) * 1')
        order_epsilon = sp.sympify('9/(2*eps0**4) * eps1')

        target_expansion = pole_order + constant_order + order_epsilon
        self.assertEqual(target_expansion - sp.sympify(expansion), 0)

        # expand in the other regulator 'eps0'
        second_expansion = expansion.copy()
        for i, coeff in enumerate(expansion.coeffs):
            second_expansion.coeffs[i] = _expand_singular_step(coeff, index=0, order=0)

        # `target_expansion` is already expanded in 'eps0'
        self.assertEqual( (sp.sympify(expansion) - sp.sympify(second_expansion)).simplify() , 0)
        self.assertEqual( (target_expansion - sp.sympify(second_expansion)).simplify() , 0)

    #@attr('active')
    def test_flatten(self):
        # expand in regulator 1 first, then in regulator 0
        expansion_1 = _expand_singular_step(self.rational_polynomial, index=1, order=0)
        expansion_1_0 = expansion_1.copy()
        for i, coeff in enumerate(expansion_1.coeffs):
            expansion_1_0.coeffs[i] = _expand_singular_step(coeff, index=0, order=0)
        flattened_expansion_1_0 = _flatten(expansion_1_0)

        self.assertTrue(type(flattened_expansion_1_0) is Polynomial)
        for coeff in flattened_expansion_1_0.coeffs:
            self.assertFalse(isinstance(coeff, Polynomial))
            self.assertFalse(isinstance(coeff, PolynomialProduct))
            self.assertFalse(isinstance(coeff, PolynomialSum))

        self.assertEqual( (sp.sympify(expansion_1_0) - sp.sympify(flattened_expansion_1_0)).simplify() , 0)

    #@attr('active')
    def test_high_level_function_one_regulator(self):
        self.assertRaisesRegexp(ValueError, 'lowest order.*higher than the requested order', expand_singular, self.rational_polynomial, indices=1, orders=-2)
        expansion = expand_singular(self.rational_polynomial, indices=1, orders=1)

        self.assertTrue(type(expansion) is Polynomial)
        self.assertEqual(expansion.number_of_variables, 2)
        for coeff in expansion.coeffs:
            self.assertFalse(isinstance(coeff, Polynomial))
            self.assertFalse(isinstance(coeff, PolynomialProduct))
            self.assertFalse(isinstance(coeff, PolynomialSum))

        # expansion in eps1 yields a simple pole --> expansion to order epsilon has three terms
        self.assertEqual(len(expansion.coeffs), 3)

        pole_order = sp.sympify('1/(12*eps0**2) * 1/eps1')
        constant_order = sp.sympify('-(12*3+36)*eps0/(12*12*eps0**4) * 1')
        order_epsilon = sp.sympify('9/(2*eps0**4) * eps1')

        target_expansion = pole_order + constant_order + order_epsilon
        self.assertEqual(target_expansion - sp.sympify(expansion), 0)

    #@attr('active')
    def test_high_level_function_two_regulators(self):
        # expand in regulator 1 first, then in regulator 0
        expansion_1 = _expand_singular_step(self.rational_polynomial, index=1, order=1)
        expansion_1_0 = expansion_1.copy()
        for i, coeff in enumerate(expansion_1.coeffs):
            expansion_1_0.coeffs[i] = _expand_singular_step(coeff, index=0, order=0)
        flattened_expansion_1_0 = _flatten(expansion_1_0)

        # the high-level function should run exactly the commands above
        high_level_output = expand_singular(self.rational_polynomial, indices=[1,0], orders=[1,0])

        self.assertTrue(type(high_level_output) is Polynomial)
        for coeff in high_level_output.coeffs:
            self.assertFalse(isinstance(coeff, Polynomial))
            self.assertFalse(isinstance(coeff, PolynomialProduct))
            self.assertFalse(isinstance(coeff, PolynomialSum))

        self.assertEqual( (sp.sympify(high_level_output) - sp.sympify(flattened_expansion_1_0)).simplify() , 0)