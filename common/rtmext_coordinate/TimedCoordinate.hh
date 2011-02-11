// This file is generated by omniidl (C++ backend)- omniORB_4_1. Do not edit.
#ifndef __TimedCoordinate_hh__
#define __TimedCoordinate_hh__

#ifndef __CORBA_H_EXTERNAL_GUARD__
#include <omniORB4/CORBA.h>
#endif

#ifndef  USE_stub_in_nt_dll
# define USE_stub_in_nt_dll_NOT_DEFINED_TimedCoordinate
#endif
#ifndef  USE_core_stub_in_nt_dll
# define USE_core_stub_in_nt_dll_NOT_DEFINED_TimedCoordinate
#endif
#ifndef  USE_dyn_stub_in_nt_dll
# define USE_dyn_stub_in_nt_dll_NOT_DEFINED_TimedCoordinate
#endif



#ifndef __BasicDataType_hh_EXTERNAL_GUARD__
#define __BasicDataType_hh_EXTERNAL_GUARD__
#include <BasicDataType.hh>
#endif



#ifdef USE_stub_in_nt_dll
# ifndef USE_core_stub_in_nt_dll
#  define USE_core_stub_in_nt_dll
# endif
# ifndef USE_dyn_stub_in_nt_dll
#  define USE_dyn_stub_in_nt_dll
# endif
#endif

#ifdef _core_attr
# error "A local CPP macro _core_attr has already been defined."
#else
# ifdef  USE_core_stub_in_nt_dll
#  define _core_attr _OMNIORB_NTDLL_IMPORT
# else
#  define _core_attr
# endif
#endif

#ifdef _dyn_attr
# error "A local CPP macro _dyn_attr has already been defined."
#else
# ifdef  USE_dyn_stub_in_nt_dll
#  define _dyn_attr _OMNIORB_NTDLL_IMPORT
# else
#  define _dyn_attr
# endif
#endif





_CORBA_MODULE RTC

_CORBA_MODULE_BEG

  struct TimedCoordinate {
    typedef _CORBA_ConstrType_Variable_Var<TimedCoordinate> _var_type;

    
    Time tm;

    ::CORBA::String_member parent_id;

    ::CORBA::String_member frame_id;

    typedef _CORBA_Unbounded_Sequence_w_FixSizeElement< ::CORBA::Float, 4, 4 >  _transform_seq;
    _transform_seq transform;

  

    void operator>>= (cdrStream &) const;
    void operator<<= (cdrStream &);
  };

  typedef TimedCoordinate::_var_type TimedCoordinate_var;

  typedef _CORBA_ConstrType_Variable_OUT_arg< TimedCoordinate,TimedCoordinate_var > TimedCoordinate_out;

  _CORBA_MODULE_VAR _dyn_attr const ::CORBA::TypeCode_ptr _tc_TimedCoordinate;

_CORBA_MODULE_END



_CORBA_MODULE POA_RTC
_CORBA_MODULE_BEG

_CORBA_MODULE_END



_CORBA_MODULE OBV_RTC
_CORBA_MODULE_BEG

_CORBA_MODULE_END





#undef _core_attr
#undef _dyn_attr

extern void operator<<=(::CORBA::Any& _a, const RTC::TimedCoordinate& _s);
extern void operator<<=(::CORBA::Any& _a, RTC::TimedCoordinate* _sp);
extern _CORBA_Boolean operator>>=(const ::CORBA::Any& _a, RTC::TimedCoordinate*& _sp);
extern _CORBA_Boolean operator>>=(const ::CORBA::Any& _a, const RTC::TimedCoordinate*& _sp);





#ifdef   USE_stub_in_nt_dll_NOT_DEFINED_TimedCoordinate
# undef  USE_stub_in_nt_dll
# undef  USE_stub_in_nt_dll_NOT_DEFINED_TimedCoordinate
#endif
#ifdef   USE_core_stub_in_nt_dll_NOT_DEFINED_TimedCoordinate
# undef  USE_core_stub_in_nt_dll
# undef  USE_core_stub_in_nt_dll_NOT_DEFINED_TimedCoordinate
#endif
#ifdef   USE_dyn_stub_in_nt_dll_NOT_DEFINED_TimedCoordinate
# undef  USE_dyn_stub_in_nt_dll
# undef  USE_dyn_stub_in_nt_dll_NOT_DEFINED_TimedCoordinate
#endif

#endif  // __TimedCoordinate_hh__

