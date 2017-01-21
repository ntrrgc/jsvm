//
// Created by ntrrgc on 1/21/17.
//

#ifndef JSVM_EXCEPTIONS_H
#define JSVM_EXCEPTIONS_H

namespace jsvm {

    enum may_throw {
        OK = 0,
        THREW_EXCEPTION = -1,
        STATUS_UNDEFINED = -2
    };

    template<class T> struct Result {
        Result(): m_status(STATUS_UNDEFINED) {}

        may_throw status() const {
            return m_status;
        }

        T& get() const {
            assert(m_status == OK);
            return m_valueUnion.value;
        }

        T& get() {
            assert(m_status == OK);
            return m_valueUnion.value;
        }

        static Result<T> createOK(const T &value) {
            Result<T> result;
            result.m_status = OK;
            result.m_valueUnion.value = value;
            return result;
        }

        static Result<T> createThrew() {
            Result<T> result;
            result.m_status = THREW_EXCEPTION;
            return result;
        }

    private:
        union ValueUnion {
            T value;
        };

        may_throw m_status;
        ValueUnion m_valueUnion;
    };

}

#endif //JSVM_EXCEPTIONS_H
