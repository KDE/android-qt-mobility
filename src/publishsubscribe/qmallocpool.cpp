/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qmallocpool_p.h"
#include <qglobal.h>

struct malloc_state;

QTM_BEGIN_NAMESPACE

static void* qmallocpool_sbrk(intptr_t increment);

#define USE_DL_PREFIX
#define MORECORE QTM_NAMESPACE::qmallocpool_sbrk
#define HAVE_MMAP 0
#define __STD_C 1
#ifdef Q_OS_WINCE
#define WIN32
#define LACKS_ERRNO_H
#define MALLOC_FAILURE_ACTION
#endif

static QMallocPoolPrivate * qmallocpool_instance = 0;
static struct malloc_state * qmallocpool_state(QMallocPoolPrivate *);
#define get_malloc_state() (QTM_NAMESPACE::qmallocpool_state(QTM_NAMESPACE::qmallocpool_instance))

QTM_END_NAMESPACE

#include "dlmalloc.c"

QTM_BEGIN_NAMESPACE

class QMallocPoolPrivate
{
public:
    QMallocPoolPrivate(void * _pool, unsigned int _poolLen,
                       QMallocPool::PoolType type, const QString &_name)
        : name(_name), pool((char *)_pool), poolLength(_poolLen), poolPtr(0)
    {
        Q_ASSERT(pool);
        Q_ASSERT(poolLength > 0);

        if(QMallocPool::Owned == type) {
            qMemSet(&owned_mstate, 0, sizeof(struct malloc_state));
            mstate = &owned_mstate;
        } else if(QMallocPool::NewShared == type) {
            Q_ASSERT(poolLength >= sizeof(struct malloc_state));
            qMemSet(pool, 0, sizeof(struct malloc_state));
            mstate = (struct malloc_state *)pool;
            pool += sizeof(struct malloc_state);
            poolLength -= sizeof(struct malloc_state);
        } else if(QMallocPool::Shared == type) {
            Q_ASSERT(poolLength >= sizeof(struct malloc_state));
            mstate = (struct malloc_state *)pool;
            pool += sizeof(struct malloc_state);
            poolLength -= sizeof(struct malloc_state);
        }
    }

    QString name;
    char * pool;
    unsigned int poolLength;
    unsigned int poolPtr;
    struct malloc_state *mstate;
    struct malloc_state owned_mstate;
};

static struct malloc_state * qmallocpool_state(QMallocPoolPrivate *d)
{
    Q_ASSERT(d);
    return d->mstate;
}

static void* qmallocpool_sbrk(intptr_t increment)
{
    Q_ASSERT(qmallocpool_instance);
    QMallocPoolPrivate * const d = qmallocpool_instance;

    if(increment > 0) {
        if((unsigned)increment > d->poolLength ||
           (d->poolLength - increment) < d->poolPtr)
            // Failure
            return (void *)MORECORE_FAILURE;

        void * rv = (void *)(d->pool + d->poolPtr);
        d->poolPtr += increment;
        return rv;

    } else /* increment <= 0 */ {
        Q_ASSERT(d->poolPtr >= (unsigned)(-1 * increment));
        d->poolPtr += increment;
        return (void *)(d->pool + d->poolPtr);
    }
}

struct QMallocPtr
{
    QMallocPtr(QMallocPoolPrivate * d)
    {
        Q_ASSERT(!qmallocpool_instance);
        qmallocpool_instance = d;
    }
    ~QMallocPtr()
    {
        Q_ASSERT(qmallocpool_instance);
        qmallocpool_instance = 0;
    }
};

/*!
  \class QMallocPool
  \internal
  \ingroup publishsubscribe

  \brief The QMallocPool class allows management of allocations within a
  designated memory region.

  QMallocPool provides heap management capabilities into a fixed region of
  memory.  Primarily this is useful for managing allocations in a shared
  memory region, but could be used in other scenarios.

  The QMallocPool class provides equivalents for most standard memory management
  functions, such as \c {malloc}, \c {calloc}, \c {realloc} and \c {free}.
  However, unlike these standard functions which acquire their memory from the
  system kernel, QMallocPool operators on a region of memory provided to it
  during construction.

  QMallocPool is based on dlmalloc, a public domain malloc implementation
  written by Doug Lea.  dlmalloc is used as the default allocator in many
  projects, including several versions of Linux libc.

  QMallocPool is not thread safe.
*/

/*!
  \enum QMallocPool::PoolType

  Controls the type of pool to be created.  In order to manage memory, a small
  amount of book keeping information is maintained.  While this information is
  not required for reading from the managed pool, it is required for
  allocations.  The PoolType controls where this bookkeeping data is stored.

  \value Owned
  The bookkeeping data is maintained in the QMallocPool instance.  Allocation to
  the pool is only possible via this instance.
  \value NewShared
  The bookkeeping data is maintained in the managed region itself.  This allows
  multiple QMallocPool instances, possibly in separate processes, to allocate
  from the pool.

  The NewShared PoolType also initializes this bookkeeping data to its default
  state.  Thus, while the bookkeeping data is shared, only one of the sharing
  instances should use a NewShared type.  All other instances should use the
  Shared pool type.

  The malloc pool bookkeeping data contains absolute pointers.  As such, if
  multiple processes intend to allocate into the malloc pool, is is essential 
  that they map the memory region to the same virtual address location.
  \value Shared
  The bookkeeping data is stored in the managed region, and has previously been
  initialized by another QMallocPool instance constructed using the NewShared
  pool type.

  The malloc pool bookkeeping data contains absolute pointers.  As such, if
  multiple processes intend to allocate into the malloc pool, is is essential 
  that they map the memory region to the same virtual address location.
  */

/*!
  Creates an invalid QMallocPool.
  */
QMallocPool::QMallocPool()
: d(0)
{
}

/*!
  Creates a QMallocPool on the memory region \a poolBase of length
  \a poolLength.  The pool will be constructed with the passed \a type and
  \a name. The \a name is used for diagnostics purposes only.
  */
QMallocPool::QMallocPool(void * poolBase, unsigned int poolLength,
                         PoolType type, const QString& name)
: d(0)
{
    if((type == NewShared || Shared == type) &&
            poolLength < sizeof(struct malloc_state))
        return;

    d = new QMallocPoolPrivate(poolBase, poolLength, type, name);
}

/*!
  Destroys the malloc pool.
  */
QMallocPool::~QMallocPool()
{
    if(d)
        delete d;
    d = 0;
}

/*!
  Returns the allocated size of \a mem, assuming \a mem was previously returned
  by malloc(), calloc() or realloc().
  */
size_t QMallocPool::size_of(void *mem)
{
    return chunksize(mem2chunk(mem)) - sizeof(mchunkptr);
}

/*!
  Allocates memory for an array of \a nmemb elements of \a size each and returns
  a pointer to the allocated memory.  The memory is  set to zero.  Returns 0 if
  the memory could not be allocated.
  */
void *QMallocPool::calloc(size_t nmemb, size_t size)
{
    Q_ASSERT(d && "Cannot operate on a null malloc pool");
    QMallocPtr p(d);
    return dlcalloc(nmemb, size);
}

/*!
  Allocates \a size  bytes and returns a pointer to the allocated memory.  The
  memory is not cleared.  Returns 0 if the memory could not be allocated.
 */
void *QMallocPool::malloc(size_t size)
{
    Q_ASSERT(d && "Cannot operate on a null malloc pool");
    QMallocPtr p(d);
    return dlmalloc(size);
}

/*!
  Frees the memory space pointed to by \a ptr, which must  have  been returned
  by a previous call to malloc(), calloc() or realloc().  Otherwise, or  if
  \c {free(ptr)}  has  already  been  called  before,  undefined behavior
  occurs.  If \a ptr is 0, no operation is performed.
 */
void QMallocPool::free(void *ptr)
{
    Q_ASSERT(d && "Cannot operate on a null malloc pool");
    QMallocPtr p(d);
    dlfree(ptr);
}

/*!
  Changes the size of the memory block pointed to by \a ptr to \a size bytes.
  The contents will be unchanged to the minimum of the old and new sizes; newly
  allocated memory will be uninitialized.  If \a ptr is 0, the call is
  equivalent to malloc(size); if size is equal to zero, the  call is equivalent
  to free(ptr).  Unless ptr is 0, it must have been returned by an earlier call
  to malloc(),  calloc()  or  realloc().  If the area pointed to was moved, a
  free(ptr) is done.
 */
void *QMallocPool::realloc(void *ptr, size_t size)
{
    Q_ASSERT(d && "Cannot operate on a null malloc pool");
    QMallocPtr p(d);
    return dlrealloc(ptr, size);
}

/*!
  Returns true if this is a valid malloc pool.  Invalid malloc pools cannot be
  allocated from.
 */
bool QMallocPool::isValid() const
{
    return d;
}

/*!
  Returns a MemoryStats structure containing information about the memory use
  of this pool.
 */
QMallocPool::MemoryStats QMallocPool::memoryStatistics() const
{
    Q_ASSERT(d && "Cannot operate on a null malloc pool");
    QMallocPtr p(d);

    struct mallinfo info = dlmallinfo();

    MemoryStats rv = { d->poolLength,
                       (unsigned long)info.usmblks,
                       (unsigned long)info.arena,
                       (unsigned long)info.uordblks,
                       (unsigned long)info.keepcost };
    return rv;
}

QTM_END_NAMESPACE
