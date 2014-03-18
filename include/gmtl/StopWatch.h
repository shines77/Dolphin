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

#ifndef _GMTL_STOPWATCH_H_
#define _GMTL_STOPWATCH_H_

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
class StopWatch
{
public:
    typedef uint32_t timestamp_t;

public:
    //! Construct an absolute timestamp initialized to zero.
    StopWatch() : startTime(0), stopTime(0), elapsedTime(0), elapsedTimeTotal(0), bIsRunning(false) {};
    StopWatch(const StopWatch &src);

    StopWatch &operator =(const StopWatch &t);

    bool    isRunning(void);

    void    clear(void);
    void    reset(void);

    //! restart() is equivalent to reset() and begin()
    void    restart(void);
    void    reset_and_begin(void);

    void    start(void);
    void    stop(void);

    void    begin(void);
    void    end(void);

    //! Return current time.
    static timestamp_t  now(void);

    //! Return current time(double).
    static double       nowf(void);

    static double       intervalSeconds(uint32_t t1, uint32_t t2);
    static double       intervalSeconds(double t1, double t2);

    //! Return current time(millisecs).
    static timestamp_t  currentTimeMillis(void);
    static double       currentTimeMillisf(void);

    double getSeconds(void);
    double getMillisec(void);

    double getTotalSeconds(void);
    double getTotalMillisec(void);

    double getUsedTime(void);
    double getUsedTimeTotal(void);

protected:
    void    native_start();
    void    native_stop();

    static timestamp_t  native_now();
    static double       native_nowf();

private:
    timestamp_t startTime, stopTime;
    timestamp_t elapsedTime;
    timestamp_t elapsedTimeTotal;
    bool        bIsRunning;
};

inline StopWatch::StopWatch(const StopWatch &src)
{
    startTime           = src.startTime;
    stopTime            = src.stopTime;
    elapsedTime         = src.elapsedTime;
    elapsedTimeTotal    = src.elapsedTimeTotal;
    bIsRunning          = src.bIsRunning;
}

///*
inline StopWatch &StopWatch::operator =(const StopWatch &t)
{
    startTime           = t.startTime;
    stopTime            = t.stopTime;
    elapsedTime         = t.elapsedTime;
    elapsedTimeTotal    = t.elapsedTimeTotal;
    bIsRunning          = t.bIsRunning;
    return *this;
}
//*/

inline void StopWatch::native_start()
{
#if _WIN32 || _WIN64
    startTime = static_cast<timestamp_t>(GetTickCount());
    stopTime  = startTime;
#elif __linux__
    struct timespec ts;
#if GMTL_USE_ASSERT
    int status =
#endif /* GMTL_USE_ASSERT */
        clock_gettime(CLOCK_REALTIME, &ts);
    _GMTL_ASSERT(status == 0, "CLOCK_REALTIME not supported");
    startTime = static_cast<timestamp_t>(static_cast<int64_t>(1000UL) * static_cast<int64_t>(ts.tv_sec) + static_cast<int64_t>(ts.tv_nsec) / static_cast<int64_t>(1000000UL));
    stopTime  = startTime;
#else /* generic Unix */
    struct timeval tv;
#if GMTL_USE_ASSERT
    int status =
#endif /* GMTL_USE_ASSERT */
        gettimeofday(&tv, NULL);
    _GMTL_ASSERT(status == 0, "gettimeofday failed");
    startTime = static_cast<timestamp_t>(static_cast<int64_t>(1000UL) * static_cast<int64_t>(tv.tv_sec) + static_cast<int64_t>(tv.tv_usec) / static_cast<int64_t>(1000UL));
    stopTime  = startTime;
#endif /*(choice of OS) */
}

inline void StopWatch::native_stop()
{
#if _WIN32 || _WIN64
    stopTime = (timestamp_t)GetTickCount();
#elif __linux__
    struct timespec ts;
#if GMTL_USE_ASSERT
    int status =
#endif /* GMTL_USE_ASSERT */
        clock_gettime(CLOCK_REALTIME, &ts);
    _GMTL_ASSERT(status == 0, "CLOCK_REALTIME not supported")timestamp_t
    stopTime = static_cast<timestamp_t>(static_cast<int64_t>(1000UL) * static_cast<int64_t>(ts.tv_sec) + static_cast<int64_t>(ts.tv_nsec) / static_cast<int64_t>(1000000UL));
#else /* generic Unix */
    struct timeval tv;
#if GMTL_USE_ASSERT
    int status =
#endif /* GMTL_USE_ASSERT */
        gettimeofday(&tv, NULL);
    _GMTL_ASSERT(status == 0, "gettimeofday failed");
    stopTime = static_cast<timestamp_t>(static_cast<int64_t>(1000UL) * static_cast<int64_t>(tv.tv_sec) + static_cast<int64_t>(tv.tv_usec) / static_cast<int64_t>(1000UL));
#endif /*(choice of OS) */
}

inline StopWatch::timestamp_t StopWatch::native_now()
{
    timestamp_t result;

#if _WIN32 || _WIN64
    result = static_cast<timestamp_t>(GetTickCount());
#elif __linux__
    struct timespec ts;
#if GMTL_USE_ASSERT
    int status =
#endif /* GMTL_USE_ASSERT */
        clock_gettime(CLOCK_REALTIME, &ts);
    _GMTL_ASSERT(status == 0, "CLOCK_REALTIME not supported");
    result = static_cast<timestamp_t>(static_cast<int64_t>(1000UL) * static_cast<int64_t>(ts.tv_sec) + static_cast<int64_t>(ts.tv_nsec) / static_cast<int64_t>(1000000UL));
#else /* generic Unix */
    struct timeval tv;
#if GMTL_USE_ASSERT
    int status =
#endif /* GMTL_USE_ASSERT */
        gettimeofday(&tv, NULL);
    _GMTL_ASSERT(status == 0, "gettimeofday failed");
    result = static_cast<timestamp_t>(static_cast<int64_t>(1000UL) * static_cast<int64_t>(tv.tv_sec) + static_cast<int64_t>(tv.tv_usec) / static_cast<int64_t>(1000UL));
#endif /*(choice of OS) */

    return result;
}

inline double StopWatch::native_nowf()
{
    double result;

#if _WIN32 || _WIN64
    result = (double)GetTickCount();
#elif __linux__
    int64_t time_usecs;
    struct timespec ts;
#if GMTL_USE_ASSERT
    int status =
#endif /* GMTL_USE_ASSERT */
        clock_gettime(CLOCK_REALTIME, &ts);
    _GMTL_ASSERT(status == 0, "CLOCK_REALTIME not supported");
    time_usecs = static_cast<int64_t>(1000000000UL) * static_cast<int64_t>(ts.tv_sec) + static_cast<int64_t>(ts.tv_nsec);
    result = (double)time_usecs * 1E-6
#else /* generic Unix */
    int64_t time_usecs;
    struct timeval tv;
#if GMTL_USE_ASSERT
    int status =
#endif /* GMTL_USE_ASSERT */
        gettimeofday(&tv, NULL);
    _GMTL_ASSERT(status == 0, "gettimeofday failed");
    time_usecs = static_cast<int64_t>(1000000UL) * static_cast<int64_t>(tv.tv_sec) + static_cast<int64_t>(tv.tv_usec);
    result = (double)time_usecs * 1E-3;
#endif /*(choice of OS) */

    return result;
}

inline bool StopWatch::isRunning(void)
{
    return bIsRunning;
}

inline void StopWatch::clear(void)
{
    elapsedTime = 0;
    elapsedTimeTotal = 0;

    native_start();

    bIsRunning = false;
}

inline void StopWatch::reset(void)
{
    elapsedTime = 0;

    native_start();

    bIsRunning = false;
}

//! restart() is equivalent to reset() and begin()
inline void StopWatch::restart(void)
{
    elapsedTime = 0;

    native_start();

    bIsRunning = true;
}

inline void StopWatch::reset_and_begin(void)
{
    restart();
}

inline void StopWatch::start(void)
{
    native_start();

    bIsRunning = true;
}

inline void StopWatch::begin(void)
{
    start();
}

inline void StopWatch::stop(void)
{
    native_stop();

    if (bIsRunning) {
        elapsedTime = stopTime - startTime;
        if (elapsedTime >= 0)
            elapsedTimeTotal += elapsedTime;
        else
            elapsedTimeTotal -= elapsedTime;
    }

    bIsRunning = false;
}

inline void StopWatch::end(void)
{
    stop();
}

inline StopWatch::timestamp_t StopWatch::now(void)
{
    return native_now();
}

inline double StopWatch::nowf(void)
{
    return native_nowf();
}

inline double StopWatch::intervalSeconds(uint32_t t1, uint32_t t2)
{
    double seconds = (double)(t2 - t1) * 1E-3;
    return seconds;
}

inline double StopWatch::intervalSeconds(double t1, double t2)
{
    double seconds = (double)(t2 - t1);
    return seconds;
}

inline uint32 StopWatch::currentTimeMillis(void)
{
    uint32 now_usecs = StopWatch::native_now();
    return now_usecs;
}

inline double StopWatch::currentTimeMillisf(void)
{
    double now_usecs = StopWatch::native_nowf();
    return now_usecs;
}

inline double StopWatch::getSeconds(void)
{
    if (bIsRunning)
        stop();

#if _WIN32 || _WIN64
    return (double)elapsedTime * 1E-3;
#elif __linux__
    return (double)elapsedTime * 1E-3;
#else /* generic Unix */
    return (double)elapsedTime * 1E-3;
#endif /* (choice of OS) */
}

inline double StopWatch::getMillisec(void)
{
    if (bIsRunning)
        stop();

#if _WIN32 || _WIN64
    return (double)elapsedTime;
#elif __linux__
    return (double)elapsedTime;
#else /* generic Unix */
    return (double)elapsedTime;
#endif /* (choice of OS) */
}

inline double StopWatch::getTotalSeconds(void)
{
    if (bIsRunning)
        stop();

#if _WIN32 || _WIN64
    return (double)elapsedTimeTotal * 1E-3;
#elif __linux__
    return (double)elapsedTimeTotal * 1E-3;
#else /* generic Unix */
    return (double)elapsedTimeTotal * 1E-3;
#endif /* (choice of OS) */
}

inline double StopWatch::getTotalMillisec(void)
{
    if (bIsRunning)
        stop();

#if _WIN32 || _WIN64
    return (double)elapsedTime;
#elif __linux__
    return (double)elapsedTime;
#else /* generic Unix */
    return (double)elapsedTime;
#endif /* (choice of OS) */
}

inline double StopWatch::getUsedTime(void)
{
    return getSeconds();
}

inline double StopWatch::getUsedTimeTotal(void)
{
    return getTotalSeconds();
}

}  // namespace gmtl

#endif  /* _GMTL_STOPWATCH_H_ */
