#pragma once

#include <memory>
#include <stdint.h>
#include "utils/UtilsSpdlog.h"

template <typename T>
class Stream {
public:
    using Ptr = std::shared_ptr<Stream<T>>;

    Stream(const T* stream, size_t size) {
        if (size <= 0) { 
            THROW("Stream size[{}] <= 0", size);
        }
        CalcSize(size);
        m_msgSize = size;
        memcpy(m_pStream, stream, size*sizeof(T));
    }

    Stream(size_t size) {
        CalcSize(size);
        m_msgSize = 0;
    }

    Stream(const Stream& stream) {
        CalcSize(stream.m_size);
        m_msgSize = stream.m_msgSize;
        memcpy(m_pStream, stream, m_size * sizeof(T));
    }

    Stream& operator=(const Stream& stream) {
        CalcSize(stream.m_size);
        m_msgSize = stream.m_msgSize;
        memcpy(m_pStream, stream, m_size * sizeof(T));
        return *this;
    }

    ~Stream() {
        if (m_pStreamBegin) delete m_pStreamBegin;
    }

    Ptr Copy() {
        Ptr p = std::make_shared<Stream<T>>(m_pStream, m_msgSize);
        return p;
    }

    int SetData(T* data, size_t size) {
        if (size > m_size) {
            delete m_pStreamBegin;
            CalcSize(size);
            m_msgSize = size;
            memcpy(m_pStream, data, size * sizeof(T));
        }
        else {
            memcpy(m_pStream, data, size * sizeof(T));
        }

        return 0;
    }

    size_t PlusHeader(T* header, size_t size) {
        m_pStream -= size;
        m_msgSize += size;
        memcpy(m_pStream, header, size);
        return m_msgSize;
    }

    T* MinusHeader(size_t size) {
        T* p = m_pStream;
        m_pStream += size;
        m_msgSize -= size;
        return p;
    }

    T* Data() { return m_pStream; }
    size_t StreamSize() { return m_size; }
    size_t Size() { return m_msgSize; }
    size_t SetSize(size_t size) { m_msgSize = size; return size; }

private:
    size_t CalcSize(size_t size) {
        m_size = (size + 1024 - 1) / 1024 * 1024;
        m_pStreamBegin = new T[size+1024]; // 预留了一些空间
        m_pStream = m_pStreamBegin + 1024;
        return m_size;
    }

private:
    T* m_pStreamBegin = nullptr;
    T* m_pStream = nullptr;
    size_t m_msgSize = 0;
    size_t m_size = 0;
};

using I8Stream = Stream<char>;
using U8Stream = Stream<unsigned char>;
