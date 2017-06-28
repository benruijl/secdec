"""
Integral Interface
------------------

An interface to libraries generated by
:func:`pySecDec.code_writer.make_package` or
:func:`pySecDec.loop_integral.loop_package`.

"""

from ctypes import CDLL, c_void_p, c_char_p, c_bool, c_int, c_uint, c_longlong, c_double
from multiprocessing import Process

class CPPIntegrator(object):
    '''
    Abstract base class for integrators to be used with
    an :class:`.IntegralLibrary`.
    This class holds a pointer to the c++ integrator and
    defines the destructor.

    '''
    def __del__(self):
        self.c_lib.free_integrator.restype = None
        self.c_lib.free_integrator.argtypes = [c_void_p]
        self.c_lib.free_integrator(self.c_integrator_ptr)

class Vegas(CPPIntegrator):
    '''
    Wrapper for the Vegas integrator defined in the cuba
    library.

    :param integral_library:
        :class:`IntegralLibrary`;
        The integral to be computed with this integrator.

    The other options are defined in the cuba manual.

    '''
    def __init__(self,integral_library,epsrel=1e-2,epsabs=1e-7,flags=0,seed=0,mineval=0,maxeval=10**6,nstart=1000,nincrease=500,nbatch=1000,real_complex_together=False):
        self.c_lib = integral_library.c_lib
        self.c_lib.allocate_cuba_Vegas.restype = c_void_p
        self.c_lib.allocate_cuba_Vegas.argtypes = [c_double, c_double, c_int, c_int, c_longlong, c_longlong, c_longlong, c_longlong, c_longlong, c_bool]
        self.c_integrator_ptr = self.c_lib.allocate_cuba_Vegas(epsrel,epsabs,flags,seed,mineval,maxeval,nstart,nincrease,nbatch,real_complex_together)

class Suave(CPPIntegrator):
    '''
    Wrapper for the Suave integrator defined in the cuba
    library.

    :param integral_library:
        :class:`IntegralLibrary`;
        The integral to be computed with this integrator.

    The other options are defined in the cuba manual.

    '''
    def __init__(self,integral_library,epsrel=1e-2,epsabs=1e-7,flags=0,seed=0,mineval=0,maxeval=10**6,nnew=1000,nmin=10,flatness=25.,real_complex_together=False):
        self.c_lib = integral_library.c_lib
        self.c_lib.allocate_cuba_Suave.restype = c_void_p
        self.c_lib.allocate_cuba_Suave.argtypes = [c_double, c_double, c_int, c_int, c_longlong, c_longlong, c_longlong, c_longlong, c_double, c_bool]
        self.c_integrator_ptr = self.c_lib.allocate_cuba_Suave(epsrel,epsabs,flags,seed,mineval,maxeval,nnew,nmin,flatness,real_complex_together)

class Divonne(CPPIntegrator):
    '''
    Wrapper for the Divonne integrator defined in the cuba
    library.

    :param integral_library:
        :class:`IntegralLibrary`;
        The integral to be computed with this integrator.

    The other options are defined in the cuba manual.

    '''
    def __init__(self, integral_library, epsrel=1e-2, epsabs=1e-7, flags=0, seed=0, mineval=0, maxeval=10**6,
                                         key1=2000, key2=1, key3=1, maxpass=4, border=0., maxchisq=1.,
                                         mindeviation=.15, real_complex_together=False):
        self.c_lib = integral_library.c_lib
        self.c_lib.allocate_cuba_Divonne.restype = c_void_p
        self.c_lib.allocate_cuba_Divonne.argtypes = [c_double, c_double, c_int, c_int, c_longlong, c_longlong,
                                                     c_int, c_int, c_int, c_int, c_double, c_double, c_double,
                                                     c_bool]
        self.c_integrator_ptr = self.c_lib.allocate_cuba_Divonne(epsrel, epsabs, flags, seed, mineval,maxeval,
                                                                 key1, key2, key3, maxpass, border, maxchisq,
                                                                 mindeviation, real_complex_together)

class Cuhre(CPPIntegrator):
    '''
    Wrapper for the Cuhre integrator defined in the cuba
    library.

    :param integral_library:
        :class:`IntegralLibrary`;
        The integral to be computed with this integrator.

    The other options are defined in the cuba manual.

    '''
    def __init__(self,integral_library,epsrel=1e-2,epsabs=1e-7,flags=0,mineval=0,maxeval=10**6,key=0,real_complex_together=False):
        self.c_lib = integral_library.c_lib
        self.c_lib.allocate_cuba_Cuhre.restype = c_void_p
        self.c_lib.allocate_cuba_Cuhre.argtypes = [c_double, c_double, c_int, c_longlong, c_longlong, c_int, c_bool]
        self.c_integrator_ptr = self.c_lib.allocate_cuba_Cuhre(epsrel,epsabs,flags,mineval,maxeval,key,real_complex_together)

class IntegralLibrary(object):
    r'''
    Interface to a c++ library produced by
    :func:`.make_package` or :func:`.loop_package`.

    :param shared_object_path:
        str;
        The path to the file "<name>_pylink.so"
        that can be built by the command

        .. code::

            $ make pylink

        in the root directory of the c++ library.

    Instances of this class can be called with the
    following arguments:

    :param real_parameters:
        iterable of float;
        The real_parameters of the library.

    :param complex_parameters:
        iterable of complex;
        The complex parameters of the library.

    :param together:
        bool, optional;
        Whether to integrate the sum of all sectors
        or to integrate the sectors separately.
        Default: ``True``.

    :param number_of_presamples:
        unsigned int, optional;
        The number of samples used for the
        contour optimization.
        This option is ignored if the integral
        library was created without deformation.
        Default: ``100000``.

    :param deformation_parameters_maximum:
        float, optional;
        The maximal value the deformation parameters
        :math:`\lambda_i` can obtain.
        If ``number_of_presamples=0``, all
        :math:`\lambda_i` are set to this value.
        This option is ignored if the integral
        library was created without deformation.
        Default: ``1.0``.

    :param deformation_parameters_minimum:
        float, optional;
        The minimal value the deformation parameters
        :math:`\lambda_i` can obtain.
        If ``number_of_presamples=0``, all
        :math:`\lambda_i` are set to this value.
        This option is ignored if the integral
        library was created without deformation.
        Default: ``1e-5``.

    :param deformation_parameters_decrease_factor:
        float, optional;
        If the sign check with the optimized
        :math:`\lambda_i` fails, all :math:`\lambda_i`
        are multiplied by this value until the sign
        check passes.
        This option is ignored if the integral
        library was created without deformation.
        Default: ``0.9``.

    The call operator returns three strings:
    * The integral without its prefactor
    * The prefactor
    * The integral multiplied by the prefactor

    The integrator cen be configured by calling the
    member methods :meth:`.use_Vegas`, :meth:`.use_Suave`,
    :meth:`.use_Divonne`, and :meth:`.use_Cuhre`.
    The available options are listed in the documentation of
    :class:`.Vegas`, :class:`.Suave`, :class:`.Divonne`, and
    :class:`.Cuhre`, respectively.
    If not specified otherwise, :class:`.Vegas` is used with
    its default arguments. For details about the options,
    refer to the cuba manual.

    Further information about the library is stored in
    the member variable `info` of type :class:`dict`.

    '''
    def __init__(self, shared_object_path):
        # import c++ library
        c_lib = self.c_lib = CDLL(shared_object_path)


        # set c prototypes
        c_lib.allocate_string.restype = c_void_p
        c_lib.allocate_string.argtypes = None

        c_lib.free_string.restype = None
        c_lib.free_string.argtypes = [c_void_p]

        c_lib.get_integral_info.restype = c_void_p
        c_lib.get_integral_info.argtypes = [c_void_p]

        c_lib.string2charptr.restype = c_char_p
        c_lib.string2charptr.argtypes = [c_void_p]


        # get integral info
        cpp_str_integral_info = c_lib.allocate_string()
        c_lib.get_integral_info(cpp_str_integral_info)
        str_integral_info = c_lib.string2charptr(cpp_str_integral_info)
        c_lib.free_string(cpp_str_integral_info)
        if not isinstance(str_integral_info, str):
            str_integral_info = str_integral_info.decode('ASCII')


        # store the integral info in a dictionary
        integral_info = self.info = dict()
        for line in str_integral_info.split('\n'):
            key, value = line.split('=')
            integral_info[key.strip()] = value.strip(' ,')


        # comtinue set c prototypes
        self.real_parameter_t = c_double * int(integral_info['number_of_real_parameters'])
        self.complex_parameter_t = c_double * (2*int(integral_info['number_of_complex_parameters'])) # flattened as: ``real(x0), imag(x0), real(x1), imag(x1), ...``

        c_lib.compute_integral.restype = None
        c_lib.compute_integral.argtypes = [
                                               c_void_p, c_void_p, c_void_p, # output strings
                                               c_void_p, # integrator
                                               self.real_parameter_t, # double array
                                               self.complex_parameter_t, # double array as real(x0), imag(x0), real(x1), imag(x1), ...
                                               c_bool, # together
                                               c_uint, # number_of_presamples
                                               c_double, # deformation_parameters_maximum
                                               c_double, # deformation_parameters_minimum
                                               c_double # deformation_parameters_decrease_factor
                                          ]

        # set the default integrator
        self.integrator = Vegas(self)

    def __call__(
                     self, real_parameters=[], complex_parameters=[], together=True,
                     number_of_presamples=100000, deformation_parameters_maximum=1.,
                     deformation_parameters_minimum=1.e-5,
                     deformation_parameters_decrease_factor=0.9
                ):
        # Passed in correct number of parameters?
        assert len(real_parameters) == int(self.info['number_of_real_parameters']), \
            'Passed %i `real_parameters` but %s needs %i.' % (len(real_parameters),self.info['name'],int(self.info['number_of_real_parameters']))
        assert len(complex_parameters) == int(self.info['number_of_complex_parameters']), \
            'Passed %i `complex_parameters` but `%s` needs %i.' % (len(complex_parameters),self.info['name'],int(self.info['number_of_complex_parameters']))

        # Set sample values
        c_real_parameters = self.real_parameter_t(*real_parameters)

        flattened_complex_parameters = []
        for c in complex_parameters:
            flattened_complex_parameters.append(c.real)
            flattened_complex_parameters.append(c.imag)
        c_complex_parameters = self.complex_parameter_t(*flattened_complex_parameters)

        # allocate c++ strings
        cpp_str_integral_without_prefactor = self.c_lib.allocate_string()
        cpp_str_prefactor = self.c_lib.allocate_string()
        cpp_str_integral_with_prefactor = self.c_lib.allocate_string()

        # Call function in a new process. That way, the interpreter
        # is not terminated when a c++ exception is thrown.
        Process(
                   target=self.c_lib.compute_integral,
                   args=(
                            cpp_str_integral_without_prefactor, cpp_str_prefactor, cpp_str_integral_with_prefactor,
                            self.integrator.c_integrator_ptr, c_real_parameters, c_complex_parameters, together,
                            number_of_presamples, deformation_parameters_maximum, deformation_parameters_minimum,
                            deformation_parameters_decrease_factor
                        )
               ).run()

        # convert c++ stings to python strings or bytes (depending on whether we use python2 or python3)
        str_integral_without_prefactor = self.c_lib.string2charptr(cpp_str_integral_without_prefactor)
        str_prefactor = self.c_lib.string2charptr(cpp_str_prefactor)
        str_integral_with_prefactor = self.c_lib.string2charptr(cpp_str_integral_with_prefactor)

        # free allocated c++ strings
        self.c_lib.free_string(cpp_str_integral_without_prefactor)
        self.c_lib.free_string(cpp_str_prefactor)
        self.c_lib.free_string(cpp_str_integral_with_prefactor)

        # python 2/3 compatibility: make sure the strings read from c++ have type "str" with ASCII encoding
        if not isinstance(str_integral_without_prefactor, str):
            str_integral_without_prefactor = str_integral_without_prefactor.decode('ASCII')
        if not isinstance(str_prefactor, str):
            str_prefactor = str_prefactor.decode('ASCII')
        if not isinstance(str_integral_with_prefactor, str):
            str_integral_with_prefactor = str_integral_with_prefactor.decode('ASCII')

        return str_integral_without_prefactor, str_prefactor, str_integral_with_prefactor

    def use_Vegas(self, *args, **kwargs):
        self.integrator = Vegas(self,*args,**kwargs)

    def use_Suave(self, *args, **kwargs):
        self.integrator = Suave(self,*args,**kwargs)

    def use_Divonne(self, *args, **kwargs):
        self.integrator = Divonne(self,*args,**kwargs)

    def use_Cuhre(self, *args, **kwargs):
        self.integrator = Cuhre(self,*args,**kwargs)
