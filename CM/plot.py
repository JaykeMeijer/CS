#!/usr/bin/env python
from __future__ import division
import sys
import matplotlib.pyplot as plt

uniq = {}

# Calculate average of duplicates
for line in sys.stdin.readlines():
    split = line[:-1].split(' ')
    dtype = split[0]

    try:
        nthreads, nelements, worktime, runtime = map(int, split[1:])
        key = (dtype, nthreads, nelements)

        if key in uniq:
            oldn, average = uniq[key]
            newn = oldn + 1
            uniq[key] = (newn, (oldn * average + runtime) / newn)
        else:
            uniq[key] = (1, runtime)
    except ValueError:
        pass

# Accumulate results in plottable data
vary_nthreads = {}
vary_nelements = {}

for (dtype, nthreads, nelements), (_, runtime) in uniq.iteritems():
    if nelements not in vary_nthreads:
        vary_nthreads[nelements] = {}

    if dtype not in vary_nthreads[nelements]:
        vary_nthreads[nelements][dtype] = []

    if nthreads not in vary_nelements:
        vary_nelements[nthreads] = {}

    if dtype not in vary_nelements[nthreads]:
        vary_nelements[nthreads][dtype] = []

    vary_nthreads[nelements][dtype].append((nthreads, runtime))
    vary_nelements[nthreads][dtype].append((nelements, runtime))

# Plot varying number of threads
for nelements, data in vary_nthreads.iteritems():
    fig = plt.figure()
    plt.title('%d elements' % nelements)
    plt.xlabel('threads')

    for dtype, typedata in data.iteritems():
        if not typedata:
            continue

        # Sort data pairs according to horizontal axis
        nthreads, runtime = zip(*typedata)
        indices = dict([(n, i) for i, n in enumerate(nthreads)])
        threads_range = sorted(indices.keys())
        sorted_runtimes = [runtime[indices[i]] for i in threads_range]

        plt.plot(threads_range, sorted_runtimes, '-o', label=dtype.upper())

    plt.legend()

# Plot varying number of elements
for nthreads, data in vary_nelements.iteritems():
    fig = plt.figure()
    plt.title('%d threads' % nthreads)
    plt.xlabel('elements')

    for dtype, typedata in data.iteritems():
        if not typedata:
            continue

        # Sort data pairs according to horizontal axis
        nelements, runtime = zip(*typedata)
        indices = dict([(n, i) for i, n in enumerate(nelements)])
        elements_range = sorted(indices.keys())
        sorted_runtimes = [runtime[indices[i]] for i in elements_range]

        plt.plot(elements_range, sorted_runtimes, '-o', label=dtype.upper())

    plt.legend()

plt.show()
