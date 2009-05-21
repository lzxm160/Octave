/*

Copyright (C) 2008 Michael Goffioul

This file is part of Octave.

Octave is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 3 of the License, or (at your
option) any later version.

Octave is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with Octave; see the file COPYING.  If not, see
<http://www.gnu.org/licenses/>.

*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "oct-mutex.h"
#include "lo-error.h"

#if defined (__WIN32__) && ! defined (__CYGWIN__)
#include <windows.h>
#elif defined (HAVE_PTHREAD_H)
#include <pthread.h>
#endif

void
octave_base_mutex::lock (void)
{
  (*current_liboctave_error_handler) ("mutex not supported on this platform");
}

void
octave_base_mutex::unlock (void)
{
  (*current_liboctave_error_handler) ("mutex not supported on this platform");
}

#if defined (__WIN32__) && ! defined (__CYGWIN__)

class
octave_w32_mutex : public octave_base_mutex
{
public:
  octave_w32_mutex (void)
    : octave_base_mutex ()
  {
    InitializeCriticalSection (&cs);
  }

  ~octave_w32_mutex (void)
  {
    DeleteCriticalSection (&cs);
  }

  void lock (void)
  {
    EnterCriticalSection (&cs);
  }

  void unlock (void)
  {
    LeaveCriticalSection (&cs);
  }

private:
  CRITICAL_SECTION cs;
};

#elif defined (HAVE_PTHREAD_H)

class
octave_pthread_mutex : public octave_base_mutex
{
public:
  octave_pthread_mutex (void)
    : octave_base_mutex ()
  {
    pthread_mutexattr_t attr;

    pthread_mutexattr_init (&attr);
    pthread_mutexattr_settype (&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init (&pm, &attr);
    pthread_mutexattr_destroy (&attr);
  }

  ~octave_pthread_mutex (void)
  {
    pthread_mutex_destroy (&pm);
  }

  void lock (void)
  {
    pthread_mutex_lock (&pm);
  }

  void unlock (void)
  {
    pthread_mutex_unlock (&pm);
  }

private:
  pthread_mutex_t pm;
};

#endif

octave_mutex::octave_mutex (void)
{
#if defined (__WIN32__) && ! defined (__CYGWIN__)
  rep = new octave_w32_mutex ();
#elif defined (HAVE_PTHREAD_H)
  rep = new octave_pthread_mutex ();
#else
  rep = new octave_base_mutex ();
#endif
}
