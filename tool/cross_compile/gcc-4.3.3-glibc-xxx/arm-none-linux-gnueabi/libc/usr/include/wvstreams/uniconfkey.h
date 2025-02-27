/* -*- Mode: C++ -*-
 * Worldvisions Weaver Software:
 *   Copyright (C) 2002 Net Integration Technologies, Inc.
 * 
 * UniConfKeys are paths in the UniConf hierarchy.
 */
#ifndef __UNICONFKEY_H
#define __UNICONFKEY_H

#include "wvstring.h"
#include "wvlinklist.h"


// The Python headers try to #define ANY as void.  If we're building Python
// wrappers, get rid of that.
#undef ANY


/**
 * Represents a UniConf key which is a path in a hierarchy structured much
 * like the traditional Unix filesystem.
 * 
 * - Segments in the path are delimited by slashes.
 * - The empty string refers to the current level of the tree (eg. root).
 * - Keys are case insensitive yet preserve case information.
 * - Paired slashes are converted to single slashes.
 * - Trailing slashes are discarded.
 * 
 * The following paths are equivalent when canonicalized:
 * 
 * - foo/key (the canonical representation)
 * - Foo/Key (also canonical but preserves case)
 * - /foo/key (converted to foo/key)
 * - foo//key (converted to foo/key)
 * - foo/key/ (converted to foo/key)
 * 
 * Keys that may contain slashes or nulls should be escaped in some fashion
 * prior to constructing a UniConfKey object. Simply prefixing slashes with
 * backslashes is inadequate because UniConfKey does not give any special
 * meaning to backslash.
 */
class UniConfKey
{
    WvString path;

public:
    static UniConfKey EMPTY; /*!< represents "" (root) */
    static UniConfKey ANY;   /*!< represents "*" */
    static UniConfKey RECURSIVE_ANY; /*!< represents "..." */

    /** Constructs an empty UniConfKey (the 'root'). */
    UniConfKey();

    /**
     * Constructs a UniConfKey from a string.
     * 
     * See the rules above for information about how the key string
     * is canonicalized.
     * 
     * "key" is the key as a string
     */
    UniConfKey(WvStringParm key)
        { init(key); }

    /**
     * Constructs a UniConfKey from a string.
     * 
     * See the rules above for information about how the key string
     * is canonicalized.  This constructor only exists to help out the
     * C++ compiler with its automatic type conversions.
     * 
     * "key" is the key as a string
     */
    UniConfKey(const char *key)
        { init(key); }   
    
    /** Constructs a UniConfKey from an int. */
    UniConfKey(int key)
        { init(key); }

    /**
     * Copies a UniConfKey.
     * "other" is the key to copy
     */
    UniConfKey(const UniConfKey &other);

    /**
     * Constructs a UniConfKey by concatenating two keys.
     * "path" is the initial part of the new path
     * "key" is the tail of the new path
     */
    UniConfKey(const UniConfKey &path, const UniConfKey &key);

    /**
     * Appends a path to this path.
     * "other" is the path
     */
    void append(const UniConfKey &other);

    /**
     * Prepends a path to this path.
     * "other" is the path
     */
    void prepend(const UniConfKey &other);

    /**
     * Returns true if this path has zero segments (also known as root).
     * Returns: numsegments() == 0
     */
    bool isempty() const;

    /** Returns true if the key contains a wildcard. */
    bool iswild() const;

    /** Returns true if the key has a trailing slash. */
    bool hastrailingslash() const;

    /**
     * Returns the number of segments in this path.
     * 
     * The number of segments is equal to the number of slashes
     * in the path unless the path is "/" (the root), which has
     * zero segments.
     * 
     * Returns: the number of segments
     */
    int numsegments() const;

    /**
     * Returns the specified segment of the path.
     * "i" is the segment index
     * Returns: the segment
     */
    UniConfKey segment(int i) const;

    /**
     * Returns the path formed by the first n segments of this path and 
     * removes them from the key.
     * Returns: the path
     */
    UniConfKey pop(int n = 1);
    
    /**
     * Returns the path formed by the n first segments of this path.
     * "n" is the number of segments
     * Returns: the path
     */
    UniConfKey first(int n = 1) const;

    /**
     * Returns the path formed by the n last segments of this path.
     * "n" is the number of segments
     * Returns: the path
     */
    UniConfKey last(int n = 1) const;

    /**
     * Returns the path formed by removing the first n segments of
     *   this path.
     * "n" is the number of segments
     * Returns: the path
     */
    UniConfKey removefirst(int n = 1) const;

    /**
     * Returns the path formed by removing the last n segments of
     *   this path.
     * "n" is the number of segments
     * Returns: the path
     */
    UniConfKey removelast(int n = 1) const;

    /**
     * Returns a range of segments.
     * "i" is the first segment index, beginning if <= 0
     * "j" is the last segment index, end if >= numsegments()
     * Returns: the path, empty if j <= i
     */
    UniConfKey range(int i, int j) const;

    /**
     * Returns the canonical string representation of the path.
     * 
     * If the UniConfKey was constructed in part or whole from
     * strings, then the string returned here will have the same
     * case information as those strings but the arrangement of
     * slashes may differ.  That is, the identity
     * UniConfKey(string).printable() == string does not hold.
     * 
     * Returns: the path as a string
     */
    WvString printable() const;
    operator WvString() const
        { return printable(); }
    
    /**
     * Returns a (const char *) of printable() directly.
     */
    const char *cstr() const
        { return printable(); }

    /**
     * Assigns this path to equal another.
     * "other" is the other path
     */
    UniConfKey &operator= (const UniConfKey &other);

    /**
     * Compares two paths lexicographically.
     * Uses case-insensitive matching on the path string to produce
     * a total ordering of all paths.
     * "other" is the other path
     * Returns: 0 if *this == other, < 0 if *this < other, else > 0
     */
    int compareto(const UniConfKey &other) const;

    /**
     * Determines if the key matches a pattern.
     * Patterns are simply keys that may have path segments consiting
     * entirely of "*".  Optional path segments are indicated by
     * the segment "..." which matches zero or more segments.
     * 
     * Using wildcards to represent part of a segment is not supported yet.
     * "pattern" is the pattern
     * Returns: true if the key matches, false otherwise
     */
    bool matches(const UniConfKey &pattern) const;


    /**
     * Returns true if 'key' is a the same, or a subkey, or this UniConfKey.
     */
    bool suborsame(const UniConfKey &key) const;
    bool suborsame(const UniConfKey &key, WvString &subkey) const;

    /**
     * If this UniConfKey is a subkey of 'key', then return the subkey
     * portion.  Behaviour is undefined when this is not the same.  Use
     * suborsame() to check.
     */
    UniConfKey subkey(const UniConfKey &key) const;

    /**
     * Determines if two paths are equal.
     * "other" is the other path
     * Returns: true in that case
     */
    bool operator== (const UniConfKey &other) const
        { return compareto(other) == 0; }
        
    /**
     * Determines if two paths are unequal.
     * "other" is the other path
     * Returns: true in that case
     */
    bool operator!= (const UniConfKey &other) const
        { return ! (*this == other); }

    /**
     * Determines if this path precedes the other lexicographically.
     * "other" is the other path
     * Returns: true in that case
     */
    bool operator< (const UniConfKey &other) const
        { return compareto(other) < 0; }

    class Iter;

protected:
    void init(WvStringParm key);
};


DeclareWvList(UniConfKey);

/** An iterator over the segments of a key. */
class UniConfKey::Iter
{
    const UniConfKey &key;
    int seg, max;
    UniConfKey curseg;
    
public:
    Iter(const UniConfKey &_key) : key(_key) 
        { }

    void rewind()
        { seg = -1; max = key.numsegments(); }
    
    bool cur()
        { return seg >= 0 && seg < max; }
    
    bool next()
        { seg++; curseg = key.segment(seg); return cur(); }
    
    const UniConfKey *ptr() const
        { return &curseg; }
    
    WvIterStuff(const UniConfKey);
};

#endif // __UNICONFKEY_H
