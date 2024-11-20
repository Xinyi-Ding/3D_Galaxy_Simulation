#ifndef TIMER_H
#define TIMER_H

void StartTimer();
double GetTimer();

#endif // TIMER_H

// #ifndef TIMER_H
// #define TIMER_H

// #include <stdlib.h>

// #ifdef WIN32
//   #define WIN32_LEAN_AND_MEAN
//   #include <windows.h>
// #else
//   #ifndef __USE_BSD
//     #define __USE_BSD
//   #endif
//   #include <sys/time.h>
// #endif

// #ifdef WIN32
// double PCFreq = 0.0;
// __int64 timerStart = 0;
// #else
// struct timeval timerStart;
// #endif

// void StartTimer()
// {
// #ifdef WIN32
//   LARGE_INTEGER li;
//   if(!QueryPerformanceFrequency(&li))
//     printf("QueryPerformanceFrequency failed!\n");

//   PCFreq = (double)li.QuadPart/1000.0;

//   QueryPerformanceCounter(&li);
//   timerStart = li.QuadPart;
// #else
//   gettimeofday(&timerStart, NULL);
// #endif
// }

// // time elapsed in ms
// double GetTimer()
// {
// #ifdef WIN32
//   LARGE_INTEGER li;
//   QueryPerformanceCounter(&li);
//   return (double)(li.QuadPart-timerStart)/PCFreq;
// #else
//   struct timeval timerStop, timerElapsed;
//   gettimeofday(&timerStop, NULL);
//   timersub(&timerStop, &timerStart, &timerElapsed);
//     return timerElapsed.tv_sec*1000.0+timerElapsed.tv_usec/1000.0;
// #endif
// }

// #endif // TIMER_H

// #ifndef TIMER_H
// #define TIMER_H

// #include <stdlib.h>

// #ifdef WIN32
//   #define WIN32_LEAN_AND_MEAN
//   #include <windows.h>
// #else
//   #ifndef __USE_BSD
//     #define __USE_BSD
//   #endif
//   #include <sys/time.h>
// #endif

// #ifdef WIN32
// double PCFreq = 0.0;
// __int64 timerStart = 0;
// #else
// struct timeval timerStart;
// #endif

// void StartTimer()
// {
// #ifdef WIN32
//   LARGE_INTEGER li;
//   if(!QueryPerformanceFrequency(&li))
//     printf("QueryPerformanceFrequency failed!\n");

//   PCFreq = (double)li.QuadPart/1000.0;

//   QueryPerformanceCounter(&li);
//   timerStart = li.QuadPart;
// #else
//   gettimeofday(&timerStart, NULL);
// #endif
// }

// // time elapsed in ms
// double GetTimer()
// {
// #ifdef WIN32
//   LARGE_INTEGER li;
//   QueryPerformanceCounter(&li);
//   return (double)(li.QuadPart-timerStart)/PCFreq;
// #else
//   struct timeval timerStop, timerElapsed;
//   gettimeofday(&timerStop, NULL);
  
//   // Define timersub if not available
//   #ifndef timersub
//   #define timersub(a, b, result)                      \
//     do {                                              \
//       (result)->tv_sec = (a)->tv_sec - (b)->tv_sec;   \
//       (result)->tv_usec = (a)->tv_usec - (b)->tv_usec;\
//       if ((result)->tv_usec < 0) {                    \
//         --(result)->tv_sec;                           \
//         (result)->tv_usec += 1000000;                 \
//       }                                               \
//     } while (0)
//   #endif

//   timersub(&timerStop, &timerStart, &timerElapsed);
//   return timerElapsed.tv_sec*1000.0+timerElapsed.tv_usec/1000.0;
// #endif
// }

// #endif // TIMER_H