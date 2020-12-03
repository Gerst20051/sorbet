#ifndef RUBY_TYPER_NAMEREF_H
#define RUBY_TYPER_NAMEREF_H
#include "common/common.h"
#include "core/DebugOnlyCheck.h"
namespace sorbet::core {
class GlobalState;
class GlobalSubstitution;
class Name;

struct NameDataDebugCheck {
    const GlobalState &gs;
    const unsigned int nameCountAtCreation;

    NameDataDebugCheck(const GlobalState &gs);
    void check() const;
};

/** This is to `NameRef &` what SymbolData is to `SymbolRef &`. Read docs on SymbolData */
class NameData : private DebugOnlyCheck<NameDataDebugCheck> {
    Name &name;

public:
    NameData(Name &ref, const GlobalState &gs);
    Name *operator->();
    const Name *operator->() const;
};
constexpr size_t sizeof__Name = sizeof(Name *);
constexpr size_t alignof__Name = alignof(Name *);
CheckSize(NameData, sizeof__Name, alignof__Name);

struct NameRefDebugCheck {
    int globalStateId;

    constexpr NameRefDebugCheck() : globalStateId(-1) {}

    NameRefDebugCheck(const GlobalState &gs, u4 id);

    void check(const GlobalState &gs, u4 id) const;
    void check(const GlobalSubstitution &subst) const;
};

class NameRef final : private DebugOnlyCheck<NameRefDebugCheck> {
public:
    friend GlobalState;
    friend Name;

    NameRef() : _id(0){};

    // WellKnown is a tag to statically indicate that the caller is deliberately
    // constructing a well-known name, whose ID is stable across all
    // GlobalStates. This should never be used outside of the name constructors
    // generated by tools/generate_names.cc
    struct WellKnown {};

    constexpr NameRef(WellKnown, u4 id) : _id{id} {}

    NameRef(const GlobalState &gs, u4 id);

    NameRef(const NameRef &nm) = default;

    NameRef(NameRef &&nm) = default;

    NameRef &operator=(const NameRef &rhs) = default;

    bool operator==(const NameRef &rhs) const {
        return _id == rhs._id;
    }

    bool operator!=(const NameRef &rhs) const {
        return !(rhs == *this);
    }

    inline u4 id() const {
        return _id;
    }

    NameData data(GlobalState &gs) const;

    const NameData data(const GlobalState &gs) const;

    // Returns the `0` NameRef, used to indicate non-existence of a name
    static NameRef noName() {
        return NameRef(WellKnown{}, 0);
    }

    inline bool exists() const {
        return _id != 0;
    }

    NameRef addEq(GlobalState &gs) const;
    NameRef addQuestion(GlobalState &gs) const;

    NameRef addAt(GlobalState &gs) const;

    NameRef prepend(GlobalState &gs, std::string_view s) const;

    NameRef lookupMangledPackageName(const GlobalState &gs) const;

    bool isClassName(const GlobalState &gs) const;

    // Convenience method, because enums need to be special cased in more places than other kinds of
    // unique names, and everyone always forget to unwrap the first layer (NameKind::CONSTANT)
    // before checking for UniqueNameKind::TEnum.
    bool isTEnumName(const GlobalState &gs) const;

    unsigned int hash(const GlobalState &gs) const;

    std::string_view shortName(const GlobalState &gs) const;
    std::string showRaw(const GlobalState &gs) const;
    std::string toString(const GlobalState &gs) const;
    std::string show(const GlobalState &gs) const;

    void enforceCorrectGlobalState(const GlobalState &gs) const;
    void sanityCheckSubstitution(const GlobalSubstitution &subst) const;
    void sanityCheck(const GlobalState &gs) const;

public:
    u4 _id;
};
CheckSize(NameRef, 4, 4);

template <typename H> H AbslHashValue(H h, const NameRef &m) {
    return H::combine(std::move(h), m.id());
}
} // namespace sorbet::core

#endif // RUBY_TYPER_NAMEREF_H
