#include <algorithm>
#include <cmath>
#include <limits>

#include <QHash>
#include <QList>

#include "rfl_date_estimator.h"
#include "rfl_utils.h"

namespace
{

const double infinity = std::numeric_limits<double>::infinity();
const double noDay = std::numeric_limits<double>::quiet_NaN();

// Typical values [years] used to pick the most likely date inside the possible range.
const double typicalMotherAge = 28.0;
const double typicalFatherAge = 32.0;
const double typicalHusbandSeniority = 3.0;
const double typicalMarriageToChild = 5.0;
const double typicalLifespan = 70.0;

// Tolerance of a recorded fuzzy date [years].
const double fuzzyDateSlack = 10.0;
// Child can be born this long after father's death or after end of parents relation [days].
const double gestation = 300.0;

const int maxBoundPasses = 1000;
const int maxLikelyPasses = 500;

inline double years(double nYears)
{
    return nYears * 365.25;
}

//! Range of possible julian days.
struct Range
{
    double low = -infinity;
    double high = infinity;
    //! Exact recorded date, limits can not be moved.
    bool fixed = false;

    bool isConflicting() const
    {
        return this->low > this->high;
    }

    bool raise(double value)
    {
        if (this->fixed || this->isConflicting() || !(value > this->low))
        {
            return false;
        }
        this->low = value;
        return true;
    }

    bool lower(double value)
    {
        if (this->fixed || this->isConflicting() || !(value < this->high))
        {
            return false;
        }
        this->high = value;
        return true;
    }
};

struct Node
{
    int sex = FPerson::SexIndex::undefined;
    Range birth;
    Range death;
    //! Recorded birth or its most likely day.
    double likelyBirth = noDay;
    bool birthRecorded = false;
    double recordedDeath = noDay;
    //! Relations where person is a partner.
    QList<qsizetype> partnerLinks;
    //! Relations where person is a child.
    QList<qsizetype> childLinks;
};

struct Link
{
    QList<qsizetype> partners;
    QList<qsizetype> children;
    Range begin;
    Range end;
    double recordedBegin = noDay;
};

//! Convert recorded date to a range of possible days and return the recorded day.
double readDate(const FDate &date, Range &range)
{
    if (date.isEmpty() || !date.isValid())
    {
        return noDay;
    }
    // Date without day or month covers the whole month or year.
    double earliest = double(date.toDate().toJulianDay());
    double latest = double(date.toLatestDate().toJulianDay());
    double slack = date.getFuzzy() ? years(fuzzyDateSlack) : 0.0;
    range.low = earliest - slack;
    range.high = latest + slack;
    range.fixed = !date.getFuzzy();
    return (earliest + latest) / 2.0;
}

double maxParentAge(int sex)
{
    return years(sex == FPerson::SexIndex::female ? FConstants::maxReproductiveAgeFemale : FConstants::maxReproductiveAgeMale);
}

//! How long after parent's death a child can be born.
double posthumousBirth(int sex)
{
    return sex == FPerson::SexIndex::female ? 0.0 : gestation;
}

double typicalParentAge(int sex)
{
    if (sex == FPerson::SexIndex::female)
    {
        return years(typicalMotherAge);
    }
    if (sex == FPerson::SexIndex::male)
    {
        return years(typicalFatherAge);
    }
    return years((typicalMotherAge + typicalFatherAge) / 2.0);
}

//! How much later than the partner a person is typically born.
double typicalBirthAfterPartner(int sex, int partnerSex)
{
    if (sex == FPerson::SexIndex::female && partnerSex == FPerson::SexIndex::male)
    {
        return years(typicalHusbandSeniority);
    }
    if (sex == FPerson::SexIndex::male && partnerSex == FPerson::SexIndex::female)
    {
        return -years(typicalHusbandSeniority);
    }
    return 0.0;
}

//! Narrow ranges of possible birth and death days of all persons until nothing changes.
void propagateBounds(QList<Node> &nodes, const QList<Link> &links)
{
    const double maxAge = years(FConstants::maxAge);
    const double minParentAge = years(FConstants::minReproductiveAge);
    const double minMarriageAge = years(FConstants::minMarriageAge);
    // Children of one relation share a mother.
    const double maxSiblingGap = years(FConstants::maxReproductiveAgeFemale - FConstants::minReproductiveAge);

    bool changed = true;
    for (int pass = 0; changed && pass < maxBoundPasses; pass++)
    {
        changed = false;

        for (Node &node : nodes)
        {
            changed |= node.death.raise(node.birth.low);
            changed |= node.death.lower(node.birth.high + maxAge);
            changed |= node.birth.raise(node.death.low - maxAge);
            changed |= node.birth.lower(node.death.high);
        }

        for (const Link &link : links)
        {
            for (qsizetype partnerIndex : link.partners)
            {
                Node &partner = nodes[partnerIndex];

                // Partner is an adult when the relation begins and alive until it ends.
                changed |= partner.birth.lower(link.begin.high - minMarriageAge);
                changed |= partner.birth.lower(link.end.high - minMarriageAge);
                changed |= partner.death.raise(link.begin.low);
                changed |= partner.death.raise(link.end.low);

                // Partners are adults alive at the same time.
                for (qsizetype otherIndex : link.partners)
                {
                    if (otherIndex != partnerIndex)
                    {
                        changed |= partner.birth.lower(nodes[otherIndex].death.high - minMarriageAge);
                        changed |= partner.death.raise(nodes[otherIndex].birth.low + minMarriageAge);
                    }
                }

                for (qsizetype childIndex : link.children)
                {
                    Node &child = nodes[childIndex];

                    changed |= child.birth.raise(partner.birth.low + minParentAge);
                    changed |= child.birth.lower(partner.birth.high + maxParentAge(partner.sex));
                    changed |= child.birth.lower(partner.death.high + posthumousBirth(partner.sex));

                    changed |= partner.birth.lower(child.birth.high - minParentAge);
                    changed |= partner.birth.raise(child.birth.low - maxParentAge(partner.sex));
                    changed |= partner.death.raise(child.birth.low - posthumousBirth(partner.sex));
                }
            }

            double latestLow = -infinity;
            double earliestHigh = infinity;
            for (qsizetype childIndex : link.children)
            {
                latestLow = std::max(latestLow,nodes[childIndex].birth.low);
                earliestHigh = std::min(earliestHigh,nodes[childIndex].birth.high);
            }
            for (qsizetype childIndex : link.children)
            {
                Node &child = nodes[childIndex];

                changed |= child.birth.raise(latestLow - maxSiblingGap);
                changed |= child.birth.lower(earliestHigh + maxSiblingGap);
                changed |= child.birth.lower(link.end.high + gestation);
            }
        }
    }
}

//! Earliest day at which a relation is known to exist.
double linkEpoch(const Link &link, const QList<Node> &nodes)
{
    double epoch = link.recordedBegin;
    for (qsizetype childIndex : link.children)
    {
        epoch = std::fmin(epoch,nodes[childIndex].likelyBirth);
    }
    return epoch;
}

//! Find most likely birth day of every person which has no birth date recorded.
//! Each one is a weighted average of typical distances from its relatives, kept inside its possible range.
void relaxLikelyBirths(QList<Node> &nodes, const QList<Link> &links)
{
    for (int pass = 0; pass < maxLikelyPasses; pass++)
    {
        double maxShift = 0.0;

        for (qsizetype i = 0; i < nodes.size(); i++)
        {
            Node &node = nodes[i];
            if (node.birthRecorded)
            {
                continue;
            }

            double sum = 0.0;
            double weight = 0.0;
            auto add = [&sum,&weight](double day, double dayWeight)
            {
                if (!std::isnan(day))
                {
                    sum += day * dayWeight;
                    weight += dayWeight;
                }
            };

            for (qsizetype linkIndex : node.childLinks)
            {
                const Link &link = links[linkIndex];
                for (qsizetype parentIndex : link.partners)
                {
                    add(nodes[parentIndex].likelyBirth + typicalParentAge(nodes[parentIndex].sex),1.0);
                }
                for (qsizetype siblingIndex : link.children)
                {
                    if (siblingIndex != i)
                    {
                        add(nodes[siblingIndex].likelyBirth,1.0);
                    }
                }
                add(link.recordedBegin + years(typicalMarriageToChild),0.5);
            }

            double firstEpoch = noDay;
            for (qsizetype linkIndex : node.partnerLinks)
            {
                firstEpoch = std::fmin(firstEpoch,linkEpoch(links[linkIndex],nodes));
            }

            for (qsizetype linkIndex : node.partnerLinks)
            {
                const Link &link = links[linkIndex];
                for (qsizetype childIndex : link.children)
                {
                    add(nodes[childIndex].likelyBirth - typicalParentAge(node.sex),1.0);
                }
                // Typical age at marriage and typical age difference of partners do not hold for later relations.
                if (linkEpoch(link,nodes) > firstEpoch)
                {
                    continue;
                }
                for (qsizetype partnerIndex : link.partners)
                {
                    if (partnerIndex != i)
                    {
                        add(nodes[partnerIndex].likelyBirth + typicalBirthAfterPartner(node.sex,nodes[partnerIndex].sex),1.0);
                    }
                }
                add(link.recordedBegin + years(typicalMarriageToChild) - typicalParentAge(node.sex),1.0);
            }

            add(node.recordedDeath - years(typicalLifespan),0.25);

            if (weight == 0.0)
            {
                continue;
            }

            double day = sum / weight;
            if (!node.birth.isConflicting())
            {
                day = std::clamp(day,node.birth.low,node.birth.high);
            }

            maxShift = std::max(maxShift,std::isnan(node.likelyBirth) ? infinity : std::abs(day - node.likelyBirth));
            node.likelyBirth = day;
        }

        if (maxShift < 1.0)
        {
            break;
        }
    }
}

//! Fuzzy date to be suggested, only a year unless the possible range is shorter than that.
FDate suggestDate(double day, const Range &range)
{
    QDate date(QDate::fromJulianDay(qRound64(std::clamp(day,range.low,range.high))));
    if (range.high - range.low < years(1.0))
    {
        return FDate(date,true);
    }
    return FDate(0,0,date.year(),true);
}

void setLimits(FDateEstimate &estimate, const Range &range, double today)
{
    if (std::isfinite(range.low))
    {
        estimate.low = FDate(QDate::fromJulianDay(qint64(std::ceil(range.low))));
    }
    if (range.high < today)
    {
        estimate.high = FDate(QDate::fromJulianDay(qint64(std::floor(range.high))));
    }
}

} // namespace

void FDateEstimator::estimate(const QMap<QUuid,FPerson> &persons,
                              const QMap<QUuid,FRelation> &relations,
                              const QUuid &personId,
                              FDateEstimate &birth,
                              FDateEstimate &death)
{
    birth = FDateEstimate();
    death = FDateEstimate();

    if (!persons.contains(personId))
    {
        return;
    }

    const double today = double(QDate::currentDate().toJulianDay());

    QHash<QUuid,qsizetype> index;
    QList<Node> nodes;
    index.reserve(persons.size());
    nodes.reserve(persons.size());

    for (auto iter = persons.cbegin(); iter != persons.cend(); ++iter)
    {
        const FDate &birthDate = iter.value().getBirth().getDate();
        const FDate &deathDate = iter.value().getDeath().getDate();
        bool isEstimated = (iter.key() == personId);

        Node node;
        node.sex = FPerson::sexDescToIndex(iter.value().getSex());
        if (!isEstimated || !birthDate.getFuzzy())
        {
            node.likelyBirth = readDate(birthDate,node.birth);
            node.birthRecorded = !std::isnan(node.likelyBirth);
        }
        if (!isEstimated || !deathDate.getFuzzy())
        {
            node.recordedDeath = readDate(deathDate,node.death);
        }
        node.birth.lower(today);

        index.insert(iter.key(),nodes.size());
        nodes.append(node);
    }

    QList<Link> links;
    links.reserve(relations.size());

    for (auto iter = relations.cbegin(); iter != relations.cend(); ++iter)
    {
        Link link;
        link.recordedBegin = readDate(iter.value().getBegin().getDate(),link.begin);
        readDate(iter.value().getEnd().getDate(),link.end);

        for (const QUuid &partnerId : iter.value().getPartners())
        {
            auto found = index.constFind(partnerId);
            if (found != index.cend())
            {
                link.partners.append(found.value());
                nodes[found.value()].partnerLinks.append(links.size());
            }
        }
        for (const QUuid &childId : iter.value().getChildren())
        {
            auto found = index.constFind(childId);
            if (found != index.cend())
            {
                link.children.append(found.value());
                nodes[found.value()].childLinks.append(links.size());
            }
        }

        links.append(link);
    }

    propagateBounds(nodes,links);
    relaxLikelyBirths(nodes,links);

    const Node &node = nodes[index.value(personId)];
    const FPerson &person = persons[personId];
    double birthDay = node.likelyBirth;

    if (node.birth.fixed)
    {
        birth.status = FDateEstimate::Known;
        birth.date = person.getBirth().getDate();
    }
    else if (node.birth.isConflicting())
    {
        birth.status = FDateEstimate::Conflict;
    }
    else if (!std::isnan(birthDay))
    {
        birth.status = FDateEstimate::Estimated;
        birth.date = suggestDate(birthDay,node.birth);
        setLimits(birth,node.birth,today);
        birthDay = std::clamp(birthDay,node.birth.low,node.birth.high);
    }

    if (node.death.fixed)
    {
        death.status = FDateEstimate::Known;
        death.date = person.getDeath().getDate();
    }
    else if (birth.status == FDateEstimate::Conflict || node.death.isConflicting())
    {
        death.status = FDateEstimate::Conflict;
    }
    // Death is suggested only if the person can not be alive anymore.
    else if (!std::isnan(birthDay) && node.death.high < today)
    {
        // Remaining life gets shorter with the age at which the person is last known to be alive.
        double lastAlive = std::max(node.death.low,birthDay);
        double age = lastAlive - birthDay;
        double remaining = std::max(years(typicalLifespan) - age,(years(FConstants::maxAge) - age) / 4.0);

        death.status = FDateEstimate::Estimated;
        death.date = suggestDate(lastAlive + remaining,node.death);
        setLimits(death,node.death,today);
    }
}
