/*
    Copyright 2005-2011 Intel Corporation.  All Rights Reserved.

    This file is part of Threading Building Blocks.

    Threading Building Blocks is free software; you can redistribute it
    and/or modify it under the terms of the GNU General Public License
    version 2 as published by the Free Software Foundation.

    Threading Building Blocks is distributed in the hope that it will be
    useful, but WITHOUT ANY WARRANTY; without even the implied warranty
    of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Threading Building Blocks; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/

#ifndef _GMTL_STOP_WATCH_H_
#define _GMTL_STOP_WATCH_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif

#include <gmtl/gmtl_stddef.h>
#include <windows.h>

#if _WIN32||_WIN64
#include <gmtl/machine/windows_api.h>
#include <time.h>
#elif __linux__
#include <ctime>
#else /* generic Unix */
#include <sys/time.h>
#endif /* (choice of OS) */

namespace gmtl {

//! Absolute timestamp
/** @ingroup timing */
class stop_watch
{
public:
    //! Construct an absolute timestamp initialized to zero.
    stop_watch() : startTime(0), endTime(0), elapsedTime(0), elapsedTimeTotal(0), bIsRunning(false) {};
    stop_watch(const stop_watch &src);

    stop_watch &operator =(const stop_watch &t);

    bool    isRunning(void);

    void    reset(void);

    //! restart() is equivalent to reset() and begin()
    void    restart(void);
    void    reset_and_begin(void);

    void    start(void);
    void    stop(void);

    void    begin(void);
    void    end(void);

    //! Return current time.
    static int64_t  now(void);

    //! Return current time(double).
    static double   nowf(void);

    static double   intervalSeconds(int64_t t1, int64_t t2);
    static double   intervalSeconds(double t1, double t2);

    //! Return current time(millisecs).
    static int64_t  currentTimeMillis(void);
    static double   currentTimeMillisf(void);

    double getSeconds(void);
    double getMillisec(void);

    double getTotalSeconds(void);
    double getTotalMillisec(void);

    double getUsedTime(void);
    double getUsedTimeTotal(void);

private:
    int64_t startTime, endTime;
    int64_t elapsedTime;
    int64_t elapsedTimeTotal;
    bool    bIsRunning;
};

stop_watch::stop_watch(const stop_watch &src)
{
    startTime           = src.startTime;
    endTime             = src.endTime;
    elapsedTime         = src.elapsedTime;
    elapsedTimeTotal    = src.elapsedTimeTotal;
    bIsRunning          = src.bIsRunning;
}

///*
inline stop_watch &stop_watch::operator =(const stop_watch &t)
{
    startTime           = t.startTime;
    endTime             = t.endTime;
    elapsedTime         = t.elapsedTime;
    elapsedTimeTotal    = t.elapsedTimeTotal;
    bIsRunning          = t.bIsRunning;
    return *this;
}
//*/

inline bool stop_watch::isRunning(void)
{
    return bIsRunning;
}

inline void stop_watch::reset(void)
{
    elapsedTime = 0;
    elapsedTimeTotal = 0;
    bIsRunning = false;
}

//! restart() is equivalent to reset() and begin()
inline void stop_watch::restart(void)
{
    elapsedTime = 0;
    elapsedTimeTotal = 0;

#if _WIN32 || _WIN64
    LARGE_INTEGER qp_cnt;
    QueryPerformanceCounter(&qp_cnt);
    startTime = (int64_t)qp_cnt.QuadPart;
    endTime   = startTime;
#elif __linux__
    struct timespec ts;
#if GMTL_USE_ASSERT
    int status =
#endif /* GMTL_USE_ASSERT */
        clock_gettime(CLOCK_REALTIME, &ts);
    _GMTL_ASSERT(status == 0, "CLOCK_REALTIME not supported");
    startTime = static_cast<int64_t>(1000000000UL) * static_cast<int64_t>(ts.tv_sec) + static_cast<int64_t>(ts.tv_nsec);
    endTime   = startTime;
#else /* generic Unix */
    struct timeval tv;
#if GMTL_USE_ASSERT
    int status =
#endif /* GMTL_USE_ASSERT */
        gettimeofday(&tv, NULL);
    _GMTL_ASSERT(status == 0, "gettimeofday failed");
    startTime = static_cast<int64_t>(1000000) * static_cast<int64_t>(tv.tv_sec) + static_cast<int64_t>(tv.tv_usec);
    endTime   = startTime;
#endif /*(choice of OS) */

    bIsRunning = true;
}

inline void stop_watch::reset_and_begin(void)
{
    restart();
}

inline void stop_watch::start(void)
{
#if _WIN32 || _WIN64
    LARGE_INTEGER qp_cnt;
    QueryPerformanceCounter(&qp_cnt);
    startTime = (int64_t)qp_cnt.QuadPart;
    endTime   = startTime;
#elif __linux__
    struct timespec ts;
#if GMTL_USE_ASSERT
    int status =
#endif /* GMTL_USE_ASSERT */
        clock_gettime(CLOCK_REALTIME, &ts);
    _GMTL_ASSERT(status == 0, "CLOCK_REALTIME not supported");
    startTime = static_cast<int64_t>(1000000000UL) * static_cast<int64_t>(ts.tv_sec) + static_cast<int64_t>(ts.tv_nsec);
    endTime   = startTime;
#else /* generic Unix */
    struct timeval tv;
#if GMTL_USE_ASSERT
    int status =
#endif /* GMTL_USE_ASSERT */
        gettimeofday(&tv, NULL);
    _GMTL_ASSERT(status == 0, "gettimeofday failed");
    startTime = static_cast<int64_t>(1000000) * static_cast<int64_t>(tv.tv_sec) + static_cast<int64_t>(tv.tv_usec);
    endTime   = startTime;
#endif /*(choice of OS) */

    bIsRunning = true;
}

inline void stop_watch::begin(void)
{
    start();
}

inline void stop_watch::stop(void)
{
#if _WIN32 || _WIN64
    LARGE_INTEGER qp_cnt;
    QueryPerformanceCounter(&qp_cnt);
    endTime = (int64_t)qp_cnt.QuadPart;
#elif __linux__
    struct timespec ts;
#if GMTL_USE_ASSERT
    int status =
#endif /* GMTL_USE_ASSERT */
        clock_gettime(CLOCK_REALTIME, &ts);
    _GMTL_ASSERT(status == 0, "CLOCK_REALTIME not supported");
    endTime = static_cast<int64_t>(1000000000UL) * static_cast<int64_t>(ts.tv_sec) + static_cast<int64_t>(ts.tv_nsec);
#else /* generic Unix */
    struct timeval tv;
#if GMTL_USE_ASSERT
    int status =
#endif /* GMTL_USE_ASSERT */
        gettimeofday(&tv, NULL);
    _GMTL_ASSERT(status == 0, "gettimeofday failed");
    endTime = static_cast<int64_t>(1000000) * static_cast<int64_t>(tv.tv_sec) + static_cast<int64_t>(tv.tv_usec);
#endif /*(choice of OS) */

    if (bIsRunning) {
        elapsedTime = endTime - startTime;
        if (elapsedTime >= 0)
            elapsedTimeTotal += elapsedTime;
        else
            elapsedTimeTotal -= elapsedTime;
    }

    bIsRunning = false;
}

inline void stop_watch::end(void)
{
    stop();
}

inline int64_t stop_watch::now(void)
{
    int64_t result;

#if _WIN32 || _WIN64
    LARGE_INTEGER qp_cnt, qp_freq;
    QueryPerformanceCounter(&qp_cnt);
    QueryPerformanceFrequency(&qp_freq);
    result = static_cast<int64_t>(((double)qp_cnt.QuadPart / (double)qp_freq.QuadPart) * 1000000000.0);
#elif __linux__
    struct timespec ts;
#if GMTL_USE_ASSERT
    int status =
#endif /* GMTL_USE_ASSERT */
        clock_gettime(CLOCK_REALTIME, &ts);
    _GMTL_ASSERT(status == 0, "CLOCK_REALTIME not supported");
    result = static_cast<int64_t>(1000000000UL) * static_cast<int64_t>(ts.tv_sec) + static_cast<int64_t>(ts.tv_nsec);
#else /* generic Unix */
    struct timeval tv;
#if GMTL_USE_ASSERT
    int status =
#endif /* GMTL_USE_ASSERT */
        gettimeofday(&tv, NULL);
    _GMTL_ASSERT(status == 0, "gettimeofday failed");
    result = static_cast<int64_t>(1000000000UL) * static_cast<int64_t>(tv.tv_sec) + static_cast<int64_t>(1000UL) * static_cast<int64_t>(tv.tv_usec);
#endif /*(choice of OS) */

    return result;
}

inline double stop_watch::nowf(void)
{
    double result;

#if _WIN32 || _WIN64
    LARGE_INTEGER qp_cnt, qp_freq;
    QueryPerformanceCounter(&qp_cnt);
    QueryPerformanceFrequency(&qp_freq);
    result = (double)qp_cnt.QuadPart / (double)qp_freq.QuadPart;
#elif __linux__
    int64_t time_usecs;
    struct timespec ts;
#if GMTL_USE_ASSERT
    int status =
#endif /* GMTL_USE_ASSERT */
        clock_gettime(CLOCK_REALTIME, &ts);
    _GMTL_ASSERT(status == 0, "CLOCK_REALTIME not supported");
    time_usecs = static_cast<int64_t>(1000000000UL) * static_cast<int64_t>(ts.tv_sec) + static_cast<int64_t>(ts.tv_nsec);
    result = (double)time_usecs * 1E-9
#else /* generic Unix */
    int64_t time_usecs;
    struct timeval tv;
#if GMTL_USE_ASSERT
    int status =
#endif /* GMTL_USE_ASSERT */
        gettimeofday(&tv, NULL);
    _GMTL_ASSERT(status == 0, "gettimeofday failed");
    time_usecs = static_cast<int64_t>(1000000UL) * static_cast<int64_t>(tv.tv_sec) + static_cast<int64_t>(tv.tv_usec);
    result = (double)time_usecs * 1E-6;
#endif /*(choice of OS) */

    return result;
}

inline double stop_watch::intervalSeconds(int64_t t1, int64_t t2)
{
    double seconds = (double)(t2 - t1) * 1E-9;
    return seconds;
}

inline double stop_watch::intervalSeconds(double t1, double t2)
{
    double seconds = (double)(t2 - t1);
    return seconds;
}

inline int64_t stop_watch::currentTimeMillis(void)
{
    int64_t now_usecs = (int64_t)stop_watch::now();
    return now_usecs / static_cast<int64_t>(1000UL);
}

inline double stop_watch::currentTimeMillisf(void)
{
    double now_usecs = (double)stop_watch::nowf();
    return now_usecs * 1E-3;
}

inline double stop_watch::getSeconds(void)
{
    if (bIsRunning)
        stop();

#if _WIN32 || _WIN64
    LARGE_INTEGER qp_freq;
    QueryPerformanceFrequency(&qp_freq);
    return (double)elapsedTime / (double)qp_freq.QuadPart;
#elif __linux__
    return elapsedTime * 1E-9;
#else /* generic Unix */
    return elapsedTime * 1E-6;
#endif /* (choice of OS) */
}

inline double stop_watch::getMillisec(void)
{
    if (bIsRunning)
        stop();

#if _WIN32 || _WIN64
    LARGE_INTEGER qp_freq;
    QueryPerformanceFrequency(&qp_freq);
    return ((double)elapsedTime / (double)qp_freq.QuadPart) * 1E3;
#elif __linux__
    return elapsedTime * 1E-6;
#else /* generic Unix */
    return elapsedTime * 1E-3;
#endif /* (choice of OS) */
}

inline double stop_watch::getTotalSeconds(void)
{
    if (bIsRunning)
        stop();

#if _WIN32 || _WIN64
    LARGE_INTEGER qp_freq;
    QueryPerformanceFrequency(&qp_freq);
    return (double)elapsedTimeTotal / (double)qp_freq.QuadPart;
#elif __linux__
    return elapsedTimeTotal * 1E-9;
#else /* generic Unix */
    return elapsedTimeTotal * 1E-6;
#endif /* (choice of OS) */
}

inline double stop_watch::getTotalMillisec(void)
{
    if (bIsRunning)
        stop();

#if _WIN32 || _WIN64
    LARGE_INTEGER qp_freq;
    QueryPerformanceFrequency(&qp_freq);
    return ((double)elapsedTimeTotal / (double)qp_freq.QuadPart) * 1E3;
#elif __linux__
    return elapsedTimeTotal * 1E-6;
#else /* generic Unix */
    return elapsedTimeTotal * 1E-3;
#endif /* (choice of OS) */
}

inline double stop_watch::getUsedTime(void)
{
    return getSeconds();
}

inline double stop_watch::getUsedTimeTotal(void)
{
    return getTotalSeconds();
}

}  // namespace gmtl

#endif  /* _GMTL_STOP_WATCH_H_ */
