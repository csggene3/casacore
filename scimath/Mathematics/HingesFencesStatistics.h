//# Copyright (C) 2000,2001
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU Library General Public License as published by
//# the Free Software Foundation; either version 2 of the License, or (at your
//# option) any later version.
//#
//# This library is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
//# License for more details.
//#
//# You should have received a copy of the GNU Library General Public License
//# along with this library; if not, write to the Free Software Foundation,
//# Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//# $Id: Array.h 21545 2015-01-22 19:36:35Z gervandiepen $

#ifndef SCIMATH_HINGESFENCESSTATISTICS_H
#define SCIMATH_HINGESFENCESSTATISTICS_H

#include <casacore/casa/aips.h>

#include <casacore/scimath/Mathematics/ConstrainedRangeStatistics.h>

#include <set>
#include <vector>
#include <utility>

namespace casacore {

// Class to calculate statistics using the so-called hinges and fences algorithm. In this
// algorithm, the data on which the statistics are computed from is limited to the range
// of values between Q1 - f*D and Q3 + f*D, inclusive, where D = Q3 - Q1 and Q1 and Q3 are
// the first and third quartiles, respectively.

template <class AccumType, class DataIterator, class MaskIterator=const Bool *, class WeightsIterator=DataIterator>
class HingesFencesStatistics
    : public ConstrainedRangeStatistics<CASA_STATP> {
public:

    // If <src>f</src> is negative, the full dataset is used; ie the object has the same
    // behavior as a ClassicalStatistics object
    HingesFencesStatistics(Double f=-1.0);

    virtual ~HingesFencesStatistics();

    // copy semantics
    HingesFencesStatistics<CASA_STATP>& operator=(
        const HingesFencesStatistics<CASA_STATP>& other
    );

    // Clone this instance. Caller is responsible for deleting the returned pointer.
    virtual StatisticsAlgorithm<CASA_STATP>* clone() const;
    
    // get the algorithm that this object uses for computing stats
    virtual StatisticsData::ALGORITHM algorithm() const {
        return StatisticsData::HINGESFENCES;
    };

    // reset object to initial state. Clears all private fields including data,
    // accumulators, global range. It does not affect the fence factor (_f), which was
    // set at object construction.
    virtual void reset();

    // This class does not allow statistics to be calculated as datasets are added, so
    // an exception will be thrown if <src>c</src> is True.
    void setCalculateAsAdded(Bool c);

protected:
    // <group>
    // scan through the data set to determine the number of good (unmasked, weight > 0,
    // within range) points. The first with no mask, no
    // ranges, and no weights is trivial with npts = nr in this class, but is implemented here
    // so that derived classes may override it.
    inline void _accumNpts(
        uInt64& npts,
        const DataIterator& dataStart, Int64 nr, uInt dataStride
    ) const;

    void _accumNpts(
        uInt64& npts,
        const DataIterator& dataStart, Int64 nr, uInt dataStride,
        const DataRanges& ranges, Bool isInclude
    ) const;

    void _accumNpts(
        uInt64& npts,
        const DataIterator& dataBegin, Int64 nr, uInt dataStride,
        const MaskIterator& maskBegin, uInt maskStride
    ) const;

    void _accumNpts(
        uInt64& npts,
        const DataIterator& dataBegin, Int64 nr, uInt dataStride,
        const MaskIterator& maskBegin, uInt maskStride, const DataRanges& ranges,
        Bool isInclude
    ) const;

    void _accumNpts(
        uInt64& npts,
        const DataIterator& dataBegin, const WeightsIterator& weightsBegin,
        Int64 nr, uInt dataStride
    ) const;

    void _accumNpts(
        uInt64& npts,
        const DataIterator& dataBegin, const WeightsIterator& weightsBegin,
        Int64 nr, uInt dataStride, const DataRanges& ranges, Bool isInclude
    ) const;

    void _accumNpts(
        uInt64& npts,
        const DataIterator& dataBegin, const WeightsIterator& weightsBegin,
        Int64 nr, uInt dataStride, const MaskIterator& maskBegin, uInt maskStride,
        const DataRanges& ranges, Bool isInclude
    ) const;

    void _accumNpts(
        uInt64& npts,
        const DataIterator& dataBegin, const WeightsIterator& weightBegin,
        Int64 nr, uInt dataStride, const MaskIterator& maskBegin, uInt maskStride
    ) const;
    // </group>

    // <group>
    virtual void _findBins(
        vector<vector<uInt64> >& binCounts,
        vector<CountedPtr<AccumType> >& sameVal, vector<Bool>& allSame,
        const DataIterator& dataBegin, Int64 nr, uInt dataStride,
        const vector<typename StatisticsUtilities<AccumType>::BinDesc>& binDesc,
        const vector<AccumType>& maxLimit
    ) const;

    virtual void _findBins(
        vector<vector<uInt64> >& binCounts,
        vector<CountedPtr<AccumType> >& sameVal, vector<Bool>& allSame,
        const DataIterator& dataBegin, Int64 nr, uInt dataStride,
        const DataRanges& ranges, Bool isInclude,
        const vector<typename StatisticsUtilities<AccumType>::BinDesc>& binDesc, const vector<AccumType>& maxLimit
    ) const;

    virtual void _findBins(
        vector<vector<uInt64> >& binCounts,
        vector<CountedPtr<AccumType> >& sameVal, vector<Bool>& allSame,
        const DataIterator& dataBegin, Int64 nr, uInt dataStride,
        const MaskIterator& maskBegin, uInt maskStride,
        const vector<typename StatisticsUtilities<AccumType>::BinDesc>& binDesc, const vector<AccumType>& maxLimit
    ) const;

    virtual void _findBins(
        vector<vector<uInt64> >& binCounts,
        vector<CountedPtr<AccumType> >& sameVal, vector<Bool>& allSame,
        const DataIterator& dataBegin, Int64 nr, uInt dataStride,
        const MaskIterator& maskBegin, uInt maskStride, const DataRanges& ranges,
        Bool isInclude,
        const vector<typename StatisticsUtilities<AccumType>::BinDesc>& binDesc, const vector<AccumType>& maxLimit
    ) const;

    virtual void _findBins(
        vector<vector<uInt64> >& binCounts,
        vector<CountedPtr<AccumType> >& sameVal, vector<Bool>& allSame,
        const DataIterator& dataBegin, const WeightsIterator& weightsBegin,
        Int64 nr, uInt dataStride,
        const vector<typename StatisticsUtilities<AccumType>::BinDesc>& binDesc, const vector<AccumType>& maxLimit
    ) const ;

    virtual void _findBins(
        vector<vector<uInt64> >& binCounts,
        vector<CountedPtr<AccumType> >& sameVal, vector<Bool>& allSame,
        const DataIterator& dataBegin, const WeightsIterator& weightsBegin,
        Int64 nr, uInt dataStride, const DataRanges& ranges, Bool isInclude,
        const vector<typename StatisticsUtilities<AccumType>::BinDesc>& binDesc, const vector<AccumType>& maxLimit
    ) const;

    virtual void _findBins(
        vector<vector<uInt64> >& binCounts,
        vector<CountedPtr<AccumType> >& sameVal, vector<Bool>& allSame,
        const DataIterator& dataBegin, const WeightsIterator& weightsBegin,
        Int64 nr, uInt dataStride, const MaskIterator& maskBegin, uInt maskStride,
        const DataRanges& ranges, Bool isInclude,
        const vector<typename StatisticsUtilities<AccumType>::BinDesc>& binDesc, const vector<AccumType>& maxLimit
    ) const;

    virtual void _findBins(
        vector<vector<uInt64> >& binCounts,
        vector<CountedPtr<AccumType> >& sameVal, vector<Bool>& allSame,
        const DataIterator& dataBegin, const WeightsIterator& weightBegin,
        Int64 nr, uInt dataStride, const MaskIterator& maskBegin, uInt maskStride,
        const vector<typename StatisticsUtilities<AccumType>::BinDesc>& binDesc, const vector<AccumType>& maxLimit
    ) const;
    // </group>

    // <group>
    virtual void _minMax(
        CountedPtr<AccumType>& mymin, CountedPtr<AccumType>& mymax,
        const DataIterator& dataBegin, Int64 nr, uInt dataStride
    ) const;

    virtual void _minMax(
        CountedPtr<AccumType>& mymin, CountedPtr<AccumType>& mymax,
        const DataIterator& dataBegin, Int64 nr, uInt dataStride,
        const DataRanges& ranges, Bool isInclude
    ) const;

    virtual void _minMax(
        CountedPtr<AccumType>& mymin, CountedPtr<AccumType>& mymax,
        const DataIterator& dataBegin, Int64 nr, uInt dataStride,
        const MaskIterator& maskBegin, uInt maskStride
    ) const;

    virtual void _minMax(
        CountedPtr<AccumType>& mymin, CountedPtr<AccumType>& mymax,
        const DataIterator& dataBegin, Int64 nr, uInt dataStride,
        const MaskIterator& maskBegin, uInt maskStride, const DataRanges& ranges,
        Bool isInclude
    ) const;

    virtual void _minMax(
        CountedPtr<AccumType>& mymin, CountedPtr<AccumType>& mymax,
        const DataIterator& dataBegin, const WeightsIterator& weightsBegin,
        Int64 nr, uInt dataStride
    ) const;

    virtual void _minMax(
        CountedPtr<AccumType>& mymin, CountedPtr<AccumType>& mymax,
        const DataIterator& dataBegin, const WeightsIterator& weightsBegin,
        Int64 nr, uInt dataStride, const DataRanges& ranges, Bool isInclude
    ) const;

    virtual void _minMax(
        CountedPtr<AccumType>& mymin, CountedPtr<AccumType>& mymax,
        const DataIterator& dataBegin, const WeightsIterator& weightsBegin,
        Int64 nr, uInt dataStride, const MaskIterator& maskBegin, uInt maskStride,
        const DataRanges& ranges, Bool isInclude
    ) const;

    virtual void _minMax(
        CountedPtr<AccumType>& mymin, CountedPtr<AccumType>& mymax,
        const DataIterator& dataBegin, const WeightsIterator& weightBegin,
        Int64 nr, uInt dataStride, const MaskIterator& maskBegin, uInt maskStride
    ) const;
    // </group>

    //<group>
    // populate an unsorted array with valid data. If <src>includeLimits</src> is defined,
    // then restrict values that are entered in the array to those limits (inclusive of the
    // minimum, exclusive of the maximum). <src>maxCount</src> and <src>currentCount</src> are
    // used only if <src>includeLimits</src> is defined. In this case, the method will return
    // when currentCount == maxCount, thus avoiding scanning remaining data unnecessarily.

    // no weights, no mask, no ranges
    void _populateArray(
        vector<AccumType>& ary, const DataIterator& dataBegin, Int64 nr, uInt dataStride
    ) const;

    // ranges
    void _populateArray(
        vector<AccumType>& ary, const DataIterator& dataBegin, Int64 nr,
        uInt dataStride, const DataRanges& ranges, Bool isInclude
    ) const;

    void _populateArray(
        vector<AccumType>& ary, const DataIterator& dataBegin,
        Int64 nr, uInt dataStride, const MaskIterator& maskBegin,
        uInt maskStride
    ) const;

    // mask and ranges
    void _populateArray(
        vector<AccumType>& ary, const DataIterator& dataBegin, Int64 nr,
        uInt dataStride, const MaskIterator& maskBegin, uInt maskStride,
        const DataRanges& ranges, Bool isInclude
    ) const;

    // weights
    void _populateArray(
        vector<AccumType>& ary, const DataIterator& dataBegin,
        const WeightsIterator& weightsBegin, Int64 nr, uInt dataStride
    ) const;

    // weights and ranges
    void _populateArray(
        vector<AccumType>& ary, const DataIterator& dataBegin,
        const WeightsIterator& weightsBegin, Int64 nr, uInt dataStride,
        const DataRanges& ranges, Bool isInclude
    ) const;

    // weights and mask
    void _populateArray(
        vector<AccumType>& ary, const DataIterator& dataBegin,
        const WeightsIterator& weightBegin, Int64 nr, uInt dataStride,
        const MaskIterator& maskBegin, uInt maskStride
    ) const;

    // weights, mask, ranges
    void _populateArray(
        vector<AccumType>& ary, const DataIterator& dataBegin, const WeightsIterator& weightBegin,
        Int64 nr, uInt dataStride, const MaskIterator& maskBegin, uInt maskStride,
        const DataRanges& ranges, Bool isInclude
    ) const;

    // no weights, no mask, no ranges
    virtual void _populateArrays(
        vector<vector<AccumType> >& arys, uInt64& currentCount, const DataIterator& dataBegin, Int64 nr, uInt dataStride,
        const vector<std::pair<AccumType, AccumType> > &includeLimits, uInt64 maxCount
    ) const;

    // ranges
    virtual void _populateArrays(
        vector<vector<AccumType> >& arys, uInt64& currentCount, const DataIterator& dataBegin, Int64 nr,
        uInt dataStride, const DataRanges& ranges, Bool isInclude,
        const vector<std::pair<AccumType, AccumType> > &includeLimits, uInt64 maxCount
    ) const;

    virtual void _populateArrays(
        vector<vector<AccumType> >& arys, uInt64& currentCount, const DataIterator& dataBegin,
        Int64 nr, uInt dataStride, const MaskIterator& maskBegin,
        uInt maskStride,
        const vector<std::pair<AccumType, AccumType> > &includeLimits, uInt64 maxCount
    ) const;

    // mask and ranges
    virtual void _populateArrays(
        vector<vector<AccumType> >& arys, uInt64& currentCount, const DataIterator& dataBegin, Int64 nr,
        uInt dataStride, const MaskIterator& maskBegin, uInt maskStride,
        const DataRanges& ranges, Bool isInclude,
        const vector<std::pair<AccumType, AccumType> > &includeLimits, uInt64 maxCount
    ) const;

    // weights
    virtual void _populateArrays(
        vector<vector<AccumType> >& arys, uInt64& currentCount, const DataIterator& dataBegin,
        const WeightsIterator& weightsBegin, Int64 nr, uInt dataStride,
        const vector<std::pair<AccumType, AccumType> > &includeLimits, uInt64 maxCount
    ) const;

    // weights and ranges
    virtual void _populateArrays(
        vector<vector<AccumType> >& arys, uInt64& currentCount, const DataIterator& dataBegin,
        const WeightsIterator& weightsBegin, Int64 nr, uInt dataStride,
        const DataRanges& ranges, Bool isInclude,
        const vector<std::pair<AccumType, AccumType> > &includeLimits, uInt64 maxCount
    ) const;

    // weights and mask
    virtual void _populateArrays(
        vector<vector<AccumType> >& arys, uInt64& currentCount, const DataIterator& dataBegin,
        const WeightsIterator& weightBegin, Int64 nr, uInt dataStride,
        const MaskIterator& maskBegin, uInt maskStride,
        const vector<std::pair<AccumType, AccumType> > &includeLimits, uInt64 maxCount
    ) const;

    // weights, mask, ranges
    virtual void _populateArrays(
        vector<vector<AccumType> >& arys, uInt64& currentCount, const DataIterator& dataBegin, const WeightsIterator& weightBegin,
        Int64 nr, uInt dataStride, const MaskIterator& maskBegin, uInt maskStride,
        const DataRanges& ranges, Bool isInclude,
        const vector<std::pair<AccumType, AccumType> > &includeLimits, uInt64 maxCount
    ) const;
    // </group>

    // <group>
    // no weights, no mask, no ranges
    Bool _populateTestArray(
        vector<AccumType>& ary, const DataIterator& dataBegin,
        Int64 nr, uInt dataStride, uInt maxElements
    ) const;

    // ranges
    Bool _populateTestArray(
        vector<AccumType>& ary, const DataIterator& dataBegin, Int64 nr,
        uInt dataStride, const DataRanges& ranges, Bool isInclude,
        uInt maxElements
    ) const;

    // mask
    Bool _populateTestArray(
        vector<AccumType>& ary, const DataIterator& dataBegin,
        Int64 nr, uInt dataStride, const MaskIterator& maskBegin,
        uInt maskStride, uInt maxElements
    ) const;

    // mask and ranges
    Bool _populateTestArray(
        vector<AccumType>& ary, const DataIterator& dataBegin, Int64 nr,
        uInt dataStride, const MaskIterator& maskBegin, uInt maskStride,
        const DataRanges& ranges, Bool isInclude, uInt maxElements
    ) const;

    // weights
    Bool _populateTestArray(
        vector<AccumType>& ary, const DataIterator& dataBegin,
        const WeightsIterator& weightBegin, Int64 nr, uInt dataStride,
        uInt maxElements
    ) const;

    // weights and ranges
    Bool _populateTestArray(
        vector<AccumType>& ary, const DataIterator& dataBegin,
        const WeightsIterator& weightsBegin, Int64 nr, uInt dataStride,
        const DataRanges& ranges, Bool isInclude, uInt maxElements
    ) const;

    // weights and mask
    Bool _populateTestArray(
        vector<AccumType>& ary, const DataIterator& dataBegin,
        const WeightsIterator& weightBegin, Int64 nr,
        uInt dataStride, const MaskIterator& maskBegin,
        uInt maskStride, uInt maxElements
    ) const;

    // weights, mask, ranges
    Bool _populateTestArray(
        vector<AccumType>& ary, const DataIterator& dataBegin, const WeightsIterator& weightBegin,
        Int64 nr, uInt dataStride, const MaskIterator& maskBegin, uInt maskStride,
        const DataRanges& ranges, Bool isInclude,
        uInt maxElements
    ) const;
    // </group>

    // <group>
    // no weights, no mask, no ranges
    void _unweightedStats(
        StatsData<AccumType>& stats, uInt64& ngood, LocationType& location,
        const DataIterator& dataBegin, Int64 nr, uInt dataStride
    );

    // no weights, no mask
    void _unweightedStats(
        StatsData<AccumType>& stats, uInt64& ngood, LocationType& location,
        const DataIterator& dataBegin, Int64 nr, uInt dataStride,
        const DataRanges& ranges, Bool isInclude
    );

    void _unweightedStats(
        StatsData<AccumType>& stats, uInt64& ngood, LocationType& location,
        const DataIterator& dataBegin, Int64 nr, uInt dataStride,
        const MaskIterator& maskBegin, uInt maskStride
    );

    void _unweightedStats(
        StatsData<AccumType>& stats, uInt64& ngood, LocationType& location,
        const DataIterator& dataBegin, Int64 nr, uInt dataStride,
        const MaskIterator& maskBegin, uInt maskStride,
        const DataRanges& ranges, Bool isInclude
    );
    // </group>

    // <group>
    // has weights, but no mask, no ranges
    void _weightedStats(
        StatsData<AccumType>& stats, LocationType& location,
        const DataIterator& dataBegin, const WeightsIterator& weightsBegin,
        Int64 nr, uInt dataStride
    );

    void _weightedStats(
        StatsData<AccumType>& stats, LocationType& location,
        const DataIterator& dataBegin, const WeightsIterator& weightsBegin,
        Int64 nr, uInt dataStride, const DataRanges& ranges, Bool isInclude
    );

    void _weightedStats(
        StatsData<AccumType>& stats, LocationType& location,
        const DataIterator& dataBegin, const WeightsIterator& weightBegin,
        Int64 nr, uInt dataStride, const MaskIterator& maskBegin, uInt maskStride
    );

    void _weightedStats(
        StatsData<AccumType>& stats, LocationType& location,
        const DataIterator& dataBegin, const WeightsIterator& weightBegin,
        Int64 nr, uInt dataStride, const MaskIterator& maskBegin, uInt maskStride,
        const DataRanges& ranges, Bool isInclude
    );
    // </group>

private:

    // _f defined in inclusion range between Q1 - _f*D and Q3 + _f*D, where D = Q3 - Q1 and
    // Q1 and Q3 are the first and third quartiles, respectively
    Double _f;
    Bool _rangeIsSet, _hasRange;

    void _setRange();

};

}

#ifndef CASACORE_NO_AUTO_TEMPLATES
#include <casacore/scimath/Mathematics/HingesFencesStatistics.tcc>
#endif //# CASACORE_NO_AUTO_TEMPLATES

#endif
