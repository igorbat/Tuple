//
// Created by IgorBat on 12.05.2018.
//

#ifndef TASK3_TUPLE_H
#define TASK3_TUPLE_H
//for defenitions
template <typename... T>
class Tuple;


//tuple without any elements
template <>
class Tuple<> {
    template <typename... T>
    friend class Tuple<>;

    friend class TupleOperations_;

    static const bool IsNotEmpty = false;
public:
    Tuple() = default;
    ~Tuple() = default;

    Tuple<>& operator = (const Tuple<>& other) {}
    Tuple<>& operator = (Tuple<>&& other) noexcept {}
    Tuple<>(const Tuple<>& tuple){}
    Tuple<>(Tuple<>&& tuple){}

    template<typename U>
    void swap(U&& other) {
        if(other.IsNotEmpty)
            throw std::out_of_range("Swap Tuples with different sizes");
    }


    bool operator < (const Tuple& another) const {
        return false;
    }

    bool operator <= (const Tuple& another) const {
        return true;
    }

    bool operator > (const Tuple& another) const {
        return false;
    }
    bool operator >= (const Tuple& another) const {
        return true;
    }

    bool operator == (const Tuple& another) const {
        return true;
    }

    bool operator != (const Tuple& another) const {
        return false;
    }


};


//tuple with elements
template <class First, class... Others>
class Tuple<First, Others...> {
private:
    friend struct TupleOperations_;

    static const bool IsNotEmpty = true;

    First value_;
    Tuple<Others...>* others_;



public:

    //1)
    Tuple() :
            value_(First()),
            others_(new Tuple<Others...>()) {

    }

    //2)
    template<typename... Args>
    explicit Tuple<First, Others...>(const First& value, Args&&... args):
            value_(value),
            others_(new Tuple<Others...>(std::forward<Args>(args)...)) {}

    //3)
    template<typename... Args>
    explicit Tuple<First, Others...>(First&& value, Args&&... args):
            value_(std::move(value)),
            others_(new Tuple<Others...>(std::forward<Args>(args)...)) {}

    //4)
    Tuple<First, Others...>(Tuple<First, Others...>&& tuple) noexcept {
        value_ = std::move(tuple.value_);
        others_ = tuple.others_;
        tuple.others_ = nullptr;
    }

    //4)
    Tuple<First, Others...>(const Tuple<First, Others...>& tuple):
            value_(tuple.value_),
            others_(new Tuple<Others...>(*tuple.others_)) {}

    //4)
    Tuple<First, Others...>& operator= (const Tuple<First, Others...>& tuple) {
        value_ = tuple.value_;
        others_ = new Tuple<Others...>(*tuple.others_);
        return *this;
    }

    //4)
    Tuple<First, Others...>& operator= (Tuple<First, Others...>&& tuple) noexcept {
        value_ = std::move(tuple.value_);
        others_ = tuple.others_;
        tuple.others_ = nullptr;
        return *this;
    }

    //4)
    ~Tuple<First, Others...>() {
        delete others_;
    };

    //5
    template<typename Another>
    void swap(Another&& anotuple) {
        std::swap(value_, anotuple.value_);
        others_ -> swap(std::forward<Tuple<Others...>>(*anotuple.others_));
    }

    const First& getValue() const & {
        return value_;
    }

    First && getValue() &&{
        return std::move(value_);
    }

    auto getOthers() const &{
        return others_;
    }

    First& getValue() &{
        return value_;
    }

    //10
    bool operator < (const Tuple& another) const {
        if(value_ != another.value_)
            return value_ < another.value_;
        return *others_ < *another.others_;
    }

    bool operator == (const Tuple& another) const {
        if(value_ != another.value_)
            return false;
        return *others_ == *another.others_;
    }

    bool operator <= (const Tuple& another) const {
        return (*this < another) || (*this == another);
    }

    bool operator > (const Tuple& another) const {
        return !(this <= another);
    }

    bool operator >=(const Tuple& another) const {
        return !(*this < another);
    }

    bool operator != (const Tuple& another) const {
        return !(*this == another);
    }

};

/*template <typename First, typename ...args>
Tuple<First, args...> makeTuple(First& value, args&&... others){

};
 Problem: if there will be Maketuple(empty)???
 std::decay help to  do all things,|work with const char*, for example|
*/
//6)
template <typename... Args>
Tuple<typename std::decay<Args>::type...> makeTuple(Args&&... args) {
    return Tuple<typename std::decay<Args>::type...> (std::forward<Args>(args)...);
}

struct TupleOperations_{
public:
    template <size_t t, typename First, typename... Others>
    struct take_index{
        static decltype(auto) take(Tuple<First, Others...> &tuple) {
            return take_index<t - 1, Others...>::take(*tuple.others_);
        }

        static decltype(auto) take(const Tuple<First, Others...> &tuple) {
            return take_index<t - 1, Others...>::take(const_cast<const Tuple<Others...> & > (*tuple.others_));
        }

        static decltype(auto) take(Tuple<First, Others...> &&tuple) {
            return take_index<t - 1, Others...>::take(std::forward<Tuple<Others...>>(*tuple.others_));
        }

    };
    template <typename First, typename... Others>
    struct take_index<0, First, Others...> {
        static decltype(auto) take(Tuple<First, Others...>& tuple) {
            return tuple.getValue();
        }

        static decltype(auto) take(const Tuple<First, Others...>& tuple) {
            return tuple.getValue();
        }

        static decltype(auto) take(Tuple<First, Others...>&& tuple) {
            return std::forward<First>(tuple.getValue());
        }
    };


    template <typename T, typename First, typename... Others>
    struct take_type{
        static decltype(auto) take_t(Tuple<First, Others...>& tuple) {
            return take_type<T, Others...>::take_t(*tuple.others_);
        }

        static decltype(auto) take_t(const Tuple<First, Others...>& tuple) {
            return take_type<T, Others...>::take_t(const_cast<const Tuple <Others...>& > (*tuple.others_));
        }

        static decltype(auto) take_t(Tuple<First, Others...>&& tuple) {
            return take_type<T, Others...>::take_t(std::forward<Tuple <Others...>>(*tuple.others_));
        }
    };

    template <typename First, typename... Others>
    struct take_type<First, First, Others...>{
        static decltype(auto) take_t(Tuple<First, Others...>& tuple) {
            return tuple.getValue();
        }

        static decltype(auto) take_t(const Tuple<First, Others...>& tuple) {
            return tuple.getValue();
        }

        static decltype(auto) take_t(Tuple<First, Others...>&& tuple) {
            return std::forward<First>(tuple.getValue());
        }
    };
    template <typename... Second>
    static auto tupleCat(const Tuple<> &first, const Tuple<Second...> &second) {
        return second;
    }

    template <typename First, typename... Others, typename... Second>
    static auto tupleCat(const Tuple<First, Others...>& first, const Tuple<Second...>& second) {
        Tuple<First, Others..., Second...> newTuple;
        newTuple.others_ = new Tuple<Others..., Second...>
                (tupleCat(const_cast<Tuple<Others...>&>(*(first.getOthers())), second));
        newTuple.value_ = first.value_;
        return newTuple;
    }

    template <typename FirstTuple, typename SecondTuple, class... Others>
    static auto tupleCat(const FirstTuple& first, const SecondTuple& second, const Others &...back) {
        return tupleCat(tupleCat(first, second), back...);
    }

};


//7)
template <size_t t, typename... Others>
decltype(auto) get(Tuple<Others...>& tuple) {
    return TupleOperations_::take_index<t, Others...>::take(tuple);
}

template <size_t t, typename... Others>
decltype(auto) get(const Tuple<Others...>& tuple) {
    return TupleOperations_::take_index<t, Others...>::take(tuple);
}

template <size_t t, typename... Others>
decltype(auto) get(Tuple<Others...>&& tuple) {
    return TupleOperations_::take_index<t, Others...>::take(std::move(tuple));
}

//8)
template <typename T, typename... Others>
decltype(auto) get(Tuple<Others...>& tuple) {
    return TupleOperations_::take_type<T, Others...>::take_t(tuple);
}

template <typename T, typename... Others>
decltype(auto) get(const Tuple<Others...>& tuple) {
    return TupleOperations_::take_type<T, Others...>::take_t(tuple);
}

template <typename T, typename... Others>
decltype(auto) get(Tuple<Others...>&& tuple) {
    return TupleOperations_::take_type<T, Others...>::take_t(std::move(tuple));
}

//9)

template <typename FirstTuple, typename SecondTuple, class... Others>
auto tupleCat(const FirstTuple& first, const SecondTuple& second, const Others &...back){
    return TupleOperations_::tupleCat(first, second, back...);
};
#endif //TASK3_TUPLE_H
