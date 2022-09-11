
#ifndef nr_double_t
/* The global type of double representation. */
#cmakedefine nr_double_t @QUCS_DOUBLE_TYPE@
#endif

/* The size of the double representation. */
#ifndef NR_DOUBLE_SIZE
#cmakedefine NR_DOUBLE_SIZE @QUCS_DOUBLE_SIZE@
#endif

#ifndef nr_int16_t
/* C-type for 16-bit integers. */
#cmakedefine nr_int16_t @QUCS_INT16_TYPE@
#endif

#ifndef nr_int32_t
/* C-type for 32-bit integers. */
#cmakedefine nr_int32_t @QUCS_INT32_TYPE@
#endif
