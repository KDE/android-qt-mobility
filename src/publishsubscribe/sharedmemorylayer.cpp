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

#include "qvaluespace_p.h"
#include "qmallocpool_p.h"
#include "qvaluespacepublisher.h"
#include "qsystemreadwritelock_p.h"
#include "qpacketprotocol_p.h"

#include <QSet>
#include <QCoreApplication>
#include <QLocalSocket>
#include <QLocalServer>
#include <QDir>
#include <QVariant>

#include <QMutex>
#include <QSharedMemory>
#include <QTime>
#include <QThread>

QTM_BEGIN_NAMESPACE

#define VERSION_TABLE_ENTRIES 8191
#define ROOT_VERSION_ENTRY 0

#define MAX_PATH_SIZE 16384
#define MAX_DATA_SIZE 16384
#define INVALID_HANDLE 0xFFFF

#define ALIGN4b(n) ((n) + (((n) % 4)?(4 - ((n) % 4)):0))

// #define QVALUESPACE_UPDATE_STATS

static inline QDataStream& operator<<(QDataStream& stream, unsigned long v)
{
    if (sizeof(unsigned long) == sizeof(uint)) {
        stream << (uint)v;
        return stream;
    } else {
        stream << (quint64)v;
        return stream;
    }
}

static inline QDataStream& operator>>(QDataStream& stream, unsigned long& v)
{
    if (sizeof(unsigned long) == sizeof(uint)) {
        uint v32;
        stream >> v32;
        v = (unsigned long)v32;
    } else {
        quint64 v64;
        stream >> v64;
        v = (unsigned long)v64;
    }
    return stream;
}

static int vsmemcmp(const char * s1, int l1, const char * s2, int l2)
{
    if (l1 < l2)
        return -1;
    if (l1 > l2)
        return 1;
    return ::memcmp(s1, s2, l1);
}

///////////////////////////////////////////////////////////////////////////////
// declare VersionTable
///////////////////////////////////////////////////////////////////////////////
struct VersionTable {

    struct Entry {
        Entry(unsigned int v, unsigned int n) : version(v), nodePtr(n) {}

        unsigned int version;
        union {
            unsigned int nodePtr;
            unsigned int nxtFreeBlk; // 0 means no next free block
                                     // (as root is always 0)
        };
    };

    unsigned int nxtFreeBlk;
    unsigned int nextCreationId;

    Entry entries[VERSION_TABLE_ENTRIES];
};


// declare NodeOwner
/*
   Owner of data in a node.

   Overhead: 8
 */
struct NodeOwner {
    unsigned long data1;
    unsigned long data2;
};


// declare NodeDatum
/*
   Encapsulates the data and data owner associated with a Node.

   Overhead: 12 + data size
 */
struct NodeDatum {
    NodeOwner owner;
    enum Type { Boolean = QVariant::Bool,
                Int = QVariant::Int,
                UInt = QVariant::UInt,
                LongLong = QVariant::LongLong,
                ULongLong = QVariant::ULongLong,
                Double = QVariant::Double,
                Char = QVariant::Char,
                String = QVariant::String,
                ByteArray = QVariant::ByteArray,
                SerializedType /* Catch all for everything else */ };
    //Type type;
    unsigned short type;
    unsigned short len;
    char data[0];
};

// declare NodeWatch
/*
   Node write watcher.

   Overhead: 8
*/
struct NodeWatch {
    unsigned long data1;
    unsigned long data2;
};

///////////////////////////////////////////////////////////////////////////////
// declare Node
///////////////////////////////////////////////////////////////////////////////
/*
   In memory representation of a node.

   In memory, nodes are structured in one of the following three ways.
   Virtual (No Data) Node:
       <Node (dataCount == 0)><const char [] name>
   Single data node:
       <Node (dataCount == 1)><char [] name><NodeDatum><char [] data>
   Multiple data node:
       <Node (dataCount >= 2)><char [] name><unsigned int dataListPtr>
   With single watch:
       <Node (watchCount == 1)><char [] name><Rest of node...><NodeWatch>
   With multiple watch:
       <Node (watchCount >= 2)><char [] name><Rest of node...><unsigned int nodeWatchPtr>

   Overhead: 16 bytes + name size
 */
struct Node {
    unsigned short parent;    /* Id for my parent */
    unsigned int creationId;  /* Unique id set when I was constructed */

    unsigned short subNodes;  /* Sub node count */
    unsigned int subNodePtr;  /* Ptr to malloc'd array of sub node ptrs */

    unsigned short watchCount;
    unsigned short dataCount; /* Amount of data associated with this node */
    unsigned short nameLen;   /* Length of name in bytes */
    unsigned short dummy;     /* dummy for 4 byte alignment */

    char name[0];             /* My name - NO trailing null */

    bool valid() {
        return 0 != watchCount || 0 != dataCount || 0 != subNodes;
    }
    unsigned int dataBlockSize()
    {
        if(0 == dataCount) {
            return 0;
        } else if(1 == dataCount) {
            NodeDatum * datum = (NodeDatum *)dataBegin();
            return sizeof(NodeDatum) + datum->len;
        } else if(2 >= dataCount) {
            return sizeof(unsigned int);
        }
    }

    unsigned int watchBlockSize()
    {
        if(0 == watchCount)
            return 0;
        else if(1 == watchCount)
            return sizeof(NodeWatch);
        else if(2 >= watchCount)
            return sizeof(unsigned int);
        return 0;
    }

    unsigned int size()
    {
        unsigned int rv = sizeof(Node);
        rv += ALIGN4b(nameLen);
        if(watchCount == 1) {
            rv += sizeof(NodeWatch);
        } else if(watchCount >= 2) {
            rv += sizeof(unsigned int);
        }

        if(dataCount == 1) {
            NodeDatum * datum = (NodeDatum *)dataBegin();
            rv += sizeof(NodeDatum) + ALIGN4b(datum->len);
        } else if(dataCount >= 2) {
            rv += sizeof(unsigned int);
        }

        return rv;
    }

    void * dataBegin()
    {
        return (void *)(name + ALIGN4b(nameLen));
    }

    void * watchBegin()
    {
        if(0 == dataCount) {
            return dataBegin();
        } else if(1 == dataCount) {
            NodeDatum * datum = (NodeDatum *)dataBegin();
            return datum->data + ALIGN4b(datum->len);
        } else {
            return name + ALIGN4b(nameLen) + sizeof(unsigned int);
        }
    }
};

bool operator==(const NodeOwner &lhs, const NodeOwner &rhs)
{
    return 0 == ::memcmp(&lhs, &rhs, sizeof(NodeOwner));
}
bool operator!=(const NodeOwner &lhs, const NodeOwner &rhs)
{
    return 0 != ::memcmp(&lhs, &rhs, sizeof(NodeOwner));
}
bool operator==(const NodeWatch &lhs, const NodeWatch &rhs)
{
    return 0 == ::memcmp(&lhs, &rhs, sizeof(NodeWatch));
}
bool operator!=(const NodeWatch &lhs, const NodeWatch &rhs)
{
    return 0 != ::memcmp(&lhs, &rhs, sizeof(NodeWatch));
}
static const NodeWatch INVALID_WATCH = {0,0};
static const NodeOwner INVALID_OWNER = {0,0};

///////////////////////////////////////////////////////////////////////////////
// declare FixedMemoryTree
///////////////////////////////////////////////////////////////////////////////
class FixedMemoryTree : public QObject
{
    Q_OBJECT
public:
    FixedMemoryTree(void * mem, unsigned int size, bool initMem);

    // Returns the closest node to the specified path
    unsigned short findClosest(const char * path,
                               const char ** matched);
    unsigned short findClosest(unsigned int from,
                               const char * subPath,
                               const char ** matched);

    NodeDatum * data(unsigned short node);

    bool addWatch(const char * path, NodeWatch owner);
    bool remWatch(const char * path, NodeWatch owner);

    bool insert(const char * path,
                NodeDatum::Type type,
                const char * data,
                unsigned int dataLen,
                NodeOwner owner);

    unsigned short offsetOfSubNode(unsigned short node,
                                   unsigned short subNode);

    bool remove(const char * path, NodeOwner owner);

    inline Node * getNode(unsigned int);
    inline Node * subNode(unsigned int, unsigned short);
    inline unsigned int version(unsigned int);

    inline void * fromPtr(unsigned int ptr);

#ifdef QVALUESPACE_UPDATE_STATS
    inline QMallocPool *mallocPool() const { return pool; }
#endif

    typedef void (*NodeChangeFunction)(unsigned short node, void *ctxt);
    void setNodeChangeFunction(NodeChangeFunction, void *);

private:
    static unsigned int growListSize(unsigned int currentSize);
    static unsigned int shrunkListSize(unsigned int currentSize,
                                       unsigned int toSize);

    bool setWatch(unsigned short node, NodeWatch owner);
    bool remWatch(unsigned short node, NodeWatch owner);
    bool setData(unsigned short node, NodeOwner owner,
                 NodeDatum::Type type,  const char * data,
                 unsigned int dataLen);

    void removeFrom(unsigned short from, unsigned short subNodeNumber);

    unsigned short findRecur(unsigned short node, const char * path,
                             const char ** consumed);

    bool insertRecur(unsigned short node, const char * path,
                     NodeDatum::Type type, const char * data,
                     unsigned int dataLen, NodeOwner owner);

    bool addWatchRecur(unsigned short node, const char * path,
                       NodeWatch owner);

    bool remWatchRecur(unsigned short node, const char * path,
                       NodeWatch owner);
    bool removeRecur(unsigned short node, const char * path,
                     NodeOwner owner);
    unsigned short removeRecur(unsigned short, NodeOwner owner);

    unsigned int locateInNode(Node * node, const char * name,
                              unsigned int len, bool * match);

    inline unsigned int ptr(void * mem);
    inline VersionTable * versionTable();
    inline void bump(unsigned short node);

    unsigned short newNode(const char * name, unsigned int len,
                           NodeWatch owner);
    unsigned short newNode(const char * name,
                           unsigned int len,
                           NodeOwner owner = INVALID_OWNER,
                           NodeDatum::Type type = (NodeDatum::Type)0,
                           const char * data = 0,
                           unsigned int dataLen = 0);

    inline Node * node(unsigned int);

    char * poolMem;
    QMallocPool * pool;
    bool changed;
    NodeChangeFunction changeFunc;
    void * changeFuncContext;
};

///////////////////////////////////////////////////////////////////////////////
// define FixedMemoryTree
///////////////////////////////////////////////////////////////////////////////
FixedMemoryTree::FixedMemoryTree(void * mem, unsigned int size,
                                 bool initMemory )
: poolMem((char *)mem), pool(0), changed(false), changeFunc(0)
{
    Q_ASSERT(size);
    Q_ASSERT(poolMem);

    if(initMemory) {
        /* Initialize layer version table - would be worth randomizing the
           nextFreeBlk chain to distribute entries across the 128-bit change
           notification. */
        ::memset(poolMem,0, sizeof(VersionTable));
        for(int ii = 0; ii < VERSION_TABLE_ENTRIES; ++ii)
            versionTable()->entries[ii].nxtFreeBlk =
                (ii + 1) % VERSION_TABLE_ENTRIES;

        /* Create malloc pool on the non-version table portion of the layer */
        pool =
            new QMallocPool(poolMem + sizeof(VersionTable),
                            size - sizeof(VersionTable),
                            QMallocPool::Owned, QLatin1String("FixedMemoryTree"));

        /* Create root node and fixup free block pointer */
        versionTable()->nxtFreeBlk =
            versionTable()->entries[ROOT_VERSION_ENTRY].nxtFreeBlk;
        Node * root = (Node *)pool->malloc(sizeof(Node));
        ::memset(root,0, sizeof(Node));
        root->creationId = versionTable()->nextCreationId++;
        root->parent = INVALID_HANDLE;
        Q_ASSERT((char *)root > poolMem);
        versionTable()->entries[ROOT_VERSION_ENTRY] =
            VersionTable::Entry(1, ptr(root));
    }
}

unsigned short FixedMemoryTree::findClosest(const char * path,
                                            const char ** matched)
{
    Q_ASSERT(path);
    Q_ASSERT(matched);
    Q_ASSERT(path[0] == '/');

    ++path; // Skip initial '/'

    if('\0' == *path)
        return ROOT_VERSION_ENTRY;
    else
        return findRecur(ROOT_VERSION_ENTRY, path, matched);
}

unsigned short FixedMemoryTree::findClosest(unsigned int from,
                                            const char * subPath,
                                            const char ** matched)
{
    Q_ASSERT(subPath);
    Q_ASSERT(matched);
    Q_ASSERT(subPath[0] == '/');

    ++subPath; // Skip initial '/'
    if('\0' == *subPath)
        return from;
    else
        return findRecur(from, subPath, matched);
}

/*!
  Returns the default datum for \a node, or NULL if one doesn't exist.
  */
NodeDatum * FixedMemoryTree::data(unsigned short node)
{
    Node * n = this->node(node);
    if(0 == n->dataCount) {
        return 0;
    } else if(1 == n->dataCount) {
        return (NodeDatum *)n->dataBegin();
    } else {
        unsigned int * valueList =
            (unsigned int *)fromPtr(*(unsigned int *)n->dataBegin());
        return (NodeDatum *)fromPtr(*valueList);
    }
}

/*!
  Inserts a watch for \a owner at the specified \a path
*/
bool FixedMemoryTree::addWatch(const char * path, NodeWatch owner)
{
    Q_ASSERT(path);
    Q_ASSERT(path[0] == '/');
    Q_ASSERT(INVALID_WATCH != owner);

    ++path; // Skip initial '/'

    if(*path == '\0')
        return setWatch(ROOT_VERSION_ENTRY, owner);
    else
        return addWatchRecur(ROOT_VERSION_ENTRY, path, owner);
}

/*!
  Removes an existing watch for \a owner from the specified \a path
*/
bool FixedMemoryTree::remWatch(const char * path, NodeWatch owner)
{
    Q_ASSERT(path);
    Q_ASSERT(path[0] == '/');
    Q_ASSERT(INVALID_WATCH != owner);

    ++path; // Skip initial '/'

    if(*path == '\0')
        return remWatch((unsigned short)ROOT_VERSION_ENTRY, owner);
    else
        return remWatchRecur(ROOT_VERSION_ENTRY, path, owner);
}

/*!
  Inserts \a data of length \a dataLen at \a path with the set \a owner.
  Returns true if the tree has visibly changed.
  */
bool FixedMemoryTree::insert(const char * path, NodeDatum::Type type,
                             const char * data, unsigned int dataLen,
                             NodeOwner owner)
{
    Q_ASSERT(path);
    Q_ASSERT(path[0] == '/');
    Q_ASSERT(data || !dataLen);
    Q_ASSERT(INVALID_OWNER != owner);

    if(dataLen > MAX_DATA_SIZE)
        return false;

    ++path; // Skip initial '/'

    // Special root case - no one can insert
    if(*path == '\0') return false;

    return insertRecur(ROOT_VERSION_ENTRY, path, type, data, dataLen, owner);
}

/*!
  Returns the linear offset of \a subNode within \a node.  Asserts if not
  found.
 */
unsigned short FixedMemoryTree::offsetOfSubNode(unsigned short node,
                                                unsigned short subNode)
{
    Node * me = getNode(node);
    if(!me)
        return INVALID_HANDLE;

    unsigned short * const subNodes =
        (unsigned short *)fromPtr(me->subNodePtr);
    for(unsigned short ii = 0; ii < me->subNodes; ++ii) {
        if(subNodes[ii] == subNode)
            return ii;
    }

    qFatal("SharedMemoryLayer: Unable to find sub-node %x of %x.", subNode, node);
    return 0;
}

bool FixedMemoryTree::remove(const char * path, NodeOwner owner)
{
    Q_ASSERT(path);
    Q_ASSERT(path[0] == '/');
    Q_ASSERT(INVALID_OWNER != owner);

    ++path; // Skip initial '/'

    if(*path == '\0')
        return INVALID_HANDLE != removeRecur(ROOT_VERSION_ENTRY, owner);
    else
        return removeRecur(ROOT_VERSION_ENTRY, path, owner);
}

/*!
  Given a list of size \a currentSize that needs another element, returns a
  new size designated by the following growth strategy:

  \list
  \i Initially (currentSize == 0) allocate a list of one entry.
  \i Up to 64 entries, double the size each allocation.
  \i From 64 onwards, grow in chunks of 64
  \endlist
 */
unsigned int FixedMemoryTree::growListSize(unsigned int currentSize)
{
    if(0 == currentSize)
        return 1;
    if(currentSize <= 32)
        return currentSize * 2;
    return currentSize + 64;
}

/*!
  Given that we have an allocated list of size \a currentSize, that needs to
  contain contentSize elements, returns an ideal list size designated by the
  following shrink strategy:

  \list
  \i For current size > 64, reduce in blocks of 64
  \i For currentSize < 64, reduce by half if possible
  \i Otherwise, remain the same
  \endlist
 */
unsigned int FixedMemoryTree::shrunkListSize(unsigned int currentSize,
                                             unsigned int contentSize)
{
    if(currentSize > 64) {
        return currentSize - 64 * ((currentSize - contentSize) / 64);
    } else if(contentSize < (currentSize / 2)) {
        return currentSize / 2;
    }

    return currentSize;
}

/*!
  Attempts to locate the sub node \a name (of length \a len) in \a node.  If
  found, returns the offset and sets \a *match to true.  If not found,
  sets \a *match to false and returns the offset where the node would be
  found, should it exist.  Moving all nodes up one notch and inserting the
  new node at the returned offset maintains order.
 */
unsigned int FixedMemoryTree::locateInNode(Node * node, const char * name,
                                           unsigned int len, bool * match)
{
    Q_ASSERT(node && name && match);

    // This is a linear search.  It would be possible to replace this with a 
    // binary search, which may or may not improve performance depending on the
    // number of sub nodes.
    unsigned short counter = 0;
    unsigned short * subNodes = (unsigned short *)fromPtr(node->subNodePtr);

    *match = false;
    for(counter = 0; counter < node->subNodes; ++counter) {
        Node * currentNode = this->node(subNodes[counter]);

        int memcmprv = vsmemcmp(name, len, currentNode->name,
                                             currentNode->nameLen);
        if(memcmprv < 0) {
            continue;
        } else if(memcmprv == 0) {
            *match = true;
            break;
        } else if(memcmprv > 0) {
            break;
        }
    }

    return counter;
}

/*!
  \internal
  Recursive implementation of FixedMemoryTree::findClosest
 */
unsigned short FixedMemoryTree::findRecur(unsigned short node,
                                          const char * path,
                                          const char ** consumed)
{
    // Locate end of section
    const char * endOfSection = path;
    while(*endOfSection != '/' && *endOfSection != '\0') ++endOfSection;
    const unsigned int sectionLen = (endOfSection - path);

    bool match;
    Node * const me = this->node(node);
    const unsigned int matchId = locateInNode(me, path, sectionLen, &match);
    Q_ASSERT((match && matchId < me->subNodes) ||
             (!match && matchId <= me->subNodes));

    if(!match) {
        *consumed = path - 1;
        return node;
    }

    unsigned short * const subNodes =
        (unsigned short *)fromPtr(me->subNodePtr);
    if('\0' == *endOfSection) {
        *consumed = NULL;
        return subNodes[matchId];
    }

    ++endOfSection;
    return findRecur(subNodes[matchId], endOfSection, consumed);
}

/*! Remove all nodes under node that match owner (or no-owner with no
    dependents).  Returns the last node removed, which will be \a node if
    it was removed, or INVALID_HANDLE if no nodes were removed.  This can
    be used by the parent to determine whether to increment the version
    number and whether to remove the sub node.
    */
unsigned short FixedMemoryTree::removeRecur(unsigned short node,
                                            NodeOwner owner)
{
    Node * me = this->node(node);

    unsigned short * const subNodes =
        (unsigned short*)fromPtr(me->subNodePtr);

    unsigned short removedAny = INVALID_HANDLE;
    unsigned int removedSubNodes = 0;

    for(unsigned short ii = 0; ii < me->subNodes; ++ii)  {

        unsigned short id = subNodes[ii];
        // Blank recur
        unsigned short removedId = removeRecur(id, owner);

        if(removedId != INVALID_HANDLE)
            removedAny = removedId;

        if(removedId == id) {
            // This node was removed
            ++removedSubNodes;
            subNodes[ii] = INVALID_HANDLE;
        }

    }

    // If removed any sub nodes we need to collapse our subnode list
    if(removedSubNodes == me->subNodes) {
        // All removed!
        pool->free(subNodes);
        me->subNodes = 0;
        me->subNodePtr = 0;
    } else if(removedSubNodes) {
        // Some removed
        unsigned int fillUpto = 0;

        for(unsigned int sourceUpto = 0;
            sourceUpto < me->subNodes;
            ++sourceUpto) {

            if(INVALID_HANDLE != subNodes[sourceUpto]) {
                subNodes[fillUpto] = subNodes[sourceUpto];
                ++fillUpto;
            }
        }

        me->subNodes = fillUpto;
    }

    // Now to remove data and possibly me as necessary
    if(1 == me->dataCount) {

        NodeDatum * datum = (NodeDatum *)me->dataBegin();
        if(datum->owner == owner ||
           (owner.data2 = 0xFFFFFFFF && owner.data1 == datum->owner.data1))
            me->dataCount = 0;

    } else if(me->dataCount > 1) {

        // Data list time - sigh
        unsigned int * list =
            (unsigned int *)fromPtr(*((unsigned int *)me->dataBegin()));
        unsigned int removed = 0;
        for(int ii = 0; ii < me->dataCount; ++ii) {
            NodeDatum * datum = (NodeDatum *)fromPtr(list[ii]);
            if(datum->owner == owner ||
               (owner.data2 == 0xFFFFFFFF &&
                owner.data1 == datum->owner.data1)) {
                // Remove this datum
                ++removed;
                pool->free(datum);
                list[ii] = 0xFFFFFFFF;
            }
        }

        if(removed) {
            // Three cases: Only one left in list - move into node
            //              All removed - remove list
            //              Some removed - shorten list if necessary
            if((removed + 1)== me->dataCount) {
                // Find remaining node
                bool found = false;
                for(int ii = 0; !found && ii < me->dataCount; ++ii) {
                    if(0xFFFFFFFF != list[ii]) {
                        NodeDatum * datum = (NodeDatum *)fromPtr(list[ii]);
                        me->dataCount = 0;
                        setData(node, datum->owner,
                                (NodeDatum::Type)datum->type,
                                datum->data, datum->len);
                        me = this->node(node);
                        found = true;
                    }
                }
                Q_ASSERT(found);
                pool->free(list);
            } else if(removed == me->dataCount) {
                // Easy!
                me->dataCount = 0;
                pool->free(list);
            } else {
                // Some removed
                unsigned int * newList = list;
                unsigned int allocatedListSize = pool->size_of(list) /
                                                 sizeof(unsigned int);
                unsigned int newListSize = shrunkListSize(allocatedListSize,
                        me->dataCount - removed);
                if(newListSize != allocatedListSize) {
                    newList = (unsigned int *)pool->malloc(newListSize *
                            sizeof(unsigned int));
                } else {
                    newList = list;
                }

                unsigned int newListCntr = 0;
                for(int ii = 0; ii < me->dataCount; ++ii) {
                    if(list[ii] != 0xFFFFFFFF)
                        newList[newListCntr++] = list[ii];
                }

                me->dataCount = me->dataCount - removed;
                if(list != newList) {
                    pool->free(list);
                    *((unsigned int *)me->dataBegin()) = ptr(newList);
                }
            }
        }
    }

    // Attempt watch cleanup
    if(1 == me->watchCount) {
        NodeWatch * watch = (NodeWatch *)me->watchBegin();
        if(watch->data1 == owner.data1 &&
           (owner.data2 == 0xFFFFFFFF || watch->data2 == owner.data2))
            me->watchCount = 0;

    } else if(me->watchCount > 1) {
        // Watch list time...
        NodeWatch * watch =
            (NodeWatch *)fromPtr(*(unsigned int *)me->watchBegin());

        unsigned int lastSpot = 0;
        unsigned int watchers = 0;
        for(unsigned int ii = 0; ii < me->watchCount; ++ii) {
            if(watch[ii].data1 == owner.data1 &&
               (owner.data2 == 0xFFFFFFFF || watch[ii].data2 == owner.data2)) {
                // Do nothing
            } else {
                ++watchers;
                if(ii != lastSpot)
                    watch[lastSpot] = watch[ii];
                ++lastSpot;
            }
        }

        if(watchers == me->watchCount) {
            // Nothing to do
        } else if(0 == watchers) {
            // Remove list and set watchCount appropriately
            pool->free(watch);
            me->watchCount = 0;
        } else if(1 == watchers) {
            // Set in main node
            me->watchCount = 0;
            setWatch(node, watch[0]);
            pool->free(watch);
            Q_ASSERT(1 == me->watchCount);
        } else {
            // Check if we need to shrink the list
            unsigned int allocatedLength = pool->size_of(watch) / sizeof(NodeWatch);
            unsigned int newLength = shrunkListSize(allocatedLength, watchers);
            if(newLength != allocatedLength) {
                NodeWatch * newList =
                    (NodeWatch *)pool->malloc(sizeof(NodeWatch) * newLength);
                ::memcpy(newList, watch, sizeof(NodeWatch) * watchers);
                *(unsigned int *)me->watchBegin() = ptr(newList);
                pool->free(watch);
            }
            me->watchCount = watchers;
        }
    }

    if(!me->valid() && node != ROOT_VERSION_ENTRY) {
        // Remove me :(
        VersionTable::Entry * const entry =
            &(versionTable()->entries[node]);
        entry->nxtFreeBlk = versionTable()->nxtFreeBlk;
        versionTable()->nxtFreeBlk = node;
        pool->free(me);
        bump(node);
        return node;
    } else if(removedAny != INVALID_HANDLE) {
        bump(node);
    }

    return removedAny;
}

/*!
  Removes sub node number \a subNodeNumber from \a from's sub node list.  Does
  bump \a from's version number.  Does not touch removed sub node.
 */
void FixedMemoryTree::removeFrom(unsigned short from,
                                 unsigned short subNodeNumber)
{
    Node * const me = node(from);

    Q_ASSERT(subNodeNumber < me->subNodes);

    unsigned short * const subNodes =
        (unsigned short *)fromPtr(me->subNodePtr);

    unsigned int currentSize = pool->size_of(subNodes) / sizeof(unsigned short);
    unsigned int scaleSize = shrunkListSize(currentSize, me->subNodes - 1);
    if(scaleSize != currentSize) {
        // Resize
        unsigned short * newSubNodes =
            (unsigned short *)pool->malloc(scaleSize * sizeof(unsigned short));
        ::memcpy(newSubNodes, subNodes, subNodeNumber * sizeof(unsigned short));
        ::memcpy(newSubNodes + subNodeNumber, subNodes + subNodeNumber + 1,
                  (me->subNodes - subNodeNumber - 1) * sizeof(unsigned short));
        me->subNodePtr = ptr(newSubNodes);
        pool->free(subNodes);
    } else {
        // Do not resize
        ::memmove(subNodes + subNodeNumber, subNodes + subNodeNumber + 1,
                  (me->subNodes - subNodeNumber - 1) * sizeof(unsigned short));
    }
    --me->subNodes;

    // Bump version number
    bump(from);
}

bool FixedMemoryTree::remWatchRecur(unsigned short node, const char * path,
                                    NodeWatch owner)
{
    // Locate end of section
    const char * endOfSection = path;
    while(*endOfSection != '/' && *endOfSection != '\0') ++endOfSection;
    unsigned int sectionLen = (endOfSection - path);

    Node * const me = this->node(node);
    bool match;
    unsigned int matchId = locateInNode(me, path, sectionLen, &match);
    Q_ASSERT((match && matchId < me->subNodes) ||
             (!match && matchId <= me->subNodes));

    if(!match)
        return false;

    unsigned short * const subNodes =
        (unsigned short *)fromPtr(me->subNodePtr);

    if('\0' == *endOfSection) {
        unsigned int id = subNodes[matchId];

        // We must remove this node
        if(remWatch((unsigned short)id, owner)) {
            bump(node);
            Node * them = this->node(id);
            if(!them->valid()) {
                // Remove from node list
                removeFrom(node, matchId);
                return true;
            }
            return false;
        }
        return false;
    } else {
        ++endOfSection;
        bool rv = remWatchRecur((unsigned short)subNodes[matchId], endOfSection, owner);

        if(rv) {
            bump(node);
            Node * const subNode = this->node(subNodes[matchId]);
            if(!subNode->valid()) {
                // Remove this one too
                if(subNode->subNodePtr)
                    pool->free(fromPtr(subNode->subNodePtr));
                pool->free(subNode);
                removeFrom(node, matchId);
            }
        }
        return rv;
    }
}

bool FixedMemoryTree::removeRecur(unsigned short node, const char * path,
                                  NodeOwner owner)
{
    // Locate end of section
    const char * endOfSection = path;
    while(*endOfSection != '/' && *endOfSection != '\0') ++endOfSection;
    unsigned int sectionLen = (endOfSection - path);

    Node * const me = this->node(node);
    bool match;
    unsigned int matchId = locateInNode(me, path, sectionLen, &match);
    Q_ASSERT((match && matchId < me->subNodes) ||
             (!match && matchId <= me->subNodes));

    if(!match)
        return false;

    unsigned short * const subNodes =
        (unsigned short *)fromPtr(me->subNodePtr);

    if('\0' == *endOfSection) {

        unsigned int id = subNodes[matchId];

        // We must remove this node
        unsigned short removeId = removeRecur(id, owner);

        if(removeId == id) {
            // Remove from node list
            removeFrom(node, matchId);
            // Bump my version
            bump(node);
            return true;
        } else if(INVALID_HANDLE != removeId) {
            // Bump my version
            bump(node);
            return true;
        } else {
            return false;
        }
    } else {
        ++endOfSection;
        bool rv = removeRecur(subNodes[matchId], endOfSection, owner);
        if(rv) {
            bump(node);
            Node * const subNode = this->node(subNodes[matchId]);
            if(!subNode->valid()) {
                // Remove this one too
                if(subNode->subNodePtr)
                    pool->free(fromPtr(subNode->subNodePtr));
                pool->free(subNode);
                removeFrom(node, matchId);
            }
        }
        return rv;
    }
}

/*!
  Removes \a owner as a watch on \a node.  Returns true if owner was removed,
  false if owner was not a watch.
 */
bool FixedMemoryTree::remWatch(unsigned short node, NodeWatch owner)
{
    Node * me = this->node(node);

    if(0 == me->watchCount) {
        return false;
    } else if(1 == me->watchCount) {
        NodeWatch * watch = (NodeWatch *)me->watchBegin();
        if(*watch == owner) {
            me->watchCount = 0;
            return true;
        } else {
            return false;
        }
    } else if(me->watchCount == 2) {
        NodeWatch * list = (NodeWatch *)fromPtr(*(unsigned int *)me->watchBegin());
        for(int ii = 0; ii < me->watchCount; ++ii) {
            if(list[ii] == owner) {
                // Found!
                int remaining = (ii = 0)?1:0;
                me->watchCount = 0;
                setWatch(node, list[remaining]);
                pool->free(list);
                return true;
            }
        }
        return false;
    } else if(me->watchCount > 2) {
        NodeWatch * list = (NodeWatch *)fromPtr(*(unsigned int *)me->watchBegin());
        for(unsigned short ii = 0; ii < me->watchCount; ++ii) {
            if(list[ii] == owner) {
                // Found!
                unsigned int currentListSize =
                    pool->size_of(list) / sizeof(NodeWatch);
                unsigned int newListSize =
                    shrunkListSize(currentListSize, me->watchCount - 1);
                if(currentListSize != newListSize) {
                    // Create a new list
                    NodeWatch *newList = (NodeWatch *)pool->malloc(newListSize * sizeof(NodeWatch));
                    ::memcpy(newList, list, ii * sizeof(NodeWatch));
                    ::memcpy(newList + ii, list + ii + 1,
                            (me->watchCount - 1 - ii) * sizeof(NodeWatch));
                    *(unsigned int *)me->watchBegin() = ptr(newList);
                    pool->free(list);
                } else {
                    // Simply left shift everything
                    ::memmove(list + ii, list + ii + 1,
                              (me->watchCount - 1 - ii) * sizeof(NodeWatch));
                }
                return true;
            }
        }
    }

    return false;
}

/*!
  Inserts \a owner as a watch on \a node.
 */
bool FixedMemoryTree::setWatch(unsigned short node,  NodeWatch owner)
{
    Node * me = this->node(node);

    /*
       if 0 == node->watchCount
          add to end of node
       else if 1 == node->watchCount
          move existing watch into new watch list and append to list
       else
          append to existing watch list
     */

    if(0 == me->watchCount) {
        unsigned int spareLen = pool->size_of(me) - me->size();

        if(spareLen < sizeof(NodeWatch)) {
            // Need a new node
            Node * newNode =
                (Node *)pool->malloc(me->size() + sizeof(NodeWatch));
            ::memcpy(newNode, me, me->size());
            versionTable()->entries[node].nodePtr = ptr(newNode);
            pool->free(me);
            me = newNode;
        }
        me->watchCount = 1;
        *(NodeWatch *)(me->watchBegin()) = owner;
    } else if(1 == me->watchCount) {
        // Need to create a list
        NodeWatch * list = (NodeWatch *)pool->malloc(2 * sizeof(NodeWatch));
        list[0] = *(NodeWatch *)(me->watchBegin());
        list[1] = owner;
        *(unsigned int *)(me->watchBegin()) = ptr(list);
        ++me->watchCount;
    } else {
        // Append to list
        NodeWatch * list = (NodeWatch *)fromPtr(*(unsigned int *)me->watchBegin());
        int availableSize = pool->size_of(list) / sizeof(NodeWatch);
        if(availableSize < me->watchCount + 1) {
            // Reallocate
            NodeWatch * newList =
                (NodeWatch *)pool->malloc(growListSize(availableSize) *
                                             sizeof(NodeWatch));
            ::memcpy(newList, list, me->watchCount * sizeof(NodeWatch));
            pool->free(list);
            list = newList;
            *(unsigned int *)me->watchBegin() = ptr(newList);
        }
        list[me->watchCount] = owner;
        ++me->watchCount;
    }
    return true;
}

/*!
  Inserts \a data of length \a dataLen into \a node.  Returns true if the
  version number of \a node has increased to handle this set.
  */
bool FixedMemoryTree::setData(unsigned short node,
                              NodeOwner owner,
                              NodeDatum::Type type,
                              const char * data,
                              unsigned int dataLen)
{
    Q_ASSERT(!dataLen || data);

    Node * me = this->node(node);

    /*
       if 0 == node->dataCount
           insert directly into node
       else if 1 == node->dataCount && node->dataOwner == owner
           insert directly into node
       else if 1 == node->dataCount
           move existing data into new data list and append new data to list
       else
           locate or append data to existing data list
     */

    if (0 == me->dataCount ||
       (1 == me->dataCount && ((NodeDatum *)me->dataBegin())->owner == owner)) {
        unsigned int allocatedDataLen =
            pool->size_of(me) -
            sizeof(Node) -
            me->watchBlockSize() -
            ALIGN4b(me->nameLen);

        if(allocatedDataLen > sizeof(NodeDatum))
            allocatedDataLen -= sizeof(NodeDatum);
        else
            allocatedDataLen = 0;

        if(dataLen <= allocatedDataLen) {
            // Sweet, reuse existing
            NodeDatum * datum = (NodeDatum *)me->dataBegin();
            datum->owner = owner;
            datum->type = type;
            if(me->watchBlockSize())
                ::memcpy(datum->data + ALIGN4b(dataLen), me->watchBegin(),
                        me->watchBlockSize());
            datum->len = dataLen;
            ::memcpy(datum->data, data, dataLen);
            me->dataCount = 1;
        } else {
            // Need new node
            Node * newNode =
                (Node *)pool->malloc(sizeof(Node) + ALIGN4b(me->nameLen) +
                                     sizeof(NodeDatum) + ALIGN4b(dataLen) +
                                     me->watchBlockSize());
            ::memcpy(newNode, me, sizeof(Node) + ALIGN4b(me->nameLen));
            versionTable()->entries[node].nodePtr = ptr(newNode);

            NodeDatum * datum = (NodeDatum *)newNode->dataBegin();
            datum->owner = owner;
            datum->type = type;
            if(me->watchBlockSize())
                ::memcpy(datum->data + ALIGN4b(dataLen), me->watchBegin(),
                        me->watchBlockSize());
            datum->len = dataLen;
            ::memcpy(datum->data, data, dataLen);
            me->dataCount = 1;
            pool->free(me);
            me = newNode;
        }

        bump(node);

        return true;
    } else if(1 == me->dataCount) {
        // Damn, need to create a value chain list
        NodeDatum * existingDatum = (NodeDatum *)me->dataBegin();
        unsigned int * list =
            (unsigned int *)pool->malloc(2 * sizeof(unsigned int));
        NodeDatum * currentDatum =
            (NodeDatum *)pool->malloc(sizeof(NodeDatum) + existingDatum->len);
        NodeDatum * newDatum =
            (NodeDatum *)pool->malloc(sizeof(NodeDatum) + dataLen);
        list[0] = ptr(currentDatum);
        list[1] = ptr(newDatum);

        // Copy existing data
        ::memcpy(currentDatum, existingDatum,
                 sizeof(NodeDatum) + existingDatum->len);
        // Copy new data
        newDatum->owner = owner;
        newDatum->type = type;
        newDatum->len = dataLen;
        ::memcpy(newDatum->data, data, dataLen);
        if(me->watchBlockSize())
            ::memcpy(((unsigned int *)me->dataBegin()) + 1, me->watchBegin(),
                     me->watchBlockSize());
        // Add value chain list to node
        *(unsigned int *)me->dataBegin() = ptr(list);
        me->dataCount = 2;
        return false;
    } else {
        // Attempt to locate in existing chain
        unsigned int * list =
            (unsigned int *)fromPtr(*(unsigned int *)me->dataBegin());
        for(int ii = 0; ii < me->dataCount; ++ii) {
            NodeDatum * ndata = (NodeDatum *)fromPtr(list[ii]);
            if(ndata->owner == owner) {
                // Found - but is it big enough?
                unsigned int allocatedDataLen =
                    pool->size_of(ndata) - sizeof(NodeDatum);
                if(allocatedDataLen >= dataLen) {
                    // Copy over the top
                    ::memcpy(ndata->data, data, dataLen);
                    ndata->len = dataLen;
                    ndata->type = type;
                } else {
                    // Reallocate
                    NodeDatum * newData =
                        (NodeDatum *)pool->malloc(sizeof(NodeDatum) +
                                                  dataLen);
                    newData->owner = owner;
                    newData->type = type;
                    newData->len = dataLen;
                    ::memcpy(newData->data, data, dataLen);
                    list[ii] = ptr(newData);
                    pool->free(ndata);
                }
                return ii == 0;
            }
        }

        // Not found - append
        unsigned int allocatedListLen =
            pool->size_of(list) / sizeof(unsigned int);
        if(allocatedListLen >= (unsigned int)me->dataCount + 1) {
            // We fit!
        } else {
            // We don't fit
            unsigned int * newList =
                (unsigned int *)pool->malloc(growListSize(allocatedListLen) *
                                             sizeof(unsigned int));
            ::memcpy(newList, list, allocatedListLen * sizeof(unsigned int));
            pool->free(list);
            list = newList;
            *(unsigned int *)me->dataBegin() = ptr(newList);
        }

        NodeDatum * datum = (NodeDatum *)pool->malloc(sizeof(NodeDatum) +
                                                      dataLen);
        datum->owner = owner;
        datum->len = dataLen;
        datum->type = type;
        ::memcpy(datum->data, data, dataLen);
        list[me->dataCount] = ptr(datum);
        ++me->dataCount;

        return false;
    }
}

bool FixedMemoryTree::addWatchRecur(unsigned short node, const char * path,
                                    NodeWatch owner)
{
    // Locate end of section
    const char * endOfSection = path;
    while(*endOfSection != '/' && *endOfSection != '\0') ++endOfSection;
    const unsigned int sectionLen = (endOfSection - path);

    // We operate on structures, not numbers
    Node * const me = this->node(node);
    unsigned short * subNodes =
        me->subNodePtr?(unsigned short *)fromPtr(me->subNodePtr):0;

    // Attempt to locate the sub node within the current node
    bool match;
    const unsigned int matchId = locateInNode(me, path, sectionLen, &match);
    Q_ASSERT((match && matchId < me->subNodes) ||
             (!match && matchId <= me->subNodes));

    if(match) {

        // There is a sub node of the correct name
        const unsigned short id = subNodes[matchId];

        if('\0' == *endOfSection) {
            // Update the node.  Bumps *their* version if necessary.
            bool rv = setWatch(id, owner);
            if(rv) bump(node);
            return rv;
        }
        ++endOfSection;

        bool rv = addWatchRecur(id, endOfSection, owner);
        if(rv) bump(node);
        return rv;

    } else { /* !match */

        // We *may* need to lengthen our list
        unsigned short maxSubNodesLen = subNodes?
            pool->size_of(subNodes) / sizeof(unsigned short):0;
        unsigned int newSubNode = 0;

        if('\0' == *endOfSection)
            newSubNode = newNode(path, sectionLen, owner);
        else
            newSubNode = newNode(path, sectionLen);

        if (newSubNode == ROOT_VERSION_ENTRY)
            return false;

        Node * const newNode = this->node(newSubNode);
        newNode->parent = node;

        if(maxSubNodesLen < me->subNodes + 1) {
            // Grow list
            unsigned short * newSubNodes =
                (unsigned short *)pool->malloc(growListSize(me->subNodes) *
                                             sizeof(unsigned short));
            ::memcpy(newSubNodes, subNodes, matchId * sizeof(unsigned short));
            ::memcpy(newSubNodes + matchId + 1, subNodes + matchId,
                     (me->subNodes - matchId) * sizeof(unsigned short));
            pool->free(fromPtr(me->subNodePtr));
            me->subNodePtr = ptr(newSubNodes);
            subNodes = newSubNodes;
        } else {
            // Split list
            ::memmove(subNodes + matchId + 1, subNodes + matchId,
                      (me->subNodes - matchId) * sizeof(unsigned short));
        }
        subNodes[matchId] = newSubNode;
        ++me->subNodes;

        // Bump my version
        bump(node);

        if('\0' != *endOfSection) {
            ++endOfSection;
            addWatchRecur(newSubNode, endOfSection, owner);
        }
        return true;
    }
}

/*!
  Returns true if the insert should force a version bump.
  */
bool FixedMemoryTree::insertRecur(unsigned short node,
                                  const char * path,
                                  NodeDatum::Type type,
                                  const char * data,
                                  unsigned int dataLen,
                                  NodeOwner owner)
{
    // Locate end of section
    const char * endOfSection = path;
    while(*endOfSection != '/' && *endOfSection != '\0') ++endOfSection;
    const unsigned int sectionLen = (endOfSection - path);

    // We operate on structures, not numbers
    Node * const me = this->node(node);
    unsigned short * subNodes =
        me->subNodePtr?(unsigned short *)fromPtr(me->subNodePtr):0;

    // Attempt to locate the sub node within the current node
    bool match;
    const unsigned int matchId = locateInNode(me, path, sectionLen, &match);
    Q_ASSERT((match && matchId < me->subNodes) ||
             (!match && matchId <= me->subNodes));

    if(match) {

        // There is a sub node of the correct name
        const unsigned short id = subNodes[matchId];

        if('\0' == *endOfSection) {
            // Update the node.  Bumps *their* version if necessary.
            bool rv = setData(id, owner, type, data, dataLen);
            if(rv) bump(node);
            return rv;
        }
        ++endOfSection;

        bool rv = insertRecur(id, endOfSection, type, data, dataLen, owner);
        if(rv) bump(node);
        return rv;

    } else { /* !match */

        // We *may* need to lengthen our list
        unsigned short maxSubNodesLen = subNodes?
            pool->size_of(subNodes) / sizeof(unsigned short):0;
        unsigned int newSubNode = 0;

        if('\0' == *endOfSection)
            newSubNode = newNode(path, sectionLen, owner, type, data, dataLen);
        else
            newSubNode = newNode(path, sectionLen);

        if (newSubNode == ROOT_VERSION_ENTRY)
            return false;

        Node * const newNode = this->node(newSubNode);
        newNode->parent = node;

        if(maxSubNodesLen < me->subNodes + 1) {
            // Grow list
            unsigned short * newSubNodes =
                (unsigned short *)pool->malloc(growListSize(me->subNodes) *
                                             sizeof(unsigned short));
            ::memcpy(newSubNodes, subNodes, matchId * sizeof(unsigned short));
            ::memcpy(newSubNodes + matchId + 1, subNodes + matchId,
                     (me->subNodes - matchId) * sizeof(unsigned short));
            pool->free(fromPtr(me->subNodePtr));
            me->subNodePtr = ptr(newSubNodes);
            subNodes = newSubNodes;
        } else {
            // Split list
            ::memmove(subNodes + matchId + 1, subNodes + matchId,
                      (me->subNodes - matchId) * sizeof(unsigned short));
        }
        subNodes[matchId] = newSubNode;
        ++me->subNodes;

        // Bump my version
        bump(node);

        if('\0' != *endOfSection) {
            ++endOfSection;
            insertRecur(newSubNode, endOfSection, type, data, dataLen, owner);
        }
        return true;
    }
}

VersionTable * FixedMemoryTree::versionTable()
{
    return (VersionTable *)poolMem;
}

void * FixedMemoryTree::fromPtr(unsigned int ptr)
{
    if(!ptr)
        return 0;
    return (void *)(poolMem + ptr);
}

void FixedMemoryTree::setNodeChangeFunction(NodeChangeFunction func, void *ctxt)
{
    changeFunc = func;
    changeFuncContext = ctxt;
}

unsigned int FixedMemoryTree::ptr(void * mem)
{
    Q_ASSERT(mem > poolMem);
    return (unsigned int)((char *)mem - poolMem);
}

/*! Increment \a node's version number */
void FixedMemoryTree::bump(unsigned short node)
{
    ++(versionTable()->entries[node].version);
    if(changeFunc)
        changeFunc(node, changeFuncContext);
}

Node * FixedMemoryTree::node(unsigned int entry)
{
    Q_ASSERT(entry < VERSION_TABLE_ENTRIES);
    return (Node *)fromPtr(versionTable()->entries[entry].nodePtr);
}

/*! Returns the node identified by \a entry, or null if there is no node in that
    slot.  This is different to FixedMemoryTree::node() in that it checks the
    return value (and is thus slower).
 */
Node * FixedMemoryTree::getNode(unsigned int entry)
{
    Q_ASSERT(entry < VERSION_TABLE_ENTRIES);
    void * rv = fromPtr(versionTable()->entries[entry].nodePtr);
    if(rv < poolMem + sizeof(VersionTable)) {
        return 0;
    } else {
        return (Node *)rv;
    }
}

Node * FixedMemoryTree::subNode(unsigned int node, unsigned short sub)
{
    Q_ASSERT(node < VERSION_TABLE_ENTRIES);
    Node * me = this->node(node);
    Q_ASSERT(sub < me->subNodes);
    unsigned short * const subNodes =
        (unsigned short *)fromPtr(me->subNodePtr);
    return this->node(subNodes[sub]);
}

unsigned int FixedMemoryTree::version(unsigned int entry)
{
    Q_ASSERT(entry < VERSION_TABLE_ENTRIES);
    return versionTable()->entries[entry].version;
}

/*!
    Creates a new node of \a name (name length \a len) and sets a single \a watch on it.  Returns
    ROOT_VERSION_ENTRY if the new node could not be created.
*/
unsigned short FixedMemoryTree::newNode(const char * name, unsigned int len,
                                        NodeWatch owner)
{
    Q_ASSERT(owner != INVALID_WATCH);

    // Find an empty node entry
    unsigned int node = versionTable()->nxtFreeBlk;
    if (node == ROOT_VERSION_ENTRY)
        return node;

    // Reset the next free blk
    versionTable()->nxtFreeBlk = versionTable()->entries[node].nxtFreeBlk;

    int nodeSize = sizeof(Node) + ALIGN4b(len) + sizeof(NodeWatch);

    // Allocate a new node
    Node * nodePtr = (Node *)pool->malloc(nodeSize);
    Q_ASSERT((char *)nodePtr > poolMem);
    ::memset(nodePtr,0, sizeof(Node));
    nodePtr->creationId = versionTable()->nextCreationId++;

    // Setup name
    nodePtr->nameLen = len;
    ::memcpy(nodePtr->name, name, len);

    // Setup watch
    nodePtr->watchCount = 1;
    *(NodeWatch *)(nodePtr->name + ALIGN4b(len)) = owner;

    // Enter node
    versionTable()->entries[node] = VersionTable::Entry(1, ptr(nodePtr));
    // Bump version
    bump(node);

    // Done
    return node;

}

/*!
    Creates a new node of \a name (name length \a len) and pre-fills it with \a data of length
    \a dataLen owned by \a owner.  If \a owner is an invalid owner, the node is a virtual node and
    not prefilled with data.  Returns ROOT_VERSION_ENTRY if the new node could not be created.
*/
unsigned short FixedMemoryTree::newNode(const char * name, unsigned int len,
                                        NodeOwner owner, NodeDatum::Type type,
                                        const char * data, unsigned int dataLen)
{
    Q_ASSERT(owner != INVALID_OWNER || (!data && 0 == dataLen));

    // Find an empty node entry
    unsigned int node = versionTable()->nxtFreeBlk;
    if (node == ROOT_VERSION_ENTRY)
        return node;

    // Reset the next free blk
    versionTable()->nxtFreeBlk = versionTable()->entries[node].nxtFreeBlk;

    int nodeSize = sizeof(Node) + ALIGN4b(len);
    if(INVALID_OWNER != owner)
        nodeSize += sizeof(NodeDatum) + ALIGN4b(dataLen);

    // Allocate a new node
    Node * nodePtr = (Node *)pool->malloc(nodeSize);
    Q_ASSERT((char *)nodePtr > poolMem);
    ::memset(nodePtr, 0, sizeof(Node));
    nodePtr->creationId = versionTable()->nextCreationId++;

    // Setup name
    nodePtr->nameLen = len;
    ::memcpy(nodePtr->name, name, len);

    if(INVALID_OWNER != owner) {
        // Setup data
        nodePtr->dataCount = 1;
        NodeDatum * datum = (NodeDatum *)nodePtr->dataBegin();
        datum->owner = owner;
        datum->len = dataLen;
        datum->type = type;
        if(dataLen)
            ::memcpy(datum->data, data, dataLen);
    }

    // Enter node
    versionTable()->entries[node] = VersionTable::Entry(1, ptr(nodePtr));
    // Bump version
    bump(node);

    // Done
    return node;
}

// Use aggregation/composition - the SharedMemoryLayer "has a" server socket
// rather than "is a" server socket - also gets around multiple inheritance
// issues with QObject since the QLocalServer in Qt is a QObject
class ALServerImpl : public QLocalServer
{
public:
    ALServerImpl( QObject *p ) : QLocalServer( p ) {}
    virtual ~ALServerImpl();
protected:
    virtual void incomingConnection(quintptr socketDescriptor);
};

ALServerImpl::~ALServerImpl()
{
}

// declare SharedMemoryLayer
class SharedMemoryLayer : public QAbstractValueSpaceLayer
{
    Q_OBJECT
public:
    SharedMemoryLayer();
    virtual ~SharedMemoryLayer();

    /* Common functions */
    QString name();

    bool startup(Type);

    QUuid id();
    unsigned int order();

    Handle item(Handle parent, const QString &);
    void removeHandle(Handle);
    void setProperty(Handle handle, Properties);

    bool value(Handle, QVariant *);
    bool value(Handle, const QString &, QVariant *);
    QSet<QString> children(Handle);

    QValueSpace::LayerOptions layerOptions() const;

    /* QValueSpaceSubscriber functions */
    bool supportsInterestNotification() const;
    bool notifyInterest(Handle handle, bool interested);

    /* QValueSpacePublisher functions */
    bool setValue(QValueSpacePublisher *creator, Handle handle, const QString &, const QVariant &);
    bool removeValue(QValueSpacePublisher *creator, Handle handle, const QString &);
    bool removeSubTree(QValueSpacePublisher *creator, Handle handle);
    void addWatch(QValueSpacePublisher *creator, Handle handle);
    void removeWatches(QValueSpacePublisher *creator, Handle parent);
    void sync();

    void nodeChanged(unsigned short);

    static SharedMemoryLayer * instance();

private slots:
    void disconnected();
    void readyRead();
    void closeConnections();

    void doClientSync();

protected:
    virtual void timerEvent(QTimerEvent *);

private:
#ifdef QVALUESPACE_UPDATE_STATS
    void updateStats();
#else
    inline void updateStats() {}
#endif
    QList<NodeWatch> watchers(const QByteArray &);
    void doClientEmit();
    void doClientTransmit();
    void doServerTransmit();
    bool doRemove(const QByteArray &path);
    bool doWriteItem(const QByteArray &path, const QVariant &val);
    bool setItem(NodeOwner owner, const QByteArray &path, const QVariant &val);
    bool doSetItem(NodeOwner owner, const QByteArray &path, const QVariant &val);
    bool remItems(NodeOwner owner, const QByteArray &path);
    bool doRemItems(NodeOwner owner, const QByteArray &path);
    bool setWatch(NodeWatch watch, const QByteArray &path);
    bool doSetWatch(NodeWatch watch, const QByteArray &path);
    bool remWatch(NodeWatch watch, const QByteArray &path);
    bool doRemWatch(NodeWatch watch, const QByteArray &path);
    void doNotify(const QByteArray &path, const QPacketProtocol *protocol, bool interested);
    void doClientNotify(QValueSpacePublisher *publisher, const QByteArray &path, bool interested);
    void doNotifyObject(unsigned long own, unsigned long protocol);

    QString socket() const;

    static QVariant fromDatum(const NodeDatum * data);

    struct ReadHandle
    {
        ReadHandle(const QByteArray &_path)
            : refCount(1), path(_path), currentPath(0),
              currentNode(INVALID_HANDLE), version(0), creationId(0),
              forceChange(false)
              {
              }

        unsigned int refCount;

        /* Path this read handle is *intended* to point to */
        QByteArray path;
        /* Offset into path indicating where we currently point to.  0xFFFFFFFF
           means we point to the full path (saves path.size() call) */
        unsigned int currentPath;
        /* Node we are currently pointing to. */
        unsigned short currentNode;
        /* Last version of current node */
        unsigned int version;
        /* Last creation Id of current node */
        unsigned int creationId;

        /* Indicates that version has been updated in a value() call, but
           a change should be emitted anyway. */
        bool forceChange;
    };
    ReadHandle * rh(QAbstractValueSpaceLayer::Handle handle) const
    {
        Q_ASSERT(handle && INVALID_HANDLE != handle);
        return reinterpret_cast<ReadHandle *>(handle);
    }
    bool refreshHandle(ReadHandle *);
    void clearHandle(ReadHandle *);

    QLocalServer *sserver;

    Type type;
    FixedMemoryTree * layer;
    QSystemReadWriteLock * lock;
    QMutex localLock;
    QMap<QString, ReadHandle *> handles;

    void triggerTodo();
    bool triggeredTodo;
    QPacket todo;
    QSet<QPacketProtocol *> connections;

    unsigned int newPackId() {
        lastSentId = nextPackId++;
        return lastSentId;
    }
    unsigned int nextPackId;

    unsigned int lastSentId;
    unsigned int lastRecvId;

    bool valid;
    friend class ALServerImpl;

    unsigned int forceChangeCount;

    uchar *clientIndex;
    void incNode(unsigned short);
    void decNode(unsigned short);
    QHash<unsigned short, unsigned int> m_nodeInterest;


    unsigned int changedNodesCount;
    unsigned short changedNodes[VERSION_TABLE_ENTRIES];

    // Stats memory
    unsigned long *m_statPoolSize;
    unsigned long *m_statMaxSystemBytes;
    unsigned long *m_statSystemBytes;
    unsigned long *m_statInuseBytes;
    unsigned long *m_statKeepCost;

    QSharedMemory* shm;
    QSharedMemory* subShm;
};

QVALUESPACE_AUTO_INSTALL_LAYER(SharedMemoryLayer);

///////////////////////////////////////////////////////////////////////////////
// define SharedMemoryLayer
///////////////////////////////////////////////////////////////////////////////
#define SHMLAYER_SIZE 1000000
#define SHMLAYER_ADD 0
#define SHMLAYER_REM 1
#define SHMLAYER_DONE 2
#define SHMLAYER_ADDWATCH 5
#define SHMLAYER_REMWATCH 6
#define SHMLAYER_SYNC 7
#define SHMLAYER_SUBINDEX 8
#define SHMLAYER_NOTIFY 9

struct SharedMemoryLayerClient : public QPacketProtocol
{
    SharedMemoryLayerClient(QIODevice *dev, QObject *parent = 0)
        : QPacketProtocol(dev, parent), index(0) {}

    uchar *index;
};

SharedMemoryLayer::SharedMemoryLayer()
: type(Client), layer(0), lock(0), localLock(QMutex::Recursive), triggeredTodo(false),
  nextPackId(1), lastSentId(0), lastRecvId(0), valid(false),
  forceChangeCount(0), clientIndex(0),
  changedNodesCount(0),
  m_statPoolSize(0), m_statMaxSystemBytes(0), m_statSystemBytes(0),
  m_statInuseBytes(0), m_statKeepCost(0), shm(0), subShm(0)
{
    sserver = new ALServerImpl( this );

    connect(qApp, SIGNAL(destroyed()), this, SLOT(closeConnections()));
}

SharedMemoryLayer::~SharedMemoryLayer()
{
    if(Server == type) {
        delete lock;
        lock = 0;
    }
}

QString SharedMemoryLayer::name()
{
    return QLatin1String("Shared Memory Layer");
}

static void ShmLayerNodeChanged(unsigned short, void *);
bool SharedMemoryLayer::startup(Type type)
{
    QMutexLocker locker(&localLock);

    valid = false;

    Q_ASSERT(!layer);
    if(Server == type) {
        if(!sserver->listen(socket())) {
            qWarning() << "SharedMemoryLayer: Unable to create server socket.  Only "
                     "local connections will be allowed." << sserver->errorString();
        }
    } else {
        QLocalSocket * sock = new QLocalSocket;
        sock->connectToServer(socket());
        if(!sock->waitForConnected(1000)) {
            qWarning("SharedMemoryLayer: Unable to connect to server, "
                     "shared memory layer will be disabled.");
            delete sock;
            return false;
        }

        QPacketProtocol * protocol;
        protocol = new QPacketProtocol(sock, this);
        sock->setParent(protocol);

        QObject::connect(sock, SIGNAL(stateChanged(QLocalSocket::LocalSocketState)),
                         protocol, SIGNAL(invalidPacket()));
        QObject::connect(protocol, SIGNAL(invalidPacket()),
                         this, SLOT(disconnected()));
        QObject::connect(protocol, SIGNAL(readyRead()),
                         this, SLOT(readyRead()));

        connections.insert(protocol);
    }

    if(Server == type) {
        shm = new QSharedMemory(socket(), this);
        bool created = shm->create(SHMLAYER_SIZE);
        if (!created)  {
            //qDebug() << "Reattaching to existing memory";
            shm->attach();
        }
        lock = new QSystemReadWriteLock(socket() + "_lock", QSystemReadWriteLock::Create);
    } else {
        shm = new QSharedMemory(socket(), this);
        shm->attach(QSharedMemory::ReadOnly);
        qsrand(QTime(0,0,0).secsTo(QTime::currentTime())+QCoreApplication::applicationPid());
        subShm = new QSharedMemory(socket()+QString::number(qrand()), this);
        if (!subShm->create((VERSION_TABLE_ENTRIES + 7) / 8, QSharedMemory::ReadWrite)) {
            qWarning() << "SharedMemoryLayer client cannot create clientIndex:"
                       << subShm->errorString() << subShm->key();
        }

        lock = new QSystemReadWriteLock(socket() + "_lock", QSystemReadWriteLock::Open);
    }

    if (shm->error() != QSharedMemory::NoError ||
        ((!subShm || subShm->error()!= QSharedMemory::NoError) && Server != type)) {
        qFatal("SharedMemoryLayer: Unable to create or access shared resources. (%s - %s)",
               shm->errorString().toLatin1().constData(),
               subShm->errorString().toLatin1().constData());
        return false;
    }

    if (subShm)
        clientIndex = (uchar *)subShm->data();
    if(Client == type) {
        QPacket mem;
        mem << (unsigned int)0
            << (quint8)SHMLAYER_SUBINDEX << subShm->key()
            << (quint8)SHMLAYER_DONE;
        (*connections.begin())->send(mem);
    }

    layer = new FixedMemoryTree((char*)shm->data(),
                                SHMLAYER_SIZE,
                                (Server == type));
    if(Server == type)
        layer->setNodeChangeFunction(&ShmLayerNodeChanged, (void *)this);

    this->type = type;

    valid = (lock != 0) && (layer != 0);
    if (!valid)
        qWarning("SharedMemoryLayer::startup failed");
    return valid;
}

void ALServerImpl::incomingConnection(quintptr socketDescriptor)
{
    QLocalSocket * sock = new QLocalSocket;
    sock->setSocketDescriptor(socketDescriptor);

    QPacketProtocol * protocol;
    protocol = new SharedMemoryLayerClient(sock, this);
    sock->setParent(protocol);

    SharedMemoryLayer *al = static_cast<SharedMemoryLayer*>( QObject::parent() );

    QObject::connect(sock, SIGNAL(stateChanged(QLocalSocket::LocalSocketState)),
                     protocol, SIGNAL(invalidPacket()));
    QObject::connect(protocol, SIGNAL(invalidPacket()),
                     al, SLOT(disconnected()));
    QObject::connect(protocol, SIGNAL(readyRead()),
                     al, SLOT(readyRead()));

    qDebug() << "New client" << protocol << "connected";

    al->connections.insert(protocol);
}

void SharedMemoryLayer::timerEvent(QTimerEvent *)
{
    QMutexLocker locker(&localLock);

    if(Client == type)
        doClientTransmit();
    else
        doServerTransmit();
}

void SharedMemoryLayer::doServerTransmit()
{
    QMutexLocker locker(&localLock);

    Q_ASSERT(Server == type);

    triggeredTodo = false;

    QPacket others;
    others << (quint8)SHMLAYER_SYNC << (unsigned int)0;

    for(QSet<QPacketProtocol *>::ConstIterator iter = connections.begin();
            iter != connections.end();
            ++iter)  {

        SharedMemoryLayerClient *client =
            (SharedMemoryLayerClient *)(*iter);
        if(!client->index) {
            (*iter)->send(others);
        } else {
            bool found = false;
            for(unsigned int ii = 0;!found && ii < changedNodesCount; ++ii) {
                if(client->index[changedNodes[ii] >> 3] & (1 << (changedNodes[ii] & 0x7)))
                    found = true;
            }
            if(found) {
                //qDebug() << "GGGGGGGGGGGGGGGG";
                (*iter)->send(others);
            }
        }

    }

    changedNodesCount = 0;
    doClientEmit();
}

void SharedMemoryLayer::doClientTransmit()
{
    QMutexLocker locker(&localLock);

    Q_ASSERT(Client == type);

    triggeredTodo = false;

    if(!todo.isEmpty()) {
        todo << (quint8)SHMLAYER_DONE;
        (*connections.begin())->send(todo);
        todo.clear();
    }
}

typedef QHash<QByteArray, QMap<const QPacketProtocol *, uint> > PathsOfInterest;
Q_GLOBAL_STATIC(PathsOfInterest, pathsOfInterest);

void SharedMemoryLayer::disconnected()
{
    Q_ASSERT(sender());

    QMutexLocker locker(&localLock);

    QPacketProtocol * protocol = (QPacketProtocol *)sender();
    protocol->disconnect();
    protocol->deleteLater();

    if(Client == type) {
        qFatal("SharedMemoryLayer: Shared memory layer server unexpectedly terminated.");
    } else {
        qDebug() << "Removing" << protocol << "from space";

        NodeOwner owner;
        owner.data1 = (unsigned long)protocol;
        owner.data2 = 0xFFFFFFFF;

        connections.remove(protocol);
        if(layer->remove("/", owner)) {
            QPacket others;
            others << (quint8)SHMLAYER_SYNC << (unsigned int)0;
            for(QSet<QPacketProtocol *>::ConstIterator iter = connections.begin();
                    iter != connections.end();
                    ++iter)
                    (*iter)->send(others);
            doClientEmit();
        }

        QList<QByteArray> paths = pathsOfInterest()->keys();

        for (int i = 0; i < paths.count(); ++i) {
            const QByteArray interestPath = paths.at(i);

            if (!(*pathsOfInterest())[interestPath].remove(protocol))
                continue;

            if (!(*pathsOfInterest())[interestPath].isEmpty())
                continue;

            pathsOfInterest()->remove(interestPath);

            QList<NodeWatch> owners = watchers(interestPath);

            QSet<unsigned long> written;
            QSet<QValueSpacePublisher *> notified;

            for (int ii = 0; ii < owners.count(); ++ii) {
                const NodeWatch &watch = owners.at(ii);

                if (watch.data1 != (unsigned long)protocol)
                    continue;

                if (watch.data1 == 0) {
                    QValueSpacePublisher *publisher =
                        reinterpret_cast<QValueSpacePublisher *>(watch.data2);
                    if (!notified.contains(publisher)) {
                        doClientNotify(publisher, interestPath, false);
                        notified.insert(publisher);
                    }
                } else {
                    if (!written.contains(watch.data1)) {
                        ((QPacketProtocol *)watch.data1)->send() << quint8(SHMLAYER_NOTIFY)
                                                                 << watch.data2 << interestPath
                                                                 << false;
                        written.insert(watch.data1);
                    }
                }
            }

        }
    }
}

static void ShmLayerNodeChanged(unsigned short node, void *ctxt)
{
    SharedMemoryLayer *layer = (SharedMemoryLayer *)ctxt;
    layer->nodeChanged(node);
}

void SharedMemoryLayer::nodeChanged(unsigned short node)
{
    QMutexLocker locker(&localLock);

    if(changedNodesCount != VERSION_TABLE_ENTRIES)
        changedNodes[changedNodesCount++] = node;
}

void SharedMemoryLayer::readyRead()
{
    QMutexLocker locker(&localLock);

    Q_ASSERT(sender());
    QPacketProtocol * protocol = (QPacketProtocol *)sender();

    QPacket pack = protocol->read();
    if (Client == type) {
        //qDebug() << "Client" << protocol << "woken";
        if (pack.isEmpty())
            return;

        quint8 op = 0;
        pack >> op;
        switch(op) {
        case SHMLAYER_SYNC: {
            unsigned int recvId = 0;
            pack >> recvId;
            if(0 != recvId)
                lastRecvId = recvId;
            doClientEmit();
            break;
        }
        case SHMLAYER_NOTIFY: {
            unsigned long owner;
            QByteArray path;
            bool interested;
            pack >> owner >> path >> interested;
            doClientNotify(reinterpret_cast<QValueSpacePublisher *>(owner), path, interested);
            break;
        }
        default:
            qFatal("SharedMemoryLayer: Invalid message type %d "
                   "received from shared memory layer server.", op);
            break;
        }
    } else {
        unsigned int packId = 0;
        pack >> packId;

        bool changed = false;
        bool done = false;

        changedNodesCount = 0;
        while(!done && !pack.isEmpty()) {
            quint8 op;
            pack >> op;

            switch(op) {
                case SHMLAYER_ADD:
                    {
                        unsigned long own;
                        QByteArray path;
                        QVariant value;
                        pack >> own >> path >> value;
                        NodeOwner owner;
                        owner.data1 = (unsigned long)protocol;
                        owner.data2 = own;
                        changed |= doSetItem(owner, path, value);
                    }
                    break;

                case SHMLAYER_REM:
                    {
                        unsigned long own;
                        QByteArray path;
                        pack >> own >> path;
                        NodeOwner owner;
                        owner.data1 = (unsigned long)protocol;
                        owner.data2 = own;
                        changed |= doRemItems(owner, path);
                    }
                    break;

                case SHMLAYER_ADDWATCH:
                    {
                        unsigned long own;
                        QByteArray path;
                        pack >> own >> path;
                        NodeWatch owner;
                        owner.data1 = (unsigned long)protocol;
                        owner.data2 = own;
                        changed |= doSetWatch(owner, path);
                    }
                    break;

                case SHMLAYER_REMWATCH:
                    {
                        unsigned long own;
                        QByteArray path;
                        pack >> own >> path;
                        NodeWatch owner;
                        owner.data1 = (unsigned long)protocol;
                        owner.data2 = own;
                        changed |= doRemWatch(owner, path);
                    }
                    break;

                case SHMLAYER_DONE:
                    done = true;
                    break;

                case SHMLAYER_SUBINDEX:
                    {
                        QString shmkey;
                        pack >> shmkey;

                        QSharedMemory* mem = new QSharedMemory(shmkey, protocol);
                        if (!mem->attach(QSharedMemory::ReadOnly)) {
                            qWarning() << "SharedMemoryLayer: Unable to shmat with id:"
                                       << shmkey << "error:" << mem->errorString();
                            delete mem;
                        } else {
                            static_cast<SharedMemoryLayerClient *>(protocol)->index =
                                reinterpret_cast<uchar *>(mem->data());
                        }
                    }
                    break;

                case SHMLAYER_NOTIFY:
                    {
                        QByteArray path;
                        bool interested;
                        pack >> path >> interested;
                        doNotify(path, protocol, interested);
                    }
                    break;
                default:
                    qWarning("SharedMemoryLayer: Invalid client request %x received.", op);
                    disconnected();
                    return;
            }
        }

        // Send change notification
        QPacket causal;
        causal << (quint8)SHMLAYER_SYNC << packId;
        protocol->send(causal);

        if(changed)
            doServerTransmit();
    }
}

void SharedMemoryLayer::closeConnections()
{
    QMutexLocker locker(&localLock);

    QMutableSetIterator<QPacketProtocol *> i(connections);
    while (i.hasNext()) {
        delete i.next();
        i.remove();
    }
}

void SharedMemoryLayer::doClientEmit()
{
    QMutexLocker locker(&localLock);

    QMap<QString, ReadHandle *> cpy = handles;
    for(QMap<QString, ReadHandle *>::ConstIterator iter = cpy.begin();
            iter != cpy.end();
            ++iter)
        ++(*iter)->refCount;

    for(QMap<QString, ReadHandle *>::ConstIterator iter = cpy.begin();
            iter != cpy.end();
            ++iter)  {
        if(refreshHandle(*iter)) {
            if((*iter)->forceChange) {
                Q_ASSERT(forceChangeCount);
                --forceChangeCount;
                (*iter)->forceChange = false;
            }

            emit handleChanged((Handle)*iter);
        } else {

            if((*iter)->forceChange) {
                Q_ASSERT(forceChangeCount);
                --forceChangeCount;
                (*iter)->forceChange = false;
                emit handleChanged((Handle)*iter);
            }
        }
    }

    while(forceChangeCount) {
        for(QMap<QString, ReadHandle *>::ConstIterator iter = cpy.begin();
            forceChangeCount && iter != cpy.end();
            ++iter)  {

            if((*iter)->forceChange) {
                Q_ASSERT(forceChangeCount);
                --forceChangeCount;
                (*iter)->forceChange = false;
                emit handleChanged((Handle)*iter);
            }

        }
    }

    for(QMap<QString, ReadHandle *>::ConstIterator iter = cpy.begin();
            iter != cpy.end();
            ++iter)
        removeHandle((Handle)*iter);
}

QUuid SharedMemoryLayer::id()
{
    return QVALUESPACE_SHAREDMEMORY_LAYER;
}

unsigned int SharedMemoryLayer::order()
{
    return 0x10000000;
}

bool SharedMemoryLayer::value(Handle handle, QVariant *data)
{
    QMutexLocker locker(&localLock);

    if (!valid)
        return false;
    Q_ASSERT(layer);
    Q_ASSERT(data);

    ReadHandle * rhandle = rh(handle);

    lock->lockForRead();

    if(0xFFFFFFFF != rhandle->currentPath)
        if(refreshHandle(rhandle)) {
            if(!rhandle->forceChange) {
                ++forceChangeCount;
                rhandle->forceChange = true;
            }
        }

    Node * node = 0;
    if(0xFFFFFFFF == rhandle->currentPath) {
        node = layer->getNode(rhandle->currentNode);
        if(!node || node->creationId != rhandle->creationId) {
            refreshHandle(rhandle);
            node = layer->getNode(rhandle->currentNode);
            if(!rhandle->forceChange) {
                ++forceChangeCount;
                rhandle->forceChange = true;
            }
        }
    }

    bool rv = false;
    if(0xFFFFFFFF == rhandle->currentPath) {
        Q_ASSERT(node);
        NodeDatum const * const datum = layer->data(rhandle->currentNode);
        if(datum) {
            *data = fromDatum(datum);
            rv = true;
        }
    }

    lock->unlock();

    return rv;
}

bool SharedMemoryLayer::value(Handle handle, const QString &subPath,
                             QVariant *data)
{
    QMutexLocker locker(&localLock);

    Q_ASSERT(layer);
    Q_ASSERT(data);
    Q_ASSERT(!subPath.isEmpty());
    Q_ASSERT(*subPath.constData() == QLatin1Char('/'));

    ReadHandle * rhandle = rh(handle);

    lock->lockForRead();

    if(0xFFFFFFFF != rhandle->currentPath)
        if(refreshHandle(rhandle)) {
            if(!rhandle->forceChange) {
                ++forceChangeCount;
                rhandle->forceChange = true;
            }
        }

    bool rv = false;
    if(0xFFFFFFFF == rhandle->currentPath) {
        QByteArray abs_path = rhandle->path + subPath.toUtf8();
        if (rhandle->path.length() == 1) 
            abs_path = subPath.toUtf8();
        ReadHandle vhandle(abs_path);
        clearHandle(&vhandle);
        refreshHandle(&vhandle);
        if(0xFFFFFFFF == vhandle.currentPath) {
            // Valid!
            NodeDatum const * const datum = layer->data(vhandle.currentNode);
            if(datum) {
                *data = fromDatum(datum);
                rv = true;
            }
        }
    }

    lock->unlock();

    return rv;
}

QSet<QString> SharedMemoryLayer::children(Handle handle)
{
    QMutexLocker locker(&localLock);

    Q_ASSERT(layer);
    ReadHandle * rhandle = rh(handle);

    QSet<QString> rv;
    lock->lockForRead();

    if(0xFFFFFFFF != rhandle->currentPath)
        if(refreshHandle(rhandle)) {
            if(!rhandle->forceChange) {
                ++forceChangeCount;
                rhandle->forceChange = true;
            }
        }

    Node * node = 0;
    if(0xFFFFFFFF == rhandle->currentPath) {
        node = layer->getNode(rhandle->currentNode);
        if(!node || node->creationId != rhandle->creationId) {
            if(refreshHandle(rhandle)) {
                if(0xFFFFFFFF == rhandle->currentPath)
                    node = layer->getNode(rhandle->currentNode);
                else
                    node = 0;
                if(!rhandle->forceChange) {
                    ++forceChangeCount;
                    rhandle->forceChange = true;
                }
            }
        }
    }

    if(0xFFFFFFFF == rhandle->currentPath) {
        Q_ASSERT(node);
        for(unsigned short ii = 0; ii < node->subNodes; ++ii) {
            Node * n = layer->subNode(rhandle->currentNode, ii);
            rv.insert(QString::fromUtf8(n->name, n->nameLen));
        }
    }

    lock->unlock();
    return rv;
}

QValueSpace::LayerOptions SharedMemoryLayer::layerOptions() const
{
    return QValueSpace::TransientLayer | QValueSpace::WritableLayer;
}

SharedMemoryLayer::Handle SharedMemoryLayer::item(Handle parent, const QString &key)
{
    QMutexLocker locker(&localLock);

    Q_UNUSED(parent);
    Q_ASSERT(layer);
    Q_ASSERT(*key.constData() == QLatin1Char('/'));
    Q_ASSERT(InvalidHandle == parent);
    QMap<QString, ReadHandle *>::Iterator iter = handles.find(key);
    if(iter != handles.end()) {
        ++(*iter)->refCount;
        return (QAbstractValueSpaceLayer::Handle)*iter;
    } else {
        ReadHandle * handle = new ReadHandle(key.toUtf8());
        clearHandle(handle);
        lock->lockForRead();
        refreshHandle(handle);
        lock->unlock();
        handles.insert(key, handle);
        return (QAbstractValueSpaceLayer::Handle)(handle);
    }
}

/*!
  Attempt to advance \a handle to point to the true path or update it if it
  does.  Returns true if the handle has either:

  \list 1
  \i Changed value
  \i Moved from being a partial reference to a true reference
  \i Moved from being a true reference to a partial reference
  \endlist

  That is, returns true if a handleChanged() signal is needed :)

  The layer MUST be locked for reading.
 */

bool SharedMemoryLayer::refreshHandle(ReadHandle * handle)
{
    QMutexLocker locker(&localLock);

    Q_ASSERT(handle);

    ReadHandle old = *handle;
    unsigned short oldNode = handle->currentNode;

    // Refresh handle
    if(0xFFFFFFFF == handle->currentPath) {
        // True path!
        Node * node = layer->getNode(handle->currentNode);
        if(!node || node->creationId != handle->creationId) {
            // Bad news!
            clearHandle(handle);
            oldNode = INVALID_HANDLE; // prevent double dec
        } else {
            // Super - still a true path
            handle->version = layer->version(handle->currentNode);
        }
    }

    if(0xFFFFFFFF != handle->currentPath &&
       INVALID_HANDLE != handle->currentNode) {
        // Partial
        Node * node = layer->getNode(handle->currentNode);
        if(!node || node->creationId != handle->creationId) {
            // Bad news!
            clearHandle(handle);
            oldNode = INVALID_HANDLE; // prevent double dec
        } else if(handle->version != layer->version(handle->currentNode)) {
            // Something changed - try and advance!
            const char * out = 0;
            handle->currentNode = layer->findClosest(handle->currentNode,
                    handle->path.constData() + handle->currentPath,
                    &out);

            Node * newNode = layer->getNode(handle->currentNode);
            Q_ASSERT(newNode);
            handle->version = layer->version(handle->currentNode);
            handle->creationId = newNode->creationId;
            if(out)
                handle->currentPath = (out - handle->path.constData());
            else
                handle->currentPath = 0xFFFFFFFF;
        }
    }

    if(INVALID_HANDLE == handle->currentNode) {
        // Start from scratch
        const char * out = 0;
        handle->currentNode = layer->findClosest(handle->path.constData(),
                                                 &out);

        Node * newNode = layer->getNode(handle->currentNode);
        Q_ASSERT(newNode);
        handle->version = layer->version(handle->currentNode);
        handle->creationId = newNode->creationId;
        if(out)
            handle->currentPath = (out - handle->path.constData());
        else
            handle->currentPath = 0xFFFFFFFF;
    }

    if(handle->currentNode != oldNode) {
        decNode(oldNode);
        incNode(handle->currentNode);
    }

    bool rv = false;
    if(0xFFFFFFFF == handle->currentPath) {
        // Valid!
        rv = (handle->currentNode != old.currentNode ||
              handle->currentPath != old.currentPath ||
              handle->creationId != old.creationId ||
              handle->version != old.version);
    } else {
        // Returns true if old was valid
        rv = (0xFFFFFFFF == old.currentPath);
    }

    return rv;
}

void SharedMemoryLayer::clearHandle(ReadHandle *handle)
{
    QMutexLocker locker(&localLock);

    handle->currentPath = 0;
    decNode(handle->currentNode);
    handle->currentNode = INVALID_HANDLE;
    handle->version = 0;
    handle->creationId = 0;
}

void SharedMemoryLayer::triggerTodo()
{
    QMutexLocker locker(&localLock);

    if (triggeredTodo || !valid)
        return;

    QCoreApplication::postEvent(this, new QTimerEvent(0), Qt::LowEventPriority);
}

void SharedMemoryLayer::setProperty(Handle, Properties)
{
    // Properties aren't used by shared memory layer (always emits changed)
}

void SharedMemoryLayer::removeHandle(Handle h)
{
    QMutexLocker locker(&localLock);

    Q_ASSERT(layer);
    Q_ASSERT(h && INVALID_HANDLE != h);

    ReadHandle * rhandle = rh(h);
    Q_ASSERT(rhandle->refCount);
    --rhandle->refCount;
    if(!rhandle->refCount) {
        handles.remove(QString::fromUtf8(rhandle->path.constData(), rhandle->path.length()));
        clearHandle(rhandle);
        delete rhandle;
    }
}

#ifdef QVALUESPACE_UPDATE_STATS

void SharedMemoryLayer::updateStats()
{
    QMutexLocker locker(&localLock);

    if(!m_statPoolSize) {
        // Hasn't initialized stats!
        Q_ASSERT(layer);
        unsigned int v = 0;
        NodeOwner owner = { 1, 1 };

        layer->insert("/.ValueSpace/SharedMemoryLayer/Memory/PoolSize",
                      NodeDatum::UInt, (const char *)&v, sizeof(unsigned int),
                      owner);
        layer->insert("/.ValueSpace/SharedMemoryLayer/Memory/MaxSystemBytes",
                      NodeDatum::UInt, (const char *)&v, sizeof(unsigned int),
                      owner);
        layer->insert("/.ValueSpace/SharedMemoryLayer/Memory/SystemBytes",
                      NodeDatum::UInt, (const char *)&v, sizeof(unsigned int),
                      owner);
        layer->insert("/.ValueSpace/SharedMemoryLayer/Memory/InuseBytes",
                      NodeDatum::UInt, (const char *)&v, sizeof(unsigned int),
                      owner);
        layer->insert("/.ValueSpace/SharedMemoryLayer/Memory/KeepCost",
                      NodeDatum::UInt, (const char *)&v, sizeof(unsigned int),
                      owner);

        const char * out;
        unsigned short node;
        unsigned short baseNode =
            layer->findClosest("/.ValueSpace/SharedMemoryLayer/Memory", &out);

        node = layer->findClosest(baseNode, "/PoolSize", &out);
        m_statPoolSize = (unsigned long *)layer->data(node)->data;
        node = layer->findClosest(baseNode, "/MaxSystemBytes", &out);
        m_statMaxSystemBytes = (unsigned long *)layer->data(node)->data;
        node = layer->findClosest(baseNode, "/SystemBytes", &out);
        m_statSystemBytes = (unsigned long *)layer->data(node)->data;
        node = layer->findClosest(baseNode, "/InuseBytes", &out);
        m_statInuseBytes = (unsigned long *)layer->data(node)->data;
        node = layer->findClosest(baseNode, "/KeepCost", &out);
        m_statKeepCost = (unsigned long *)layer->data(node)->data;
    }

    QMallocPool::MemoryStats mstats = layer->mallocPool()->memoryStatistics();
    *m_statPoolSize = mstats.poolSize;
    *m_statMaxSystemBytes = mstats.maxSystemBytes;
    *m_statSystemBytes = mstats.systemBytes;
    *m_statInuseBytes = mstats.inuseBytes;
    *m_statKeepCost = mstats.keepCost;
}

#endif

QList<NodeWatch> SharedMemoryLayer::watchers(const QByteArray &path)
{
    QMutexLocker locker(&localLock);

    Q_ASSERT(layer);

    lock->lockForRead();

    const char * matched = 0;
    unsigned short node = layer->findClosest(path.constData(), &matched);
    QList<NodeWatch> owners;
    while(INVALID_HANDLE != node) {
        Node * n = layer->getNode(node);
        if(1 == n->watchCount) {
            owners.append(*(NodeWatch *)n->watchBegin());
        } else if(2 <= n->watchCount) {
            NodeWatch *watches =
                (NodeWatch *)layer->fromPtr(*(unsigned int*)n->watchBegin());
            for(unsigned int ii = 0; ii < n->watchCount; ++ii) {
                owners.append(watches[ii]);
            }
        }

        node = n->parent;
    }

    lock->unlock();

    return owners;
}

void SharedMemoryLayer::doNotify(const QByteArray &path, const QPacketProtocol *protocol,
                                 bool interested)
{
    QMutexLocker locker(&localLock);

    bool sendNotification = false;
    if (interested) {
        if ((++(*pathsOfInterest())[path][protocol]) == 1)
            sendNotification = true;
    } else {
        if ((--(*pathsOfInterest())[path][protocol]) == 0) {
            (*pathsOfInterest())[path].remove(protocol);

            if ((*pathsOfInterest())[path].isEmpty())
                pathsOfInterest()->remove(path);

            sendNotification = true;
        }
    }

    if (!sendNotification)
        return;

    QList<NodeWatch> owners = watchers(path);

    QSet<unsigned long> written;
    QSet<QValueSpacePublisher *> notified;

    for (int ii = 0; ii < owners.count(); ++ii) {
        const NodeWatch &watch = owners.at(ii);

        if (watch.data1 == 0) {
            QValueSpacePublisher *publisher =
                reinterpret_cast<QValueSpacePublisher *>(watch.data2);
            if (!notified.contains(publisher)) {
                doClientNotify(publisher, path, interested);
                notified.insert(publisher);
            }
        } else {
            if (!written.contains(watch.data1)) {
                ((QPacketProtocol *)watch.data1)->send() << quint8(SHMLAYER_NOTIFY)
                                                         << watch.data2 << path << interested;
                written.insert(watch.data1);
            }
        }
    }
}

void SharedMemoryLayer::doNotifyObject(unsigned long own, unsigned long protocol)
{
    QMutexLocker locker(&localLock);

    QList<QByteArray> paths = pathsOfInterest()->keys();

    for (int i = 0; i < paths.count(); ++i) {
        const QByteArray &interestPath = paths.at(i);

        QList<NodeWatch> owners = watchers(interestPath);

        QSet<unsigned long> written;
        QSet<QValueSpacePublisher *> notified;

        for (int ii = 0; ii < owners.count(); ++ii) {
            const NodeWatch &watch = owners.at(ii);

            if (watch.data1 != protocol || watch.data2 != own)
                continue;

            if (watch.data1 == 0) {
                QValueSpacePublisher *publisher =
                    reinterpret_cast<QValueSpacePublisher *>(watch.data2);
                if (!notified.contains(publisher)) {
                    doClientNotify(publisher, interestPath, true);
                    notified.insert(publisher);
                }
            } else {
                if (!written.contains(watch.data1)) {
                    ((QPacketProtocol *)watch.data1)->send() << quint8(SHMLAYER_NOTIFY)
                                                             << watch.data2 << interestPath
                                                             << true;
                    written.insert(watch.data1);
                }
            }
        }
    }
}

bool SharedMemoryLayer::setWatch(NodeWatch watch, const QByteArray &path)
{
    QMutexLocker locker(&localLock);

    if(path.count() > MAX_PATH_SIZE || path.startsWith("/.ValueSpace") || !valid)
        return false;
    Q_ASSERT(layer);

    if(Client == type) {
        if(todo.isEmpty())
            todo << newPackId();

        todo << (quint8)SHMLAYER_ADDWATCH << watch.data2 << path;
        triggerTodo();
        return true;
    } else {
        bool changed = doSetWatch(watch, path);
        if(changed) triggerTodo();
        return changed;
    }
}

bool SharedMemoryLayer::doSetWatch(NodeWatch watch, const QByteArray &path)
{
    QMutexLocker locker(&localLock);

    Q_ASSERT(layer);

    lock->lockForWrite();
    bool rv = layer->addWatch(path.constData(), watch);
    updateStats();
    lock->unlock();

    doNotifyObject(watch.data2, watch.data1);

    return rv;
}

bool SharedMemoryLayer::remWatch(NodeWatch watch, const QByteArray &path)
{
    QMutexLocker locker(&localLock);

    if(path.count() > MAX_PATH_SIZE || !valid)
        return false;
    Q_ASSERT(layer);

    if(Client == type) {
        if(todo.isEmpty())
            todo << newPackId();

        todo << (quint8)SHMLAYER_REMWATCH << watch.data2 << path;
        triggerTodo();
        return true;
    } else {
        bool changed = doRemWatch(watch, path);
        if(changed) triggerTodo();
        return changed;
    }
}

bool SharedMemoryLayer::doRemWatch(NodeWatch watch, const QByteArray &path)
{
    QMutexLocker locker(&localLock);

    Q_ASSERT(layer);

    lock->lockForWrite();
    bool rv = layer->remWatch(path.constData(), watch);
    updateStats();
    lock->unlock();

    return rv;
}

bool SharedMemoryLayer::setItem(NodeOwner owner, const QByteArray &path,
                               const QVariant &val)
{
    QMutexLocker locker(&localLock);

    if(path.count() > MAX_PATH_SIZE || path.startsWith("/.ValueSpace") || !valid)
        return false;
    Q_ASSERT(layer);
    bool changed = false;

    if(Client == type) {
        if(todo.isEmpty())
            todo << newPackId();

        todo << (quint8)SHMLAYER_ADD << owner.data2 << path << val;
        triggerTodo();
        return true;
    } else {
        changed = doSetItem(owner, path, val);
        if(changed)
            triggerTodo();
    }
    return changed;
}

bool SharedMemoryLayer::doSetItem(NodeOwner owner, const QByteArray &path,
                                 const QVariant &val)
{
    QMutexLocker locker(&localLock);

    bool rv = false;

    lock->lockForWrite();

    switch(val.type()) {
        case QVariant::Bool:
            {
                unsigned int v = val.toBool()?1:0;
                rv = layer->insert(path.constData(), NodeDatum::Boolean, (const char *)&v,
                              sizeof(unsigned int), owner);
            }
            break;
        case QVariant::Int:
            {
                int v = val.toInt();
                rv = layer->insert(path.constData(), NodeDatum::Int, (const char *)&v,
                              sizeof(int), owner);
            }
            break;
        case QVariant::UInt:
            {
                unsigned int v = val.toUInt();
                rv = layer->insert(path.constData(), NodeDatum::UInt, (const char *)&v,
                              sizeof(unsigned int), owner);
            }
            break;
        case QVariant::LongLong:
            {
                long long v = val.toLongLong();
                rv = layer->insert(path.constData(), NodeDatum::LongLong, (const char *)&v,
                              sizeof(long long), owner);
            }
            break;
        case QVariant::ULongLong:
            {
                unsigned long long v = val.toULongLong();
                rv = layer->insert(path.constData(), NodeDatum::ULongLong, (const char *)&v,
                              sizeof(unsigned long long), owner);
            }
            break;
        case QVariant::Double:
            {
                double v = val.toDouble();
                rv = layer->insert(path.constData(), NodeDatum::Double, (const char *)&v,
                              sizeof(double), owner);
            }
            break;
        case QVariant::Char:
            {
                QChar v = val.toChar();
                rv = layer->insert(path.constData(), NodeDatum::Char, (const char *)&v,
                              sizeof(QChar), owner);
            }
            break;
        case QVariant::String:
            {
                QString v = val.toString();
                rv = layer->insert(path.constData(), NodeDatum::String,
                              (const char *)v.constData(), v.length() * sizeof(QChar), owner);
            }
            break;
        case QVariant::ByteArray:
            {
                QByteArray v = val.toByteArray();
                rv = layer->insert(path.constData(), NodeDatum::ByteArray,
                              v.constData(), v.length(), owner);
            }
            break;
        default:
            {
                QByteArray v;
                QDataStream ds(&v, QIODevice::WriteOnly);
                ds << val;
                rv = layer->insert(path.constData(), NodeDatum::SerializedType,
                              v.constData(), v.length(), owner);
            }
            break;
    }

    updateStats();
    lock->unlock();

    return rv;
}

bool SharedMemoryLayer::remItems(NodeOwner owner, const QByteArray &path)
{
    QMutexLocker locker(&localLock);

    if (!valid)
        return false;
    bool rv = false;

    if(Client == type) {
        if(todo.isEmpty())
            todo << newPackId();

        todo << (quint8)SHMLAYER_REM << owner.data2 << path;
        triggerTodo();
    } else {
        rv = doRemItems(owner, path);
        if(rv) triggerTodo();
    }

    return rv;
}

bool SharedMemoryLayer::doRemItems(NodeOwner owner, const QByteArray &path)
{
    QMutexLocker locker(&localLock);

    if (!valid)
        return false;
    bool rv = false;

    if(Client == type) {
        if(todo.isEmpty())
            todo << newPackId();

        todo << (quint8)SHMLAYER_REM << owner.data2 << path;
        triggerTodo();

    } else {
        Q_ASSERT(layer);
        lock->lockForWrite();
        rv = layer->remove(path.constData(), owner);
        updateStats();
        lock->unlock();
    }

    return rv;
}

QString SharedMemoryLayer::socket() const
{
    QString socketPath(QLatin1String("qt/"));

#ifdef Q_OS_UNIX
    static bool pipePathExists = false;
    if (!pipePathExists) {
        if (QDir::temp().mkpath(socketPath))
            pipePathExists = true;
    }
    socketPath = QDir::temp().absoluteFilePath(socketPath);
#endif

    QByteArray key = qgetenv("QT_VALUESPACE_SHMLAYER");
    if (!key.isEmpty())
        socketPath.append(QString::fromLocal8Bit(key.constData(), key.length()));
    else
        socketPath.append(QLatin1String("valuespace_shmlayer"));

    return socketPath;
}

void SharedMemoryLayer::sync()
{
    if (type == Client) {
        if (QThread::currentThread() != thread())
            QMetaObject::invokeMethod(this, "doClientSync", Qt::BlockingQueuedConnection);
        else
            doClientSync();
    } else {
        doServerTransmit();
    }
}

void SharedMemoryLayer::doClientSync()
{
    QMutexLocker locker(&localLock);

    Q_ASSERT(1 == connections.count());

    unsigned int waitId = lastSentId;

    if (lastRecvId >= waitId)
        return; // Done

    // Send any outstanding messages
    doClientTransmit();

    // Get transmission socket
    QLocalSocket *socket = static_cast<QLocalSocket *>((*connections.begin())->device());
    socket->flush();

    // Wait
    while (QLocalSocket::UnconnectedState != socket->state() && waitId > lastRecvId)
        socket->waitForReadyRead(-1);
}

QVariant SharedMemoryLayer::fromDatum(const NodeDatum * data)
{
    switch(data->type) {
        case NodeDatum::Boolean:
            Q_ASSERT(4 == data->len);
            return QVariant(0 != (*(unsigned int *)data->data));
        case NodeDatum::Int:
            Q_ASSERT(4 == data->len);
            return QVariant((*(int *)data->data));
        case NodeDatum::UInt:
            Q_ASSERT(4 == data->len);
            return QVariant((*(unsigned int *)data->data));
        case NodeDatum::LongLong:
            Q_ASSERT(8 == data->len);
            return QVariant((*(long long*)data->data));
        case NodeDatum::ULongLong:
            Q_ASSERT(8 == data->len);
            return QVariant((*(unsigned long long*)data->data));
        case NodeDatum::Double:
            Q_ASSERT(8 == data->len);
            return QVariant((*(double*)data->data));
        case NodeDatum::Char:
            Q_ASSERT(2 == data->len);
            return QVariant(QChar((unsigned short)(0xFFFF & (*(unsigned int*)data->data))));
        case NodeDatum::String:
            return QVariant(QString((QChar *)data->data,
                                     data->len / sizeof(QChar)));
        case NodeDatum::ByteArray:
            return QVariant(QByteArray(data->data, data->len));
        case NodeDatum::SerializedType:
            {
                QByteArray ba(data->data, data->len);
                QDataStream ds(ba);
                QVariant rv;
                ds >> rv;
                return rv;
            }
        default:
            qFatal("SharedMemoryLayer: Unknown datum type.");
            return QVariant();
    }
}

void SharedMemoryLayer::incNode(unsigned short node)
{
    QMutexLocker locker(&localLock);

    if(type == Server || node == INVALID_HANDLE)
        return;

    QHash<unsigned short, unsigned int>::Iterator iter =
        m_nodeInterest.find(node);
    if(iter == m_nodeInterest.end()) {
        clientIndex[node >> 3] |= (1 << (node & 0x7));
        m_nodeInterest.insert(node, 1);
    } else {
        (*iter)++;
    }
}

void SharedMemoryLayer::decNode(unsigned short node)
{
    QMutexLocker locker(&localLock);

    if(type == Server || node == INVALID_HANDLE)
        return;

    QHash<unsigned short, unsigned int>::Iterator iter =
        m_nodeInterest.find(node);
    Q_ASSERT(iter != m_nodeInterest.end());
    (*iter)--;
    if(!*iter) {
        clientIndex[node >> 3] &= ~(1 << (node & 0x7));
        m_nodeInterest.erase(iter);
    }
}

Q_GLOBAL_STATIC(SharedMemoryLayer, sharedMemoryLayer);
SharedMemoryLayer * SharedMemoryLayer::instance()
{
    return sharedMemoryLayer();
}

typedef QSet<QValueSpacePublisher *> WatchObjects;
Q_GLOBAL_STATIC(WatchObjects, watchObjects);

void SharedMemoryLayer::doClientNotify(QValueSpacePublisher *publisher, const QByteArray &path,
                                       bool interested)
{
    QMutexLocker locker(&localLock);

    // Invalid publisher.
    if (!watchObjects()->contains(publisher))
        return;

    QByteArray emitPath;

    const QByteArray publisherPath = publisher->path().toUtf8();
    if (path.startsWith(publisherPath)) {
        // path is under publisherPath
        emitPath = path.mid(publisherPath.length());
    } else if (publisherPath.startsWith(path)) {
        // path is a parent of publisherPath
    } else {
        // path is not for this publisher.
        return;
    }

    emitInterestChanged(publisher, QString::fromUtf8(emitPath.constData()), interested);
 }

bool SharedMemoryLayer::supportsInterestNotification() const
{
    return true;
}

bool SharedMemoryLayer::notifyInterest(Handle handle, bool interested)
{
    QMutexLocker locker(&localLock);

    if (!valid)
        return false;
    Q_ASSERT(layer);

    ReadHandle *rhandle = rh(handle);
    if (type == Client) {
        if (todo.isEmpty())
            todo << newPackId();

        todo << quint8(SHMLAYER_NOTIFY) << rhandle->path << interested;

        triggerTodo();
    } else {
        doNotify(rhandle->path, 0, interested);
    }

    return true;
}

bool SharedMemoryLayer::setValue(QValueSpacePublisher *creator, Handle handle, const QString &path,
                                 const QVariant &data)
{
    QMutexLocker locker(&localLock);

    ReadHandle *readHandle = reinterpret_cast<ReadHandle *>(handle);

    if (!handles.values().contains(readHandle))
        return false;

    NodeOwner owner;
    owner.data1 = 0;
    owner.data2 = reinterpret_cast<unsigned long>(creator);

    QByteArray fullPath(readHandle->path);
    if (!fullPath.endsWith('/') && path != QLatin1String("/"))
        fullPath.append('/');

    fullPath.append(path.mid(1));

    return setItem(owner, fullPath, data);
}

bool SharedMemoryLayer::removeValue(QValueSpacePublisher *creator,
                                    Handle handle,
                                    const QString &path)
{
    QMutexLocker locker(&localLock);

    ReadHandle *readHandle = reinterpret_cast<ReadHandle *>(handle);

    if (!handles.values().contains(readHandle))
        return false;

    NodeOwner owner;
    owner.data1 = 0;
    owner.data2 = reinterpret_cast<quintptr>(creator);

    QByteArray fullPath(readHandle->path);
    if (!fullPath.endsWith('/'))
        fullPath.append('/');

    int index = 0;
    while (index < path.length() && path[index] == QLatin1Char('/'))
        ++index;

    fullPath.append(path.mid(index));

    return remItems(owner, fullPath);
}

bool SharedMemoryLayer::removeSubTree(QValueSpacePublisher *creator, Handle handle)
{
    QMutexLocker locker(&localLock);

    ReadHandle *readHandle = reinterpret_cast<ReadHandle *>(handle);

    if (!handles.values().contains(readHandle))
        return false;

    NodeOwner owner;
    owner.data1 = 0;
    owner.data2 = reinterpret_cast<quintptr>(creator);

    return remItems(owner, readHandle->path);
}

void SharedMemoryLayer::addWatch(QValueSpacePublisher *creator, Handle handle)
{
    QMutexLocker locker(&localLock);

    watchObjects()->insert(creator);

    ReadHandle *readHandle = reinterpret_cast<ReadHandle *>(handle);

    if (!handles.values().contains(readHandle))
        return;

    NodeWatch owner;
    owner.data1 = 0;
    owner.data2 = reinterpret_cast<unsigned long>(creator);

    setWatch(owner, readHandle->path);
}

void SharedMemoryLayer::removeWatches(QValueSpacePublisher *creator, Handle parent)
{
    QMutexLocker locker(&localLock);

    watchObjects()->remove(creator);

    ReadHandle *readHandle = reinterpret_cast<ReadHandle *>(parent);

    if (!handles.values().contains(readHandle))
        return;

    NodeWatch owner;
    owner.data1 = 0;
    owner.data2 = reinterpret_cast<unsigned long>(creator);

    remWatch(owner, readHandle->path);
}

#include "sharedmemorylayer.moc"
QTM_END_NAMESPACE

