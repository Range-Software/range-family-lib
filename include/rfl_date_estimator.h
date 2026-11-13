#ifndef RFL_DATE_ESTIMATOR_H
#define RFL_DATE_ESTIMATOR_H

#include <QMap>
#include <QUuid>

#include "rfl_date.h"
#include "rfl_person.h"
#include "rfl_relation.h"

class FDateEstimate
{

    public:

        enum Status
        {
            //! Nothing can be suggested.
            Unknown = 0,
            //! Date is recorded and it is not fuzzy.
            Known,
            //! Date was estimated from dates of relatives.
            Estimated,
            //! Dates of relatives contradict each other.
            Conflict
        };

        //! Status.
        Status status = Unknown;
        //! Recorded or suggested date.
        FDate date;
        //! Earliest possible date, empty if there is no limit.
        FDate low;
        //! Latest possible date, empty if there is no limit.
        FDate high;

};

class FDateEstimator
{

    public:

        //! Estimate birth and death dates of given person from dates recorded in the whole tree.
        //! Fuzzy dates of given person are treated as missing.
        static void estimate(const QMap<QUuid,FPerson> &persons,
                             const QMap<QUuid,FRelation> &relations,
                             const QUuid &personId,
                             FDateEstimate &birth,
                             FDateEstimate &death);

};

#endif // RFL_DATE_ESTIMATOR_H
