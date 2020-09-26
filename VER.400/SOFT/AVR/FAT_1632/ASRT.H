
#ifndef __ASRT
  #define __ASRT


#ifdef DEBUG
    #define ASSERT(cond)                        \
    {                                           \
      if ( !(cond) )                            \
      {                                         \
        Printf("\nASR %s %d\n" #cond, __FILE__, (int) __LINE__ );\
        while (1);                              \
      }                                         \
    }
#else
  #define ASSERT(cond)
#endif

#endif /* __ASRT */
