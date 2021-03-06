from __future__ import print_function
from pySecDec.integral_interface import IntegralLibrary
import sympy as sp
import unittest

class CheckLib(unittest.TestCase):
    def setUp(self):
        # load c++ library
        self.lib = IntegralLibrary('../bubble1L_dot_rank4/bubble1L_dot_rank4_pylink.so')

        # set global options
        self.real_parameters = [1.275, 1.275]
        self.complex_parameters = [30.886875, 30.886875, 123.5475]
        self.maxeval = 10**6
        self.epsrel = 1e-4
        self.epsabs = 1e-7

        self.target_result_with_prefactor = \
        {
              -1:  1.0    + 0.0j,
               0: -1.2708 + 2.4179j
        }

    def check_result(self, computed_series, target_series, epsrel, epsabs, order_min, order_max):
        # convert result to sympy expressions
        computed_series = sp.sympify(  computed_series.replace(',','+I*').replace('+/-','*value+error*')  )

        for order in range(order_min, order_max+1):
            value = complex( computed_series.coeff('eps',order).coeff('value') )
            error = complex( computed_series.coeff('eps',order).coeff('error') )

            # check that the uncertainties are reasonable
            self.assertLessEqual(error.real, abs(2*epsrel * target_series[order].real))
            if target_series[order].imag != 0.0:
                self.assertLessEqual(error.imag, abs(2*epsrel * target_series[order].imag))

            # check that the desired uncertainties are reached
            self.assertLessEqual(error.real, abs(epsrel * value.real) )
            if target_series[order].imag == 0.0:
                self.assertLessEqual(error.imag, epsabs)
            else:
                self.assertLessEqual(error.imag, abs(epsrel * value.imag) )

            # check integral value
            self.assertAlmostEqual(  value.real, target_series[order].real, delta=epsrel*abs(target_series[order].real)  )
            if target_series[order].imag == 0.0:
                self.assertAlmostEqual(  value.imag, target_series[order].imag, delta=epsabs  )
            else:
                self.assertAlmostEqual(  value.imag, target_series[order].imag, delta=epsrel*abs(target_series[order].imag)  )

    def test_Cuhre(self):
        # choose integrator
        self.lib.use_Cuhre(epsrel=self.epsrel, maxeval=self.maxeval, epsabs=self.epsabs, real_complex_together=True, flags=2)

        # integrate
        str_integral_without_prefactor, str_prefactor, str_integral_with_prefactor = self.lib(self.real_parameters, self.complex_parameters)

        # check integral
        self.check_result(str_integral_with_prefactor, self.target_result_with_prefactor, self.epsrel, self.epsabs, order_min=-1, order_max=0)

    def test_Cuhre_CQuad(self):
        # choose integrator
        self.lib.use_Cuhre(epsrel=self.epsrel, maxeval=self.maxeval, epsabs=self.epsabs, real_complex_together=True, flags=2)
        self.lib.use_CQuad(epsrel=self.epsrel, epsabs=self.epsabs, verbose=True)

        # integrate
        str_integral_without_prefactor, str_prefactor, str_integral_with_prefactor = self.lib(self.real_parameters, self.complex_parameters)

        # check integral
        self.check_result(str_integral_with_prefactor, self.target_result_with_prefactor, self.epsrel, self.epsabs, order_min=-1, order_max=0)

if __name__ == '__main__':
    unittest.main()
